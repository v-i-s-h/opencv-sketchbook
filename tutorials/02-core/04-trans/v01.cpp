/*
 * Basic Linear Transform
 */

#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using std::cin;
using std::cout;
using std::endl;

Mat& transformC(Mat&, Mat&, double, double);
Mat& transformRA(Mat&, Mat&, double, double);

int main(int argc, char *argv[]) {
    
    CommandLineParser parser(argc, argv, 
        "{ input  | lena.jpg | input image  }"
        "{ alpha  | 1.50     | alpha value  }"
        "{ beta   | 50.0     | beta value   }"
    );
    
    Mat image = imread(samples::findFile(parser.get<String>("input"), false, true));
    if(image.empty()) {
        cout << "Could not find image!" << endl;
        return EXIT_FAILURE;
    }

    double alpha = parser.get<double>("alpha");
    double beta = parser.get<double>("beta");

    cout << "alpha = " << alpha << "    beta = " << beta << endl;

    double t1 = (double)getTickCount();
    Mat img1 = Mat::zeros(image.size(), image.type());
    transformC(image, img1, alpha, beta);
    t1 = 1000 * ((double)getTickCount() - t1) / getTickFrequency();


    double t2 = (double)getTickCount();
    Mat img2 = Mat::zeros(image.size(), image.type());
    transformRA(image, img2, alpha, beta);
    t2 = 1000 * ((double)getTickCount() - t2) / getTickFrequency();

    cout << "C []       : " << t1 << "ms\n"
         << "RA         : " << t2 << "ms" << endl;

    imshow("Original", image);
    imshow("v1", img1);
    imshow("v2", img2);
    imshow("diff", img1 - img2);
    waitKey();

    return EXIT_SUCCESS;
}


Mat& transformC(Mat& img, Mat& dst, double alpha, double beta) {
    int nChannels = img.channels();
    int nRows = img.rows;
    int nCols = img.cols * nChannels;

    uchar *p1 = 0, *p2 = 0;
    for(int i = 0; i < nRows; ++i) {
        p1 = img.ptr<uchar>(i);
        p2 = dst.ptr<uchar>(i);
        for(int j = 0; j < nCols; j++) {
            p2[j] = saturate_cast<uchar>(alpha * p1[j] + beta);
        }
    }

    return dst;
}


Mat& transformRA(Mat& img, Mat& dst, double alpha, double beta) {

    for(int i = 0; i < img.rows; ++i) {
        for(int j = 0; j < img.cols; ++j) {
            for(int c = 0; c < img.channels(); ++c) {
                dst.at<Vec3b>(i, j)[c] = saturate_cast<uchar>(
                        alpha * img.at<Vec3b>(i, j)[c] + beta
                );
            }
        }
    }
    
    return dst;
}

