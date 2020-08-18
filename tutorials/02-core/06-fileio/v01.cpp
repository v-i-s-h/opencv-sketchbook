/*
 * openCV Serialization
 */

#include <iostream>

#include <opencv2/core.hpp>

using namespace std;
using namespace cv;


class MyData {
    public:
        int A;
        double X;
        string id;

        MyData(): A(0), X(0.0), id() {
        }

        explicit MyData(int): A(97), X(CV_PI), id("mydata123") {
        }

        void write(FileStorage &fs) const {
            fs << "{"
               << "A" << A
               << "X" << X
               << "id" << id
               << "}";
        }

        void read(const FileNode &node) {
            A   = (int) node["A"];
            X   = (double) node["X"];
            id  = (string) node["id"];
        }
};


static void write(FileStorage &fs, const std::string&, const MyData &x) {
    x.write(fs);
}


static void read(const FileNode &node, MyData &x, const MyData &default_value = MyData()) {
    if(node.empty()) {
        x = default_value;
    } else {
        x.read(node);
    }
}


static ostream& operator<<(ostream &out, const MyData &m) {
    out << "{ "
        << "id = " << m.id << ", "
        << "A = " << m.A << ", "
        << "X = " << m.X 
        << "}";
    
    return out;
}


int main(int argc, char *argv[]) {
    
    if(argc != 2) {
        cout << "Not enough arguments" << endl;
        return EXIT_FAILURE;
    }

    string filename = argv[1];

    // Write
    {
        Mat R = Mat_<uchar>::eye(3, 3);
        Mat T = Mat_<double>::zeros(3, 1);

        MyData m(1);

        FileStorage fs(filename, FileStorage::WRITE);
        
        // write some data
        fs << "iterationNr" << 100;
        fs << "strings" << "[";
        fs << "image1.jpg" << "Awesomeness" << "../data/baboon.jpg";
        fs << "]";

        fs << "Mapping"
           << "{" 
           << "One" << 1
           << "Two" << 2
           << "}";

        fs << "R" << R
           << "T" << T;

        fs << "MyData" << m;

        // close the file
        fs.release();
        cout << "Write complete.\n";
    }

    // Read
    {
        cout << "\nReading..." << endl;

        FileStorage fs;
        fs.open(filename, FileStorage::READ);
        if(!fs.isOpened()) {
            cerr << "Failed to open " << filename << endl;
            return EXIT_FAILURE;
        }

        int itNr;
        fs["iterationNr"] >> itNr;
        cout << "itNr   = " << itNr << endl;

        cout << "\nstrings\n";
        FileNode n = fs["strings"];
        if(n.type() != FileNode::SEQ) {
            cerr << "strings are not in sequence!" << endl;
            return EXIT_FAILURE;
        }
        for(FileNodeIterator it = n.begin(); it != n.end(); ++it) {
            cout << "    " << (string) *it << endl;
        }

        cout << "\nMapping\n";
        n = fs["Mapping"];
        cout << "Two    " << (int) n["Two"] << "; "
             << "One    " << (int) n["One"] << endl << endl;

        cout << "\nMyData\n";
        MyData m;
        fs["MyData"] >> m;
        cout << "m = " << m << endl;

        cout <<"\nMat\n";
        Mat R, T;
        fs["R"] >> R;
        fs["T"] >> T;
        cout << "R = " << R << "\n"
             << "T = " << T << endl;

        // non-existing
        fs["NonExist"] >> m;
        cout << "\nNon existing\n";
        cout << m << endl;
    }

    return 0;
}
