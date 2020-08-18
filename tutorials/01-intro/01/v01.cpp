/*
 * Read, display and optionally write an image
 */

#include <opencv2/core.hpp>
// #include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

using namespace cv;

int main(int argc, char *argv[]) {
    std::string img_path = samples::findFile("starry_night.jpg", false, true);
    std::cout << "Image file : " << img_path << std::endl;

    // Mat img = imread(img_path, IMREAD_COLOR);
    Mat img = imread(img_path, IMREAD_GRAYSCALE);

    if(img.empty()) {
        std::cout << "ERROR: Could not load image" << std::endl;
        return 1;
    }

    imshow("Image", img);

    int k = waitKey(0);

    return 0;
}
