#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sstream>
#include <pthread.h>
#include <atomic>
#include <chrono>

using namespace std;
using namespace std::chrono;
int h,w;
struct Pixel {
    int r, g, b;
};
int ij;
// Global buffer to store row numbers
vector<int> buffer;
pthread_mutex_t lock_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for buffer access

// Function to convert RGB to grayscale
int rgb_to_gray(int r, int g, int b) {
    return (r + g + b) / 3;
}

// Function to read ppm file
vector<vector<Pixel>> read_ppm(const string& filename, int& width, int& height, int& max_color) {
    ifstream file(filename);
    string line;
    getline(file, line); 
    file >> width >> height >> max_color; // read width, height, and max_color
    getline(file, line); // move to next line

    vector<vector<Pixel>> pixels(height, vector<Pixel>(width));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            file >> pixels[i][j].r >> pixels[i][j].g >> pixels[i][j].b;
        }
    }

    file.close();
    return pixels;
}

// Function to write ppm file
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

void printMatrix(const vector<vector<Pixel>>& original_pixels){
    for (int i = 0; i < original_pixels.size(); ++i) {
        for (int j = 0; j < original_pixels[0].size(); ++j) {
            cout << original_pixels[i][j].r << " " << original_pixels[i][j].g << " " << original_pixels[i][j].b;
        }
        cout << endl;
    }
}
void* convert_to_grayscale(void* arg) {
    cout << "Into convert_to_grayscale" << " ";   
    vector<vector<Pixel>>& original_pixels = *reinterpret_cast<vector<vector<Pixel>>*>(arg);
    int width = original_pixels[0].size();
    int height = original_pixels.size();

    while (true) {
        pthread_mutex_lock(&lock_mutex);
        // cout<<"ha/ningin";
        if (!buffer.empty()) {
            int row = buffer.back();
            buffer.pop_back();
            pthread_mutex_unlock(&lock_mutex);
            
            cout << "Accessing row = " << row << endl;
            for (int col = 0; col < width; ++col) {
                int gray_value = rgb_to_gray(original_pixels[row][col].r, original_pixels[row][col].g, original_pixels[row][col].b);
                original_pixels[row][col].r = original_pixels[row][col].g = original_pixels[row][col].b = gray_value;
                h--;
            }
        } else {
            pthread_mutex_unlock(&lock_mutex);
            cout<<height<<width<<endl;
            if (buffer.empty() && h * w == 0) { // Break the loop when all rows have been processed
                cout << "All rows processed. Exiting convert_to_grayscale loop." << endl;
                break;
            }
        }
    }
    cout << "Exit convert_to_grayscale" << endl;
    return nullptr;
}

// Function to apply horizontal blur effect
void* horizontalBlurImage(void* arg) {
    cout << "Into horizontalBlurImage" << " ";   
    vector<vector<Pixel>>& original_pixels = *reinterpret_cast<vector<vector<Pixel>>*>(arg);
    int height = original_pixels.size();
    int width = original_pixels[0].size();

    for(int row = 0; row < height; row++) {
        pthread_mutex_lock(&lock_mutex);
        for (int col = 0; col < width; col++) {
            int neighbor_count = 0;
            int red_blur_value = 0, blue_blur_value = 0, green_blur_value = 0;

            // Calculate average color values of next 10 pixels to the right
            for (int i = col + 1; i <= col + 10 && i < width; i++) {
                red_blur_value += original_pixels[row][i].r;
                blue_blur_value += original_pixels[row][i].b;
                green_blur_value += original_pixels[row][i].g;
                neighbor_count++;
            }

            // Calculate average color values of next 10 pixels to the left
            for (int i = col - 1; i >= col - 10 && i >= 0; i--) {
                red_blur_value += original_pixels[row][i].r;
                blue_blur_value += original_pixels[row][i].b;
                green_blur_value += original_pixels[row][i].g;
                neighbor_count++;
            }

            // Calculate average color values
            red_blur_value += original_pixels[row][col].r;
            blue_blur_value += original_pixels[row][col].b;
            green_blur_value += original_pixels[row][col].g;
            neighbor_count++;

            red_blur_value /= neighbor_count;
            blue_blur_value /= neighbor_count;
            green_blur_value /= neighbor_count;

            // Update pixel color values
            original_pixels[row][col].r = red_blur_value;
            original_pixels[row][col].g = green_blur_value;
            original_pixels[row][col].b = blue_blur_value;
        }        
        ij+=1;
        cout<<"in blur "<<ij<<endl;
        buffer.push_back(row);
        pthread_mutex_unlock(&lock_mutex);
    }
    cout << "Exit horizontalBlurImage" << endl;
    return nullptr;
}


int main(int argc, char* argv[]) {
    ij=0;
    auto start_total = high_resolution_clock::now();
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <path-to-original-image> <path-to-final-image>" << endl;
        return 1;
    }
    string input_filename = argv[1];
    string final_output_filename = argv[2];

    int width, height, max_color;
    vector<vector<Pixel>> original_pixels = read_ppm(input_filename, width, height, max_color);
    
    h=height;
    pthread_t t1, t2;
    pthread_create(&t1, nullptr, horizontalBlurImage, &original_pixels);
    pthread_create(&t2, nullptr, convert_to_grayscale, &original_pixels);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    auto end_total = high_resolution_clock::now();
    auto duration_total = duration_cast<microseconds>(end_total - start_total).count();

    // Write blurred image to file
    write_ppm(final_output_filename, original_pixels, width, height, max_color);

    cout << "Final image saved successfully." << endl;
    cout << "Total time taken for the entire code execution: " << duration_total << " microseconds" << endl;

    return 0;
}
