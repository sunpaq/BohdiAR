//
//  BECVMarkers.hpp
//  BEDemo
//
//  Created by YuliSun on 17/03/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#ifndef BECVMarkers_hpp
#define BECVMarkers_hpp

#import "aruco.h"

using namespace std;
using namespace cv;
using namespace aruco;

class BARMarkers {

public:
    //ARUCO_MIP_36h12 is recommand
    BARMarkers(Mat cameraMatrix, Mat distCoeffs, float length, Dictionary::DICT_TYPES preDefine = Dictionary::ARUCO_MIP_36h12);
    
    int detect(Mat& image, bool drawMarker = true);
    int estimate(Mat& image, bool drawAxis = true);
    int getId(int index);
    void getPoseMat(int index, float* mat4);

private:
    int frameCount;
    float markerLength;
    
    Mat cameraMatrix;
    Mat distCoeffs;

    Dictionary dict;
    MarkerDetector detector;

    vector<Marker> markers;
    vector<MarkerPoseTracker> trackers;
    
    //OpenCV coordinate system z pointing into screen
    //OpenGL coordinate system z pointing out of screen
    void matrix4AddValue(float* mat, float* newmat, float rotateRatio, float transRatio);
    void glMatrixFromCV(float* glmat4, Mat R, Mat T, bool useStabilizer = false, float rotateStabilizer = 1.0, float translateStabilizer = 1.0, bool doesFlip = true);
};

#endif /* BECVMarkers_hpp */
