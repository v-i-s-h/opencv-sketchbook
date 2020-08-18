/*
* Linear blending of images
*/

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {

    double alpha = 0.50;

    cout << "*Enter alpha   :";
    cin >> alpha;
    if(alpha < 0.0 || alpha > 1.0) {
        alpha = 0.50;
    }

    Mat img1, img2, imgout;

    img1 = imread(samples::findFile("LinuxLogo.jpg", false, true));
    img2 = imread(samples::findFile("WindowsLogo.jpg", false, true));

    if(img1.empty()) { cout << "Error loading img1" << endl; return EXIT_FAILURE; }
    if(img2.empty()) { cout << "Error loading img2" << endl; return EXIT_FAILURE; }

    addWeighted(img1, alpha, img2, 1.0 - alpha, 0.0, imgout);

    // cout << "Img1\n" << img1(Rect(0, 0, 10, 10)) << endl
    //      << "Img2\n" << img2(Rect(0, 0, 10, 10)) << endl
    //      << "Out\n"  << imgout(Rect(0, 0, 10, 10)) << endl;

    // Mat imgout2 = alpha * img1 + (1 - alpha) * img2;
    // cout << "Out\n"  << imgout2(Rect(0, 0, 10, 10)) << endl;
    
    imshow("Output", imgout);
    waitKey();

    return EXIT_SUCCESS;
}