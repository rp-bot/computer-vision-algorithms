#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    std::string imagePath = "001.png";

    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);

    if (image.empty())
    {
        std::cerr << "Could not open or find the image: " << imagePath << std::endl;
        return -1;
    }

    // Convert RGB to HSV
    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

    // Split the HSV image into Hue, Saturation, and Value components
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);

    // Scale Hue channel to [1,255] (mapping from 0-180 in OpenCV to 1-255)
    channels[0].convertTo(channels[0], CV_8U, 255.0 / 180.0);
    channels[0] += 1; // Offset by 1 so that 0 becomes 1.

    // Save the images
    cv::imwrite("intensity.png", channels[2]);
    cv::imwrite("saturation.png", channels[1]);
    cv::imwrite("hue.png", channels[0]);

    std::cout << "Images saved successfully!" << std::endl;

    return 0;
}
