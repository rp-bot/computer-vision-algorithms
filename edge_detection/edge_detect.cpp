#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <vector>
// Constants representing the dimensions of the image.
const int WIDTH = 500;       // Width of the image.
const int HEIGHT = 500;      // Height of the image.
bool rgb = true;             // Flag to determine if the image is in RGB or grayscale format.
typedef unsigned char uchar; // Define a type for bytes.
typedef std::vector<std::vector<uchar>> uchar_2d_Array;

// Sobel operator in X direction.
int Sx[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}};

// Sobel operator in Y direction.
int Sy[3][3] = {
    {-1, -2, -1},
    {0, 0, 0},
    {1, 2, 1}};

// Horizontal edge detection filter of size 5x5.
int horizontal5x5[5][5] = {
    {-1, -2, 0, 2, 1},
    {-2, -4, 0, 4, 2},
    {-4, -8, 0, 8, 4},
    {-2, -4, 0, 4, 2},
    {-1, -2, 0, 2, 1}};

// Vertical edge detection filter of size 5x5.
int vertical5x5[5][5] = {
    {-1, -2, -4, -2, -1},
    {-2, -4, -8, -4, -2},
    {0, 0, 0, 0, 0},
    {2, 4, 8, 4, 2},
    {1, 2, 4, 2, 1}};

// Function to read a raw image file and store it in a 2D array.
// If the image is RGB, it will be converted to grayscale.
void readRaw(const std::string &filename, uchar_2d_Array &img)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }

    if (rgb)
    {
        for (int i = 0; i < HEIGHT; ++i)
        {
            for (int j = 0; j < WIDTH; ++j)
            {
                unsigned char r = file.get();
                unsigned char g = file.get();
                unsigned char b = file.get();

                // Convert RGB to grayscale.
                img[i][j] = (r + g + b) / 3;
            }
        }
    }
    else
    {
        for (int i = 0; i < HEIGHT; ++i)
            for (int j = 0; j < WIDTH; ++j)
                img[i][j] = file.get();
    }

    file.close();
}

// Function to apply a 3x3 filter to the image.
void convolve3x3(uchar_2d_Array &img, int filter[3][3], uchar_2d_Array &result)
{
    // Iterate over the image skipping border pixels.
    for (int y = 1; y < HEIGHT - 1; ++y)
    {
        for (int x = 1; x < WIDTH - 1; ++x)
        {
            int sum = 0;

            // Apply the filter.
            for (int j = -1; j <= 1; ++j)
                for (int i = -1; i <= 1; ++i)
                    sum += img[y + j][x + i] * filter[j + 1][i + 1];

            // Normalize the result and ensure it's within the byte range.
            int sum_normalized = 128 - (sum / 8);
            // Clip values to ensure they lie between 0 and 255.
            sum_normalized = sum_normalized < 0 ? 0 : sum_normalized;
            sum_normalized = sum_normalized > 255 ? 255 : sum_normalized;
            result[y][x] = sum_normalized;
        }
    }
}

// Function to apply a 5x5 filter to the image.
void convolve5x5(uchar_2d_Array &img, int filter[5][5], uchar_2d_Array &result)
{
    // Iterate over the image skipping the outermost pixels.
    for (int y = 2; y < HEIGHT - 2; ++y)
    {
        for (int x = 2; x < WIDTH - 2; ++x)
        {
            int sum = 0;

            // Apply the filter.
            for (int j = -2; j <= 2; ++j)
                for (int i = -2; i <= 2; ++i)
                    sum += img[y + j][x + i] * filter[j + 2][i + 2];

            // Normalize the result and ensure it's within the byte range.
            int sum_normalized = 128 - (sum / 50);
            // Clip values to ensure they lie between 0 and 255.
            sum_normalized = sum_normalized < 0 ? 0 : sum_normalized;
            sum_normalized = sum_normalized > 255 ? 255 : sum_normalized;
            result[y][x] = sum_normalized;
        }
    }
}

