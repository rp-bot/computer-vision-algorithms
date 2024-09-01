#include <iostream>
#include <fstream>

// Constants representing the dimensions of the image.
const int WIDTH = 750;       // Width of the image.
const int HEIGHT = 500;      // Height of the image.
bool rgb = true;             // Flag to determine if the image is in RGB or grayscale format.
typedef unsigned char uchar; // Define a type for bytes.

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
void readRaw(const std::string &filename, uchar img[HEIGHT][WIDTH])
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
void convolve3x3(uchar img[HEIGHT][WIDTH], int filter[3][3], uchar result[HEIGHT][WIDTH])
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
void convolve5x5(uchar img[HEIGHT][WIDTH], int filter[5][5], uchar result[HEIGHT][WIDTH])
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

// Function to save the 2D array as a raw grayscale image file.
void writeRaw(const std::string &filename, const uchar img[HEIGHT][WIDTH])
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
    uchar img[HEIGHT][WIDTH];
    uchar result_3x3_X[HEIGHT][WIDTH];
    uchar result_3x3_Y[HEIGHT][WIDTH];
    uchar result_5x5_X[HEIGHT][WIDTH];
    uchar result_5x5_Y[HEIGHT][WIDTH];

    // Read the raw image.
    rgb = false;
    readRaw("unesco-1.raw", img);

    // Apply the Sobel and 5x5 edge detection filters.
    convolve3x3(img, Sx, result_3x3_X);
    convolve3x3(img, Sy, result_3x3_Y);
    convolve5x5(img, horizontal5x5, result_5x5_X);
    convolve5x5(img, vertical5x5, result_5x5_Y);

    // Save the processed images.
    writeRaw("unesco-1_out.raw", img);
    writeRaw("out_india_flag_3x3_Sy.raw", result_3x3_Y);
    writeRaw("out_india_flag_5x5_x.raw", result_5x5_X);
    writeRaw("out_india_flag_5x5_y.raw", result_5x5_Y);

    return 0;
}
