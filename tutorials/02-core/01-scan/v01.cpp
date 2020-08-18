// tutorial on going through the image - pixel by pixel

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <sstream>

using namespace std;
using namespace cv;

// Test functions
Mat& ScanAndReduceC(Mat&, const uchar*);
Mat& ScanAndReduceIterator(Mat&, const uchar*);
Mat& ScanAndReduceRandomAccess(Mat&, const uchar*);

int main(int argc, char *argv[]) {

    if(argc < 3) {
        cout << "Not enough parameters" << endl;
        return -1;
    }

    // get image path from filename
    string img_path = samples::findFile(argv[1], false, true);
    cout << "Image file: " << img_path << endl;

    Mat I, J;
    I = imread(img_path, (argc == 4 && !strcmp(argv[3], "G"))? IMREAD_GRAYSCALE: IMREAD_COLOR);

    // Did we load the image?
    if(I.empty()) {
        cout << "ERROR: Could not load image." << endl;
        return 1;
    }
    
    // image is loaded
    // imshow("Image", I);
    // int k = waitKey(0); // wait untill a key press

    // create look up table
    int divideWith = 0;
    stringstream ss; ss << argv[2]; ss >> divideWith;   // convert to int, c++ style
    if(!ss || !divideWith) {
        cout << "ERROR: Invalid number to divideWith" << endl;
        return 2;
    }

    uchar table[256];
    for(int i =0; i < 256; i++) {
        table[i] = (uchar)(divideWith * (i/divideWith));
    }

    // tools for measuring
    const int rounds = 100;
    double t = 0;

    // --------------- Test 1: The Efficient Way - C [] Operator! --------------
    t = (double) getTickCount();
    for(int i = 0; i < rounds; i++) {
        Mat clone_I = I.clone();
        J = ScanAndReduceC(clone_I, table);
    }
    t = 1000 * ((double)getTickCount() - t) / getTickFrequency();
    cout << "C [] operator      : " << t / rounds << "ms" << endl;
    
    // --------------- Test 2: The Safe Way - Iterator -------------------------
    t = (double)getTickCount();
    for(int i = 0; i < rounds; i++) {
        Mat clone_I = I.clone();
        J = ScanAndReduceIterator(clone_I, table);
    }
    t = 1000 * ((double)getTickCount() - t) / getTickFrequency();
    cout << "Iterator           : " << t / rounds << "ms" << endl;
    
    // --------------- Test 3: On the fly --------------------------------------
    t = (double)getTickCount();
    for(int i = 0; i < rounds; i++) {
        Mat clone_I = I.clone();
        J = ScanAndReduceRandomAccess(clone_I, table);
    }
    t = 1000 * ((double)getTickCount() - t) / getTickFrequency();
    cout << "On the fly         : " << t / rounds << "ms" << endl;
    // --------------- Test 4: LookUp Table ------------------------------------
    // make lookup table
    Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    for(int i  = 0; i < 256; ++i) {
        p[i] = table[i];
    }

    t = (double)getTickCount();
    for(int i = 0; i < rounds; i++) {
        LUT(I, lookUpTable, J);
    }
    t = 1000 *((double)getTickCount() - t) / getTickFrequency();
    cout << "LUT                : " << t / rounds << "ms" << endl;
    // -------------------------------------------------------------------------

    imshow("Original", I);
    imshow("Modified", J);

    waitKey(0);

    return 0;
}

// The Efficient Way - C [] Operator
Mat& ScanAndReduceC(Mat &I, const uchar* const table) {
    // accept only char type matrices
    CV_Assert(I.depth() == CV_8U);

    int channels = I.channels();

    int nRows = I.rows;
    int nCols = I.cols * channels;

    if(I.isContinuous()) {
        nCols *= nRows;
        nRows = 1;
    }

    int i, j;
    uchar *p;
    for(i = 0; i < nRows; i++) {
        p = I.ptr<uchar>(i);
        for(j = 0; j < nCols; j++) {
            p[j] = table[p[j]];
        }
    }

    return I;
}


// The Safe Way - Iterator
Mat& ScanAndReduceIterator(Mat &I, const uchar* const table) {
    // accept only char type matrices
    CV_Assert(I.depth() == CV_8U);

    const int channels = I.channels();
    switch(channels) {
        case 1: {
            MatIterator_<uchar> it, end;
            for(it = I.begin<uchar>(), end = I.end<uchar>(); it != end; ++it) {
                *it = table[*it];
            }
            break;
        }
        case 3: {
            MatIterator_<Vec3b> it, end;
            for(it = I.begin<Vec3b>(), end = I.end<Vec3b>(); it != end; ++it) {
                (*it)[0] = table[(*it)[0]];
                (*it)[1] = table[(*it)[1]];
                (*it)[2] = table[(*it)[2]];
            }
            break;
        }
    }

    return I;
}


// Random Access
Mat& ScanAndReduceRandomAccess(Mat &I, const uchar* const table) {
    // accept only char type matrices
    CV_Assert(I.depth() == CV_8U);

    const int channels = I.channels();
    switch(channels) {
        case 1: {
            for(int i = 0; i < I.rows; ++i) {
                for(int j = 0; j < I.cols; ++j) {
                    I.at<uchar>(i, j) = table[I.at<uchar>(i, j)];
                }
            }
            break;
        }
        case 3: {
            Mat_<Vec3b> _I = I;
            for(int i = 0; i < I.rows; ++i) {
                for(int j = 0; j < I.rows; ++j) {
                    _I(i, j)[0] = table[_I(i, j)[0]];
                    _I(i, j)[1] = table[_I(i, j)[1]];
                    _I(i, j)[2] = table[_I(i, j)[2]];
                }
            }
            I = _I;
            break;
        }
    }

    return I;
}