// Function to calculate and normalize the gradient magnitude
void calculateGradientMagnitude(uchar_2d_Array &result_X, uchar_2d_Array &result_Y, uchar_2d_Array &gradient)
{
    int maxVal = 0;
    int minVal = 255;

    // First pass to find the min and max gradient magnitudes
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            // Compute the gradient magnitude
            int gx = static_cast<int>(result_X[y][x]) - 128; // Re-center the value around 0
            int gy = static_cast<int>(result_Y[y][x]) - 128; // Re-center the value around 0
            int magnitude = static_cast<int>(sqrt(gx * gx + gy * gy));

            // Update min and max values
            if (magnitude > maxVal)
                maxVal = magnitude;
            if (magnitude < minVal)
                minVal = magnitude;
        }
    }

    // Second pass to normalize the gradient magnitudes
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            // Compute the gradient magnitude
            int gx = static_cast<int>(result_X[y][x]) - 128; // Re-center the value around 0
            int gy = static_cast<int>(result_Y[y][x]) - 128; // Re-center the value around 0
            int magnitude = static_cast<int>(sqrt(gx * gx + gy * gy));

            // Normalize and clip the magnitude to the range of 0 to 255
            int normalizedMagnitude = (maxVal != minVal) ? 255 * (magnitude - minVal) / (maxVal - minVal) : 0;
            normalizedMagnitude = std::max(0, std::min(255, normalizedMagnitude));

            // Set the normalized magnitude to the gradient array
            gradient[y][x] = static_cast<uchar>(normalizedMagnitude);
        }
    }
}

void edgeDetection(uchar_2d_Array &result_X, uchar_2d_Array &result_Y, uchar_2d_Array &gradient, uchar_2d_Array &edges, int threshold)
{
    for (int y = 1; y < HEIGHT - 1; ++y)
    {
        for (int x = 1; x < WIDTH - 1; ++x)
        {
            int magnitude = gradient[y][x];

            if (magnitude < threshold)
            {
                edges[y][x] = 0;
                continue;
            }

            // Simplified edge direction based on the larger of the gradients in x or y direction
            bool isEdge = false;
            if (abs(result_X[y][x]) > abs(result_Y[y][x]))
            {
                // Edge is closer to horizontal, compare left and right
                isEdge = (magnitude > gradient[y][x - 1] && magnitude > gradient[y][x + 1]);
            }
            else
            {
                // Edge is closer to vertical, compare top and bottom
                isEdge = (magnitude > gradient[y - 1][x] && magnitude > gradient[y + 1][x]);
            }

            edges[y][x] = isEdge ? 200 : 0; // Set edge pixel to 200, non-edge to 0
        }
    }
}

// Function to save the 2D array as a raw grayscale image file.
void writeRaw(const std::string &filename, uchar_2d_Array &img)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        exit(1);
    }

    // Write the grayscale data to the file.
    for (int i = 0; i < HEIGHT; ++i)
        for (int j = 0; j < WIDTH; ++j)
            file.put(img[i][j]);

    file.close();
}

int main()
{
    // Define arrays to hold the original and processed images.
    uchar_2d_Array img(HEIGHT, std::vector<uchar>(WIDTH));
    uchar_2d_Array result_3x3_X(HEIGHT, std::vector<uchar>(WIDTH));
    uchar_2d_Array result_3x3_Y(HEIGHT, std::vector<uchar>(WIDTH));
    uchar_2d_Array result_5x5_X(HEIGHT, std::vector<uchar>(WIDTH));
    uchar_2d_Array result_5x5_Y(HEIGHT, std::vector<uchar>(WIDTH));
    uchar_2d_Array gradient_5x5(HEIGHT, std::vector<uchar>(WIDTH));
    uchar_2d_Array gradient_3x3(HEIGHT, std::vector<uchar>(WIDTH));
    uchar_2d_Array edges(HEIGHT, std::vector<uchar>(WIDTH));
    uchar_2d_Array edges_5x5(HEIGHT, std::vector<uchar>(WIDTH));
    int threshold = 30;
    // Read the raw image.
    rgb = false;
    readRaw("unesco-1_square.raw", img);

    convolve3x3(img, Sx, result_3x3_X);
    convolve3x3(img, Sy, result_3x3_Y);

    convolve5x5(img, horizontal5x5, result_5x5_X);
    convolve5x5(img, vertical5x5, result_5x5_Y);

    calculateGradientMagnitude(result_3x3_X, result_3x3_Y, gradient_3x3);
    calculateGradientMagnitude(result_5x5_X, result_5x5_Y, gradient_5x5);

    edgeDetection(result_3x3_X, result_3x3_Y, gradient_3x3, edges, threshold);
    edgeDetection(result_5x5_X, result_5x5_Y, gradient_5x5, edges_5x5, threshold);

    writeRaw("unesco-1_grad3x3.raw", gradient_3x3);
    writeRaw("unesco-1_grad5x5.raw", gradient_5x5);

    writeRaw("unesco-1_edges3x3.raw", edges);
    writeRaw("unesco-1_edges5x5.raw", edges_5x5);

    return 0;
}
