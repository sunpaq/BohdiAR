//
//  BECVMarkers.cpp
//  BEDemo
//
//  Created by YuliSun on 17/03/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#import "BARMarkers.hpp"
#import <opencv2/imgproc.hpp>

/* the default values

adaptiveThreshWinSizeMin(3),
adaptiveThreshWinSizeMax(23),
adaptiveThreshWinSizeStep(10),
adaptiveThreshConstant(7),
minMarkerPerimeterRate(0.03),
maxMarkerPerimeterRate(4.),
polygonalApproxAccuracyRate(0.03),
minCornerDistanceRate(0.05),
minDistanceToBorder(3),
minMarkerDistanceRate(0.05),
doCornerRefinement(false),
cornerRefinementWinSize(5),
cornerRefinementMaxIterations(30),
cornerRefinementMinAccuracy(0.1),
markerBorderBits(1),
perspectiveRemovePixelPerCell(4),
perspectiveRemoveIgnoredMarginPerCell(0.13),
maxErroneousBitsInBorderRate(0.35),
minOtsuStdDev(5.0),
errorCorrectionRate(0.6)
*/

BARMarkers::BARMarkers(float length, PREDEFINED_DICTIONARY_NAME preDefine, bool RANSAC, int flags)
{
    useRANSAC = RANSAC;
    markerLength = length;
    estimateFlags = flags;

    dict = getPredefinedDictionary(preDefine);
    params = DetectorParameters::create();    
    params->doCornerRefinement = true;
    params->minDistanceToBorder = 0;
    
    params->adaptiveThreshWinSizeMin = 23;
    params->adaptiveThreshWinSizeMax = 23;
    params->adaptiveThreshWinSizeStep = 23;
    
    params->minMarkerPerimeterRate = 0.1;
    params->maxMarkerPerimeterRate = 1.0;
    params->minMarkerDistanceRate = 100.0;
    
    corners = vector<vector<Point2f>>();
    markerIds = vector<int>();
    
    //create points
    objPoints.create(4, 1, CV_32FC3);
    // set coordinate system in the middle of the marker, with Z pointing out
    objPoints.ptr< Vec3f >(0)[0] = Vec3f(-markerLength / 2.f, markerLength / 2.f, 0);
    objPoints.ptr< Vec3f >(0)[1] = Vec3f(markerLength / 2.f, markerLength / 2.f, 0);
    objPoints.ptr< Vec3f >(0)[2] = Vec3f(markerLength / 2.f, -markerLength / 2.f, 0);
    objPoints.ptr< Vec3f >(0)[3] = Vec3f(-markerLength / 2.f, -markerLength / 2.f, 0);
}

bool BARMarkers::detect(Mat& image)
{
    corners.clear();
    markerIds.clear();
    
    int channels = ((InputArray)image).getMat().channels();
    if (channels != 1 && channels != 3) {
        cout << "channels:" << channels << '\n';
    }
    
    detectMarkers(image, dict, corners, markerIds, params);
    if (markerIds.size() > 0) {
        //calculate subpixel
        return true;
    }
    return false;
}

void BARMarkers::draw(Mat& image)
{
    drawDetectedMarkers(image, corners, markerIds);
}

void BARMarkers::axis(Mat& image, Mat cameraMatrix, Mat distCoeffs, Mat rvec, Mat tvec)
{
    drawAxis(image, cameraMatrix, distCoeffs, rvec, tvec, markerLength / 0.5);
}

/*
 markerLength:
 
 marker side in meters or in any other unit. 
 Note that the translation vectors of the estimated poses 
 will be in the same unit
 */
void BARMarkers::estimate(const Mat cameraMatrix, const Mat distCoeffs, Mat& rvec, Mat& tvec)
{
    //vector<Vec3d> rvecArray;
    //vector<Vec3d> tvecArray;

    if (useRANSAC) {
        //bool useExtrinsicGuess = false, int iterationsCount = 100,
        //float reprojectionError = 8.0, double confidence = 0.99,
        //OutputArray inliers = noArray(), int flags = SOLVEPNP_ITERATIVE
        solvePnPRansac(objPoints, corners[0], cameraMatrix, distCoeffs, rvec, tvec,
                       false, 50, 4.0, 0.99, noArray(), estimateFlags);
    } else {
        solvePnP(objPoints, corners[0], cameraMatrix, distCoeffs, rvec, tvec,
                 false, estimateFlags);
    }
}

int BARMarkers::getId()
{
    return markerIds[0];
}

