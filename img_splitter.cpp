#include <iostream>
#include <vector>
#include <fstream>

int main()
{
    // Dimensions of the 2D array
    const int WIDTH = 3417;  // Change to your desired width
    const int HEIGHT = 3417; // Change to your desired height

    // Create a 2D vector to hold the data
    std::vector<std::vector<unsigned char>> data(HEIGHT, std::vector<unsigned char>(WIDTH));

    // Open the RAW file for reading
    std::ifstream inputFile("0001.raw", std::ios::binary);
    if (!inputFile)
    {
        std::cerr << "Error opening input file." << std::endl;
        return 1;
    }

    // Read data from the RAW file into the 2D vector
    for (int i = 0; i < HEIGHT; i++)
    {
        inputFile.read((char *)data[i].data(), WIDTH);
    }
    inputFile.close();

    // Do any processing on the data if needed (this step is optional)

    // Open another RAW file for writing
    std::ofstream outputFile("output.raw", std::ios::binary);
    if (!outputFile)
    {
        std::cerr << "Error opening output file." << std::endl;
        return 1;
    }

    // Write the 2D vector data to the output RAW file
    for (int i = 0; i < HEIGHT; i++)
    {
        outputFile.write((char *)data[i].data(), WIDTH);
    }
    outputFile.close();

    std::cout << "Done!" << std::endl;
    return 0;
}
