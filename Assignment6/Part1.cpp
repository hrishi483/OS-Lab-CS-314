#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

int ijk = 0;

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

void convert_to_grayscale(const vector<vector<Pixel>>& input_pixels, vector<vector<Pixel>>& output_pixels) {
    int height = input_pixels.size();
    int width = input_pixels[0].size();

    for(int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int gray_value = rgb_to_gray(input_pixels[row][col].r, input_pixels[row][col].g, input_pixels[row][col].b);
            output_pixels[row][col].r = output_pixels[row][col].g = output_pixels[row][col].b = gray_value;
        }
    }
}

void horizontalBlurImage(const vector<vector<Pixel>>& input_pixels, vector<vector<Pixel>>& output_pixels) {
    int height = input_pixels.size();
    int width = input_pixels[0].size();

    for(int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int neighbor_count = 0;
            int red_blur_value = 0, blue_blur_value = 0, green_blur_value = 0;

            // Calculate average color values of next 10 pixels to the right
            for (int i = col + 1; i <= col + 10 && i < width; i++) {
                red_blur_value += input_pixels[row][i].r;
                blue_blur_value += input_pixels[row][i].b;
                green_blur_value += input_pixels[row][i].g;
                neighbor_count++;
            }

            // Calculate average color values of next 10 pixels to the left
            for (int i = col - 1; i >= col - 10 && i >= 0; i--) {
                red_blur_value += input_pixels[row][i].r;
                blue_blur_value += input_pixels[row][i].b;
                green_blur_value += input_pixels[row][i].g;
                neighbor_count++;
            }

            // Calculate average color values
            red_blur_value += input_pixels[row][col].r;
            blue_blur_value += input_pixels[row][col].b;
            green_blur_value += input_pixels[row][col].g;
            neighbor_count++;

            red_blur_value /= neighbor_count;
            blue_blur_value /= neighbor_count;
            green_blur_value /= neighbor_count;

            // Update pixel color values
            output_pixels[row][col].r = red_blur_value;
            output_pixels[row][col].g = green_blur_value;
            output_pixels[row][col].b = blue_blur_value;
        }
    }
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

    horizontalBlurImage(original_pixels, intermediate_pixels);
    cout<<"Completed horizontalBlurImage "<<endl;

    convert_to_grayscale(intermediate_pixels, original_pixels);

    write_ppm(final_output_filename, original_pixels, width, height, max_color);
    cout<<"Completed Grayscale "<<endl;

    auto stop_total = high_resolution_clock::now();
    auto duration_total = duration_cast<milliseconds>(stop_total - start_total);
        
    cout << "Total Execution Time: " << duration_total.count() << " milliseconds" << endl;

    return 0;
    
}