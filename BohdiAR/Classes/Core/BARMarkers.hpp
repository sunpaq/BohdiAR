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
    BARMarkers(float length, Dictionary::DICT_TYPES preDefine = Dictionary::ARTAG, int border = 1, bool RANSAC = false, int flags = SOLVEPNP_ITERATIVE);
    bool detect(Mat& image);
    void draw(Mat& image);
    void axis(Mat& image, Mat cameraMatrix, Mat distCoeffs, Mat rvec, Mat tvec);
    void estimateRTVecs(Mat cameraMatrix, Mat distCoeffs, Mat& rvec, Mat& tvec);
    void estimateModelViewMat(Mat cameraMatrix, Mat distCoeffs, double* modelViewMat);
    int getId();
    
private:
    bool useRANSAC;
    float markerLength;

    cv::Size imageSize;
    Dictionary dict;
    MarkerDetector detector;
    MarkerPoseTracker tracker;
    
    //Ptr<Dictionary> dict;
    //Ptr<DetectorParameters> params;
    
    Mat objPoints;
    
    //vector<vector<Point2f>> corners;
    //vector<int> markerIds;
    vector<Marker> markers;
    
    int estimateFlags;
};

#endif /* BECVMarkers_hpp */
