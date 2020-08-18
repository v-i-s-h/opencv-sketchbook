/* 
* To understand image mask operations
*/

#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

void Sharpen(const Mat& myImage, Mat& Result);

int main(int argc, char *argv[]) {

    const char* filename = (argc > 1)? argv[1]: "rubberwhale1.png";

    Mat src, dst0, dst1;

    src = imread(samples::findFile(filename, false, true), 
                (argc >= 3 && !strcmp("G", argv[2]))? IMREAD_GRAYSCALE: IMREAD_COLOR);
    if(src.empty()) {
        cerr << "ERROR: Cannot open image" << endl;
        return EXIT_FAILURE;
    }

    // do custom funtion based filtering
    double t1 = (double) getTickCount();
    Sharpen(src, dst0);
    t1 = ((double) getTickCount() - t1) / getTickFrequency();

    // filter with core function
    Mat kernel = (Mat_<char>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
    double t2 = (double) getTickCount();
    filter2D(src, dst1, src.depth(), kernel);
    t2 = ((double) getTickCount() - t2) / getTickFrequency();

    cout << "Custom     " << t1 << "ms\n"
         << "in-built   " << t2 << "ms" << endl;

    namedWindow("Input", WINDOW_AUTOSIZE);
    namedWindow("O1", WINDOW_AUTOSIZE);
    namedWindow("O2", WINDOW_AUTOSIZE);

    imshow("Input", src);
    imshow("O1", dst0);
    imshow("O2", dst1);
    
    waitKey();
    return EXIT_SUCCESS;
}

void Sharpen(const Mat& src, Mat& dst) {
    // accept only uchar images
    CV_Assert(src.depth() == CV_8U);

    const int channels = src.channels();
    dst.create(src.size(), src.type());

    for(int j = 1; j < src.rows - 1; j++) {
        const uchar* prev_row = src.ptr<uchar>(j - 1);
        const uchar* curr_row = src.ptr<uchar>(j);
        const uchar* next_row = src.ptr<uchar>(j + 1);

        uchar* curr_pixel = dst.ptr<uchar>(j);

        for(int i = channels; i < channels * (src.cols - 1); ++i) {
            *curr_pixel++ = saturate_cast<uchar>(5 * curr_row[i]
                                -(curr_row[i - channels] + curr_row[i + channels]
                                  + prev_row[i] + next_row[i]));
        }
    }

    dst.row(0).setTo(Scalar(0));
    dst.row(dst.rows - 1).setTo(Scalar(0));
    dst.col(0).setTo(Scalar(0));
    dst.col(dst.cols - 1).setTo(Scalar(0));
}