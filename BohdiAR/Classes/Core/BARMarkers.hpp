//
//  BECVMarkers.hpp
//  BEDemo
//
//  Created by YuliSun on 17/03/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#ifndef BECVMarkers_hpp
#define BECVMarkers_hpp

#import <iostream>
#import <opencv2/calib3d.hpp>
#import "aruco.h"

using namespace std;
using namespace cv;
using namespace aruco;

class BARMarkers {

public:
    //ARUCO_MIP_36h12 is recommand
    BARMarkers(Mat cameraMatrix, Mat distCoeffs, float length, Dictionary::DICT_TYPES preDefine = Dictionary::ARUCO_MIP_36h12);
    
    int detect(Mat& image, bool drawMarker = true);
    int getId(int index);
    void estimate(Mat& image, int index, float* modelViewMat, bool drawAxis = true);

private:
    int frameCount;
    float markerLength;
    
    Mat cameraMatrix;
    Mat distCoeffs;

    Dictionary dict;
    MarkerDetector detector;
    MarkerPoseTracker tracker;

    vector<Marker> markers;
    
    void matrix4AddValue(float* mat, float* newmat, float rotateRatio, float transRatio);
    void calculateExtrinsicMat(float* mat4, Mat R, Mat T, bool doesFlip, bool useStabilizer, float rotateStabilizer, float translateStabilizer);
};

#endif /* BECVMarkers_hpp */
