#include <iostream>
#include <vector>
#include <cstdio>
#include <cmath>
#include <tuple>
#include <stdio.h>

std::tuple<float, float, float> RGBtoHSV(unsigned char r, unsigned char g, unsigned char b)
{
    float R = r / 255.0f;
    float G = g / 255.0f;
    float B = b / 255.0f;

    float maximum = std::max(std::max(R, G), B);
    float minimum = std::min(std::min(R, G), B);
    float difference = maximum - minimum;

    float hue = 0;
    if (difference != 0)
    {
        if (maximum == R)
        {
            hue = fmod((60.0f * ((G - B) / difference) + 360.0f), 360.0f);
        }
        else if (maximum == G)
        {
            hue = fmod((60.0f * ((B - R) / difference) + 120.0f), 360.0f);
        }
        else if (maximum == B)
        {
            hue = fmod((60.0f * ((R - G) / difference) + 240.0f), 360.0f);
        }
    }

    float saturation = (maximum == 0) ? 0 : (difference / maximum);
    float intensity = maximum;

    return {hue, saturation, intensity};
}

int main()
{
    // Dimensions of the 2D array and channels
    const int WIDTH = 1000;
    const int HEIGHT = 1000;
    const int CHANNELS = 3;

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

    // Convert the channel from (R,G,B) to (H,S,V) and then split the channels into its own images.
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0, k = 0; j < WIDTH * CHANNELS; j += 3, k++)
        {
            float hue, saturation, intensity;
            std::tie(hue, saturation, intensity) = RGBtoHSV(data[i][j], data[i][j + 1], data[i][j + 2]);
            H_data[i][k] = static_cast<unsigned char>(((hue / 360.0f) * 254) + 1); // Rescaling hue to fit in 8-bit char
            S_data[i][k] = static_cast<unsigned char>(saturation * 255);
            V_data[i][k] = static_cast<unsigned char>(intensity * 255);
        }
    }


    // Write the Hue data to H_output.raw
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
