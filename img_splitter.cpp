#include <iostream>
#include <vector>
#include <cstdio>
#include <cmath>
#include <tuple>
#include <stdio.h>

// Adjust the return type and intermediate variables to int
std::tuple<int, int, int> RGBtoHSV(unsigned char r, unsigned char g, unsigned char b)
{
    float R = r / 255.0f;
    float G = g / 255.0f;
    float B = b / 255.0f;

    float M = std::max(std::max(R, G), B);
    float m = std::min(std::min(R, G), B);
    float C = M - m;

    float H = 0;
    if (C != 0)
    {
        if (M == R)
        {
            H = fmod((60.0f * ((G - B) / C) + 360.0f), 360.0f);
        }
        else if (M == G)
        {
            H = fmod((60.0f * ((B - R) / C) + 120.0f), 360.0f);
        }
        else if (M == B)
        {
            H = fmod((60.0f * ((R - G) / C) + 240.0f), 360.0f);
        }
    }

    float S = (M == 0) ? 0 : (C / M);
    float V = M;

    return {H, S, V};
}

int main()
{
    // Dimensions of the 2D array and channels
    const int WIDTH = 1000;  // Change to your desired width
    const int HEIGHT = 1000; // Change to your desired height
    const int CHANNELS = 3;  // Assuming RGB channels

    // Create a 2D vector to hold the data
    std::vector<std::vector<unsigned char>> data(HEIGHT, std::vector<unsigned char>(WIDTH * CHANNELS));
    std::vector<std::vector<unsigned char>> H_data(HEIGHT, std::vector<unsigned char>(WIDTH));
    std::vector<std::vector<unsigned char>> S_data(HEIGHT, std::vector<unsigned char>(WIDTH));
    std::vector<std::vector<unsigned char>> V_data(HEIGHT, std::vector<unsigned char>(WIDTH));

    // Open the RAW file for reading
    FILE *inputFile = fopen("HI_SAT.raw", "rb");
    if (!inputFile)
    {
        std::cerr << "Error opening input file." << std::endl;
        return 1;
    }

    // Read data from the RAW file into the 2D vector
    for (int i = 0; i < HEIGHT; i++)
    {
        fread(data[i].data(), sizeof(unsigned char), WIDTH * CHANNELS, inputFile);
    }
    fclose(inputFile);

    // Do any processing on the data if needed (this step is optional)
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0, k = 0; j < WIDTH * CHANNELS; j += 3, k++)
        {
            float H, S, V;
            std::tie(H, S, V) = RGBtoHSV(data[i][j], data[i][j + 1], data[i][j + 2]);
            H_data[i][k] = static_cast<unsigned char>(((H / 360.0f) * 254) + 1); // Rescaling H to fit in 8-bit char
            S_data[i][k] = static_cast<unsigned char>(S * 255);
            V_data[i][k] = static_cast<unsigned char>(V * 255);
        }
    }
    FILE *H_outputFile = fopen("H_output.raw", "wb");
    for (int i = 0; i < HEIGHT; i++)
    {
        fwrite(H_data[i].data(), sizeof(unsigned char), WIDTH, H_outputFile);
    }
    fclose(H_outputFile);

    // Write S_data to S_output.raw
    FILE *S_outputFile = fopen("S_output.raw", "wb");
    for (int i = 0; i < HEIGHT; i++)
    {
        fwrite(S_data[i].data(), sizeof(unsigned char), WIDTH, S_outputFile);
    }
    fclose(S_outputFile);

    // Write V_data to V_output.raw
    FILE *V_outputFile = fopen("V_output.raw", "wb");
    for (int i = 0; i < HEIGHT; i++)
    {
        fwrite(V_data[i].data(), sizeof(unsigned char), WIDTH, V_outputFile);
    }
    fclose(V_outputFile);

    std::cout << "Done!" << std::endl;
    return 0;
}
