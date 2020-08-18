/* 
 * Basic webcam input -- support for Android IP Webcam apps also
 */

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <iomanip>

using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {
    
    Mat frame;
    VideoCapture cap;

    // for webcam
    // int deviceId = 0;
    // int apiId = cv::CAP_ANY;
    
    // // for IP Webcam
    string deviceId = "http://192.168.43.109:8080/video";
    int apiId = cv::CAP_ANY;

    cap.open(deviceId, apiId);

    if(!cap.isOpened()) {
        cerr << "ERROR: Unable to open camera#" << deviceId << endl;
        return EXIT_FAILURE;
    }

    int width = (int) cap.get(CAP_PROP_FRAME_WIDTH);
    int height = (int) cap.get(CAP_PROP_FRAME_HEIGHT);
    cout << "Frame size : " << width << "x" << height << endl;

    // set frame sizei
    bool success = false;
    success = cap.set(CAP_PROP_FRAME_WIDTH, 320);
    cout << "Setting width to 320    " << (success? "DONE": " ERR") << endl;
    success = cap.set(CAP_PROP_FRAME_HEIGHT, 240);
    cout << "Setting height to 240   " << (success? "DONE": " ERR") << endl;
    success = cap.set(CAP_PROP_BUFFERSIZE, 1);
    cout << "Setting buffer size 1   " << (success? "DONE": " ERR") << endl;

    namedWindow("Stream", WINDOW_AUTOSIZE);

    unsigned int count = 0;
    double fps = 0;
    double t = getTickCount();
    while(1) {
        t = getTickCount();
        cap >> frame;
        if(frame.empty()) {
            cerr << "ERROR: Unable to grab frame!" << endl;
            return EXIT_FAILURE;
        }
        imshow("Stream", frame);
        int k = waitKey(10);
        fps = 1.0 / (((double) getTickCount() - t) / getTickFrequency());
        cout << setfill('-') << setw(10) << ++count << "    FPS: "
             << fps << "    ";
        int fps_bar = int(fps + 0.50);
        int fps_space = 90 - fps_bar;
        for(int i = 0; i < fps_bar; ++i) { cout << "#"; }
        for(int i = 0; i < fps_space; ++i) { cout << " "; }
        cout << "\r" << flush;
        
        if(k == 'q' || k == 'Q')
            break;
    }
    cout << endl;

    return EXIT_SUCCESS;
}
