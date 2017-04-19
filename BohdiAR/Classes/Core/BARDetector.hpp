#ifndef BECVDetector_hpp
#define BECVDetector_hpp

#import <iostream>
#import <sstream>
#import <string>
#import <ctime>
#import <cstdio>

#import <opencv2/core.hpp>
#import <opencv2/core/utility.hpp>
#import <opencv2/imgproc.hpp>
#import <opencv2/calib3d.hpp>
#import <opencv2/imgcodecs.hpp>
#import <opencv2/videoio.hpp>

#import "BARMarkers.hpp"

using namespace std;
using namespace cv;

class BARDetector {

public:
    Mat cameraMatrix;
    Mat distCoeffs;

    Mat R;
    Mat T;
    
    int markerId;
    float intrinsicMatColumnMajor[16];
    float extrinsicMatColumnMajor[16];
    
    enum Pattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };
    
    BARDetector(int width, int height, float unit, Pattern patternType, int flags = CV_ITERATIVE, bool RANSAC = false);
    bool drawChessboard;
    bool drawRect;
    bool drawAxis;
    
    bool calibrateCam(Mat& image, const char* calibrateFile);
    bool processImage(Mat& image);

private:
    BARMarkers* markerDetector;
    
    float unitSize;
    Pattern pattern;
    Size_<int> boardSize;
    
    vector<Point2f> points2D;
    vector<Point3f> points3D;
    
    int estimateFlags;
    bool useRANSAC;
    
    bool detect(Mat& image);
    double calibrate(Mat& image);
    bool estimate(int flags);
    void calculateExtrinsicMat(bool flip);
};

#endif
