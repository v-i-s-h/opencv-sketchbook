/*
 * Discrete Fourier Transform of images
 */

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

// utility function to rearrange
void rearrange(Mat& src, Mat& dst);

int main(int argc, char *argv[]) {

    CommandLineParser parser(argc, argv,
        "{@image | lena.jpg | input image }"
        "{scale | 1.0      | scale factor }"
    );
    
    string filename = parser.get<String>("@image");
    Mat img = imread(samples::findFile(filename, false, true), IMREAD_GRAYSCALE);
    if(img.empty()) {
        cout << "ERROR: Cannot read image " << filename << endl;
        return EXIT_FAILURE;
    }
    // print image info
    cout << "Input      : " << filename << "\n"
         << "Size       : " << img.rows << " x " 
                            << img.cols << " x "
                            << img.channels() << endl;
    
    // resize image
    double factor = parser.get<double>("scale");
    resize(img, img, Size(), factor, factor);
    cout << "Resized to : " << img.rows << " x "
                            << img.cols << " x "
                            << img.channels() << endl;

    // expand input image to optimal size
    Mat padded;
    int m = getOptimalDFTSize(img.rows);
    int n = getOptimalDFTSize(img.cols);
    copyMakeBorder(img, padded, 0, m - img.rows, 0, n - img.cols, 
                   BORDER_CONSTANT, Scalar::all(0));
    cout << "DFT\n"
         << "   Optimal Size    : " << m << " x " << n << "\n"
         << "   Padded Image    : " << padded.rows << " x " << padded.cols
                                    << " x " << padded.channels() << endl;

    // Make real and complex planes
    Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
    Mat complexI;
    merge(planes, 2, complexI);

    // Compute DFT
    dft(complexI, complexI);

    // compute magnitude
    split(complexI, planes);  // split into vector of channels
    Mat mag = Mat::zeros(planes[0].size(), CV_32F);
    Mat pha = Mat::zeros(planes[0].size(), CV_32F);
    magnitude(planes[0], planes[1], mag);
    phase(planes[0], planes[1], pha);

    // convert magnitude to log scale and normalize for visualization
    mag += Scalar::all(1);
    log(mag, mag);
    normalize(mag, mag, 0, 1, NORM_MINMAX);
    
    Mat mag_rearr;
    rearrange(mag, mag_rearr);

    // convert phase
    normalize(pha, pha, 0, 1, NORM_MINMAX);
    Mat pha_rearr;
    rearrange(pha, pha_rearr);

    imshow("input", img);
    // imshow("magnitude", mag);
    imshow("magnitude", mag_rearr);
    imshow("spectrum", pha_rearr);
    waitKey();

    return 0;
}


void rearrange(Mat& src, Mat& dst) {
    // crop and rearrange spectrum
    dst = src(Rect(0, 0, src.cols & -2, src.rows & -2)).clone(); // even rows and cols
    int cx = dst.cols / 2;
    int cy = dst.rows / 2;
    
    // rearrange such that origin is at image center
    Mat q0(dst, Rect(0, 0, cx, cy));      // top-left
    Mat q1(dst, Rect(cx, 0, cx, cy));     // top-right
    Mat q2(dst, Rect(0, cy, cx,  cy));    // bottom-left
    Mat q3(dst, Rect(cx, cy, cx, cy));    // bottom-right
    Mat tmp;
    q0.copyTo(tmp); q3.copyTo(q0); tmp.copyTo(q3);      // top-left to bottom-right
    q1.copyTo(tmp); q2.copyTo(q1); tmp.copyTo(q2);      // top-right to bottom-left
}
