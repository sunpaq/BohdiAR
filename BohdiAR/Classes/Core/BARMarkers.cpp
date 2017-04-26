//
//  BECVMarkers.cpp
//  BEDemo
//
//  Created by YuliSun on 17/03/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#import "BARMarkers.hpp"
#import <opencv2/imgproc.hpp>

BARMarkers::BARMarkers(Mat cameraMatrix, Mat distCoeffs, float length, Dictionary::DICT_TYPES preDefine)
{
    frameCount = 0;
    markerLength = length;
    
    this->cameraMatrix = cameraMatrix;
    this->distCoeffs = distCoeffs;
    
    detector = MarkerDetector();
    detector.setDictionary(preDefine);
    
    markers  = vector<Marker>();
    trackers = vector<MarkerPoseTracker>();
}

int BARMarkers::detect(Mat& image, bool draw)
{
    //do not estimate the pose at this step!
    detector.detect(image, markers);
    if (markers.size() > 0) {
        if (draw) {
            for (int i=0; i<markers.size(); i++) {
                markers[i].draw(image, Scalar(0, 255, 0, 255));
            }
        }
    }
    return markers.size();
}

int BARMarkers::estimate(Mat& image, bool drawAxis)
{
    if (markers.size() > 0) {
        //create as many trackers as detected markers
        if (trackers.size() != markers.size()) {
            trackers.clear();
            for (int i=0; i<markers.size(); i++) {
                trackers.push_back(MarkerPoseTracker());
            }
        }
        
        Size imageSize = Size(image.rows, image.cols);
        CameraParameters campara = CameraParameters(cameraMatrix, distCoeffs, imageSize);
        for (int i=0; i<markers.size(); i++) {
            trackers[i].estimatePose(markers[i], campara, markerLength, 1);
            if (drawAxis) {
                CvDrawingUtils::draw3dAxis(image, campara, markers[i].Rvec, markers[i].Tvec, 2.0);
            }
        }
    }
    return markers.size();
}

int BARMarkers::getId(int index)
{
    return markers[index].id;
}

void BARMarkers::getPoseMat(int index, double* mat4)
{
    //calculateExtrinsicMat((float*)mat4, markers[index].Rvec, markers[index].Tvec, true, false, 0, 0);
    markers[index].glGetModelViewMatrix(mat4);
}

void BARMarkers::matrix4AddValue(float* mat, float* newmat, float rotateRatio, float transRatio)
{
    for (int i=0; i<16; i++) {
        if (i == 12 || i == 13 || i == 14) {
            float sum = mat[i] * (1-transRatio) + newmat[i] * transRatio;
            mat[i] = sum;
        } else {
            float sum = mat[i] * (1-rotateRatio) + newmat[i] * rotateRatio;
            mat[i] = sum;
        }
        
    }
}

void BARMarkers::calculateExtrinsicMat(float* mat4, Mat R, Mat T, bool doesFlip, bool useStabilizer, float rotateStabilizer, float translateStabilizer)
{
    Mat Rod(3,3,DataType<float>::type);
    Mat Rotate, Translate;
    
    Rodrigues(R, Rod);
    //cout << "Rodrigues = "<< endl << " "  << Rod << endl << endl;
    
    if (doesFlip) {
        static float flip[] = {
            1, 0, 0,
            0,-1, 0,
            0, 0,-1
        };
        Mat_<float> flipX(3,3,flip);
        
        Rotate = flipX * Rod;
        Translate = flipX * T;
    } else {
        Rotate = Rod;
        Translate = T;
    }
    
    float scale = 1.0;
    float NewMat[16] = {
        (float)Rotate.at<float>(0, 0),
        (float)Rotate.at<float>(1, 0),
        (float)Rotate.at<float>(2, 0),
        0.0,
        
        (float)Rotate.at<float>(0, 1),
        (float)Rotate.at<float>(1, 1),
        (float)Rotate.at<float>(2, 1),
        0.0,
        
        (float)Rotate.at<float>(0, 2),
        (float)Rotate.at<float>(1, 2),
        (float)Rotate.at<float>(2, 2),
        0.0f,
        
        scale * (float)Translate.at<float>(0, 0),
        scale * (float)Translate.at<float>(1, 0),
        scale * (float)Translate.at<float>(2, 0),
        1.0
    };
    
    if (useStabilizer && frameCount > 0) {
        matrix4AddValue(mat4, &NewMat[0], rotateStabilizer, translateStabilizer);
    } else {
        frameCount = 1;
        for (int i=0; i<16; i++) {
            mat4[i] = NewMat[i];
        }
    }
}
