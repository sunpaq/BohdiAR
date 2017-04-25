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

    double fovx;
    double fovy;
    double focalLength;
    double aspectRatio;
    double principalPointX;
    double principalPointY;
    
    Mat R;
    Mat T;
    
    int markerId;
    
    bool  useStabilizer;
    float rotateStabilizer;
    float translateStabilizer;
    
    float intrinsicMatColumnMajor[16];
    float extrinsicMatColumnMajor[16];
    
    enum Pattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };
    
    BARDetector(int width, int height, float unit, Pattern patternType, int flags = SOLVEPNP_ITERATIVE, bool RANSAC = false);
    bool drawChessboard;
    bool drawMarker;
    bool drawAxis;
    
    bool calibrateFileLoad(const char* calibrateFile);
    
    //ip6_sensor_width  = 4.8;
    //ip6_sensor_height = 3.6;
    bool calibrateCam(Mat& image, const char* calibrateFile, double sensorWidth = 4.8, double sensorHeight = 3.6);
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
    
    void matrix4AddValue(float* mat, float* newmat, float rotateRatio, float transRatio);
    void calculateExtrinsicMat(bool flip);
    
    unsigned frameCount;
};

#endif
