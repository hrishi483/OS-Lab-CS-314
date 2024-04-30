#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct Pixel {
    int r, g, b;
};

struct Sema{
    sem_t semaphore;
    Sema() {
        sem_init(&semaphore, 1, 0);
    }
    ~Sema() {
        sem_destroy(&semaphore);
    }
    void wait() {
        sem_wait(&semaphore);
    }
    void signal() {
        sem_post(&semaphore);
    }

};


vector<vector<Pixel>> read_ppm(const string& filename, int& width, int& height, int& max_color) {
    ifstream file(filename);
    string line;
    getline(file, line); 
    file >> width >> height >> max_color;
    getline(file, line); // Consume newline character

    vector<vector<Pixel>> pixels(height, vector<Pixel>(width));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            file >> pixels[i][j].r >> pixels[i][j].g >> pixels[i][j].b;
        }
    }

    file.close();
    return pixels;
}

void write_ppm(const string& filename, const vector<vector<Pixel>>& pixels, int width, int height, int max_color) {
    ofstream file(filename);
    file << "P3" << endl;
    file << width << " " << height << endl;
    file << max_color << endl;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            file << pixels[i][j].r << " " << pixels[i][j].g << " " << pixels[i][j].b << " ";
        }
        file << endl;
    }

    file.close();
}

int rgb_to_gray(int r, int g, int b) {
    return (r + g + b) / 3;
}

int main(int argc, char* argv[]) {
    auto start_total = high_resolution_clock::now();
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <path-to-original-image> <path-to-final-image>" << endl;
        return 1;
    }

    string input_filename = argv[1];
    string final_output_filename = argv[2];

    int width, height, max_color;
    vector<vector<Pixel>> original_pixels = read_ppm(input_filename, width, height, max_color);
    vector<vector<Pixel>> intermediate_pixels(height, vector<Pixel>(width));
    vector<vector<Pixel>> grayscale_pixels(height, vector<Pixel>(width));

    // Create shared memory for updated pixels
    const char* shm_name = "/updated_pixels";
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(Pixel) * width * height);
    Pixel* updated_pixels = (Pixel*) mmap(NULL, sizeof(Pixel) * width * height, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    //Create a shared Memory object for semaphore
    const char* sharedMemoryName = "/my_shared_semaphore";
    int sharedMemoryFd = shm_open(sharedMemoryName, O_CREAT | O_RDWR, 0666);
    if (sharedMemoryFd == -1) {
        perror("shm_open");
        return 1;
    }
    // Set the size of the shared memory object
    ftruncate(sharedMemoryFd, sizeof(Sema));

    // Map the shared memory object into the process's address space
    Sema* sharedSemaphore = static_cast<Sema*>(mmap(NULL, sizeof(Sema), PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryFd, 0));
    if (sharedSemaphore == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    if (sem_init(&(sharedSemaphore->semaphore), 1, 0) == -1) {
        perror("sem_init");
        return 1;
    }


    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) { 
        // Child process
            // sem_t semaphore2;
        //If the shared memory for semaphore is empty
            // sem_init(&semaphore2,0,1);
        //else
            //Read the semaphore from shared memory and set it.

        int height = original_pixels.size();
        int width = original_pixels[0].size();

        for(int row = 0; row < height; row++) {
            // sem_wait(&semaphore2);
            for (int col = 0; col < width; col++) {
                int gray_value = rgb_to_gray(original_pixels[row][col].r, original_pixels[row][col].g, original_pixels[row][col].b);
                grayscale_pixels[row][col].r = grayscale_pixels[row][col].g = grayscale_pixels[row][col].b = gray_value;
            }
            // sem_post(semaphore1); // Wait for parent to read the updated row before proceeding
            // write the semaphore back to the shared memory
            // cout<<"Row: "<<row<<endl;
            for (int col = 0; col < width; col++) {
                updated_pixels[row * width + col] = grayscale_pixels[row][col];
            }
            sharedSemaphore->signal();
        }
        // sem_post(semaphore1); // Signal that the row is processed


    }
    else { // Parent process
        // Wait for child to complete processing
        
        // sem_t semaphore2;
        // sem_init(&semaphore2,0,1);
        // Read rows from shared memory and process
        // wait(&shared_semaphore);

        int height = original_pixels.size();
        int width = original_pixels[0].size();

        for(int row = 0; row < height; row++) {
            // Wait for child to process the row
            sharedSemaphore->wait();

            for (int col = 0; col < width; col++) {
                intermediate_pixels[row][col] = updated_pixels[row * width + col];
            }
            // sem_post(&semaphore2); // Signal child to proceed
        }

        // Process the intermediate pixels
        for(int row = 0; row < height; row++) {
            // sem_wait(&semaphore2);
            // sem_wait(&semaphore1);// Wait for child to process the row 
            // cout<<"Accessing Row: "<<row;
            for (int col = 0; col < width; col++) {
                int neighbor_count = 0;
                int red_blur_value = 0, blue_blur_value = 0, green_blur_value = 0;

                // Calculate average color values of next 10 pixels to the right
                for (int i = col + 1; i <= col + 10 && i < width; i++) {
                    red_blur_value += intermediate_pixels[row][i].r;
                    blue_blur_value += intermediate_pixels[row][i].b;
                    green_blur_value += intermediate_pixels[row][i].g;
                    neighbor_count++;
                }

                // Calculate average color values of next 10 pixels to the left
                for (int i = col - 1; i >= col - 10 && i >= 0; i--) {
                    red_blur_value += intermediate_pixels[row][i].r;
                    blue_blur_value += intermediate_pixels[row][i].b;
                    green_blur_value += intermediate_pixels[row][i].g;
                    neighbor_count++;
                }

                // Calculate average color values
                red_blur_value += intermediate_pixels[row][col].r;
                blue_blur_value += intermediate_pixels[row][col].b;
                green_blur_value += intermediate_pixels[row][col].g;
                neighbor_count++;

                red_blur_value /= neighbor_count;
                blue_blur_value /= neighbor_count;
                green_blur_value /= neighbor_count;

                // Update pixel color values
                original_pixels[row][col].r = red_blur_value;
                original_pixels[row][col].g = green_blur_value;
                original_pixels[row][col].b = blue_blur_value;
            }
            
        }
        // sem_post(&semaphore2);
        
        //write back the shared semaphore

        // Write the final output to file
        write_ppm(final_output_filename, original_pixels, width, height, max_color);
    }

    // Clean up shared memory
    munmap(updated_pixels, sizeof(Pixel) * width * height);
    close(shm_fd);
    shm_unlink(shm_name);

    // Close and unlink the semaphore
    // sem_close(semaphore1);
    // sem_unlink(sem_name);

    auto stop_total = high_resolution_clock::now();
    auto duration_total = duration_cast<milliseconds>(stop_total - start_total);
    cout << "Total Execution Time: " << duration_total.count() << " milliseconds" << endl;

    return 0;
}
