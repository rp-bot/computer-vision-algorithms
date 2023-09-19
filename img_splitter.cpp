#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

float computeIntensity(unsigned char r, unsigned char g, unsigned char b) {
    return 0.299f * r + 0.587f * g + 0.114f * b;
}



int main()
{
    const std::string filePath = "002.raw";

    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Failed to open the file!" << std::endl;
        return 1;
    }

    // Assuming you know the dimensions of the 2D array
    const int rows = 3417; // example value
    const int cols = 3417; // example value

    // Create a 2D vector
    std::vector<std::vector<char> > data(rows, std::vector<char>(cols));

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            char value;
            file.read(&value, sizeof(char));
            data[i][j] = value;
        }
    }

    file.close();

    // Now, you can access the data using data[i][j]
    std::cout << data[3416][3416];
    

    return 0;
}
