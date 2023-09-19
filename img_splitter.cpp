#include <iostream>
#include <vector>
#include <cstdio>

int main() {
    // Dimensions of the 2D array
    const int WIDTH = 3417;   // Change to your desired width
    const int HEIGHT = 3417;  // Change to your desired height

    // Create a 2D vector to hold the data
    std::vector<std::vector<unsigned char>> data(HEIGHT, std::vector<unsigned char>(WIDTH));

    // Open the RAW file for reading
    FILE* inputFile = fopen("0001.raw", "rb");
    if (!inputFile) {
        std::cerr << "Error opening input file." << std::endl;
        return 1;
    }

    // Read data from the RAW file into the 2D vector
    for (int i = 0; i < HEIGHT; i++) {
        fread(data[i].data(), sizeof(unsigned char), WIDTH, inputFile);
    }
    fclose(inputFile);

    // Do any processing on the data if needed (this step is optional)

    // Open another RAW file for writing
    FILE* outputFile = fopen("output.raw", "wb");
    if (!outputFile) {
        std::cerr << "Error opening output file." << std::endl;
        return 1;
    }

    // Write the 2D vector data to the output RAW file
    for (int i = 0; i < HEIGHT; i++) {
        fwrite(data[i].data(), sizeof(unsigned char), WIDTH, outputFile);
    }
    fclose(outputFile);

    std::cout << "Done!" << std::endl;
    return 0;
}
