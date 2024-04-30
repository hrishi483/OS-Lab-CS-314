#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Global variables
sem_t t1_semaphore;
sem_t t2_semaphore;


struct Pixel {
    int r, g, b;
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

// Wrapper function for pthread_create
void* rgbToGrayscaleThread(void* arg) {
    vector<vector<Pixel>>* pixels = (vector<vector<Pixel>>*)arg;
    int height = pixels->size();
    int width = (*pixels)[0].size();

    for (int y = 0; y < height; ++y) {
        // sem_wait(&t2_semaphore);
        for (int x = 0; x < width; ++x) {
            int gray = rgb_to_gray((*pixels)[y][x].r, (*pixels)[y][x].g, (*pixels)[y][x].b);
            (*pixels)[y][x].r = gray;
            (*pixels)[y][x].g = gray;
            (*pixels)[y][x].b = gray;
        }
        
        // Signal T2 that a row is ready
        sem_post(&t1_semaphore);
    }
    // sem_post(&t2_semaphore);

    pthread_exit(NULL);
}

// Function to apply blur transformation
void* applyBlur(void* arg) {
    vector<vector<Pixel>>* original_pixels = (vector<vector<Pixel>>*)arg;
    int height = original_pixels->size();
    int width = (*original_pixels)[0].size();

    for(int row = 0; row < height; row++) {
        // sem_wait(&t2_semaphore);
        sem_wait(&t1_semaphore);
        for (int col = 0; col < width; col++) {
            int neighbor_count = 0;
            int red_blur_value = 0, blue_blur_value = 0, green_blur_value = 0;

            // Calculate average color values of next 10 pixels to the right
            for (int i = col + 1; i <= col + 10 && i < width; i++) {
                red_blur_value += (*original_pixels)[row][i].r;
                blue_blur_value += (*original_pixels)[row][i].b;
                green_blur_value += (*original_pixels)[row][i].g;
                neighbor_count++;
            }

            // Calculate average color values of next 10 pixels to the left
            for (int i = col - 1; i >= col - 10 && i >= 0; i--) {
                red_blur_value += (*original_pixels)[row][i].r;
                blue_blur_value += (*original_pixels)[row][i].b;
                green_blur_value += (*original_pixels)[row][i].g;
                neighbor_count++;
            }

            // Calculate average color values
            red_blur_value += (*original_pixels)[row][col].r;
            blue_blur_value += (*original_pixels)[row][col].b;
            green_blur_value += (*original_pixels)[row][col].g;
            neighbor_count++;

            red_blur_value /= neighbor_count;
            blue_blur_value /= neighbor_count;
            green_blur_value /= neighbor_count;

            // Update pixel color values
            (*original_pixels)[row][col].r = red_blur_value;
            (*original_pixels)[row][col].g = green_blur_value;
            (*original_pixels)[row][col].b = blue_blur_value;
        }
        // sem_post(&t2_semaphore);
    }

    pthread_exit(NULL);
}

int main(int argc, char*argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <path-to-original-image> <path-to-final-image>" << endl;
        return 1;
    }

    string input_filename = argv[1];
    string final_output_filename = argv[2];

    int width, height, max_color;
    vector<vector<Pixel>> original_pixels = read_ppm(input_filename, width, height, max_color);

    // Initialize semaphores
    sem_init(&t1_semaphore, 0, 0);
    sem_init(&t2_semaphore, 0, 0);

    // Create threads for T1 and T2
    pthread_t t1_thread, t2_thread;
    pthread_create(&t1_thread, NULL, rgbToGrayscaleThread, &original_pixels);
    pthread_create(&t2_thread, NULL, applyBlur, &original_pixels);

    // Wait for threads to finish
    pthread_join(t1_thread, NULL);
    pthread_join(t2_thread, NULL);

    // Destroy semaphores
    sem_destroy(&t1_semaphore);
    sem_destroy(&t2_semaphore);

    // Write the resulting image to a new PPM file
    write_ppm(final_output_filename, original_pixels, width, height, max_color);

    std::cout << "Transformations applied successfully. Output saved to " << final_output_filename << std::endl;

    return 0;
}
