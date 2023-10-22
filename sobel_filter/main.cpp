#include <iostream>
#include <fstream>

// Constants for image dimensions.
const int WIDTH = 750;  // Change this based on your image width
const int HEIGHT = 500; // Change this based on your image height
bool rgb = true;
typedef unsigned char uchar;

int Sx[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}};

int Sy[3][3] = {
    {-1, -2, -1},
    {0, 0, 0},
    {1, 2, 1}};

// Horizontal 5x5 difference filter
int vertical5x5[5][5] = {
    {-1, -2, 0, 2, 1},
    {-2, -4, 0, 4, 2},
    {-4, -8, 0, 8, 4},
    {-2, -4, 0, 4, 2},
    {-1, -2, 0, 2, 1}};

// Vertical 5x5 difference filter
int horizontal5x5[5][5] = {
    {-1, -2, -4, -2, -1},
    {-2, -4, -8, -4, -2},
    {0, 0, 0, 0, 0},
    {2, 4, 8, 4, 2},
    {1, 2, 4, 2, 1}};

// Function to read a raw grayscale image into a 2D array.
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

                img[i][j] = (r + g + b) / 3;
            }
        }
    }
    else
    {
        for (int i = 0; i < HEIGHT; ++i)
        {
            for (int j = 0; j < WIDTH; ++j)
            {
                img[i][j] = file.get();
            }
        }
    }

    file.close();
}
void convolve3x3(uchar img[HEIGHT][WIDTH], int filter[3][3], uchar result[HEIGHT][WIDTH])
{
    for (int y = 1; y < HEIGHT - 1; ++y)
    {
        for (int x = 1; x < WIDTH - 1; ++x)
        {
            int sum = 0;
            for (int j = -1; j <= 1; ++j)
            {
                for (int i = -1; i <= 1; ++i)
                {
                    sum += img[y + j][x + i] * filter[j + 1][i + 1];
                }
            }

            // Adjust base value to 128
            sum += 128;

            // Clip values
            sum = sum < 0 ? 0 : sum;
            sum = sum > 255 ? 255 : sum;

            result[y][x] = sum;
        }
    }
}

void convolve5x5(uchar img[HEIGHT][WIDTH], int filter[5][5], uchar result[HEIGHT][WIDTH])
{
    for (int y = 2; y < HEIGHT - 2; ++y)
    {
        for (int x = 2; x < WIDTH - 2; ++x)
        {
            int sum = 0;
            for (int j = -2; j <= 2; ++j)
            {
                for (int i = -2; i <= 2; ++i)
                {
                    sum += img[y + j][x + i] * filter[j + 2][i + 2];
                }
            }

            // Adjust base value to 128
            sum += 128;

            // Clip values
            sum = sum < 0 ? 0 : sum;
            sum = sum > 255 ? 255 : sum;

            result[y][x] = sum;
        }
    }
}

// Function to write a 2D array to a raw grayscale image file.
void writeRaw(const std::string &filename, const uchar img[HEIGHT][WIDTH])
{
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        exit(1);
    }

    for (int i = 0; i < HEIGHT; ++i)
    {
        for (int j = 0; j < WIDTH; ++j)
        {
            file.put(img[i][j]); // Write one byte to the file.
        }
    }

    file.close();
}

int main()
{
    // Define a 2D array to hold the grayscale image.
    uchar img[HEIGHT][WIDTH];
    uchar result_3x3_X[HEIGHT][WIDTH];
    uchar result_3x3_Y[HEIGHT][WIDTH];
    uchar result_5x5_X[HEIGHT][WIDTH];
    uchar result_5x5_Y[HEIGHT][WIDTH];
    // Read the raw grayscale image.
    // readRaw("india_flag.raw", img);
    // rgb = false;
    readRaw("unesco-1.raw", img);

    // You can now process the img as required...
    convolve3x3(img, Sx, result_3x3_X);
    convolve3x3(img, Sy, result_3x3_Y);
    convolve5x5(img, horizontal5x5, result_5x5_X);
    convolve5x5(img, vertical5x5, result_5x5_Y);
    // Write the processed image back to a raw file.
    writeRaw("out_unesco-1.raw", img);
    writeRaw("out_unesco-13x3_Sx.raw", result_3x3_X);
    writeRaw("out_unesco-13x3_Sy.raw", result_3x3_Y);
    writeRaw("out_unesco-15x5_x.raw", result_5x5_X);
    writeRaw("out_unesco-15x5_y.raw", result_5x5_Y);
    // writeRaw("output_india_flag.raw", img);

    return 0;
}
