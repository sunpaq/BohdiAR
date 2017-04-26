#include "BARManager.hpp"

BARManager::BARManager(const char* calibrateFile, int markerLength)
{
    cameraMatrix = Mat::eye(3, 3, CV_32FC1);//CV_64FC1
    distCoeffs   = Mat::zeros(8, 1, CV_32FC1);
    
    if (calibrateFileLoad(calibrateFile)) {
        markerDetector = new BARMarkers(cameraMatrix, distCoeffs, markerLength);
    } else {
        cout << "Can not find camera parameters file: " << calibrateFile << "\n";
        exit(-1);
    }
    
    markerId = -1;
    drawChessboard = true;

    useStabilizer = true;
    rotateStabilizer    = 0.6;
    translateStabilizer = 0.4;
    frameCount = 0;
    
    R = Mat::zeros(3, 1, CV_32FC1);
    T = Mat::zeros(3, 1, CV_32FC1);
    
    points2D = vector<Point2f>();
    points3D = vector<Point3f>();
    
    //prepare marker points
    if (pattern == CHESSBOARD || pattern == CIRCLES_GRID) {
        for( int i = 0; i < boardSize.height; ++i )
            for( int j = 0; j < boardSize.width; ++j )
                points3D.push_back(Point3f(j*unitSize, i*unitSize, 0));
    }
    else if (pattern == ASYMMETRIC_CIRCLES_GRID) {
        for( int i = 0; i < boardSize.height; i++ )
            for( int j = 0; j < boardSize.width; j++ )
                points3D.push_back(Point3f((2*j + i % 2)*unitSize, i*unitSize, 0));
    }
}

bool BARManager::detect(Mat& image)
{
    //chessboard
    bool found = false;
    if (pattern == CHESSBOARD) {
        found = findChessboardCorners(image, boardSize, points2D);

    }
    else if (pattern == CIRCLES_GRID) {
        found = findCirclesGrid(image, boardSize, points2D);
    }
    else if (pattern == ASYMMETRIC_CIRCLES_GRID) {
        found = findCirclesGrid(image, boardSize, points2D, CALIB_CB_ASYMMETRIC_GRID);
    }
    
    if (found) {
        //calculate subpixel
        Size winSize = cv::Size(5,5);
        Size zeroZone = cv::Size(-1,-1);
        cornerSubPix(image, points2D, winSize, zeroZone, TermCriteria(CV_TERMCRIT_ITER, 30, 0.1));
    }
    
    if (found && drawChessboard) {
        drawChessboardCorners(image, boardSize, points2D, true);
    }
    
    return found;
}

double BARManager::calibrate(Mat& image)
{
    vector<vector<Point2f>> points2DArray;
    vector<vector<Point3f>> points3DArray;
    points2DArray.push_back(points2D);
    points3DArray.push_back(points3D);
    points3DArray.resize(points2DArray.size(), points3DArray[0]);

    return calibrateCamera(points3DArray, points2DArray, image.size(), cameraMatrix, distCoeffs, noArray(), noArray());
}

bool BARManager::estimate(int flags)
{
    bool OK;
    if (useRANSAC) {
        OK = solvePnPRansac(points3D, points2D, cameraMatrix, distCoeffs, R, T, false, flags);
    }
    else {
        OK = solvePnP(points3D, points2D, cameraMatrix, distCoeffs, R, T, false, flags);
    }
    
    //if (OK) {
        //calculateExtrinsicMat(true);
    //} else {
        
    //}
    
    return OK;
}

bool BARManager::calibrateFileLoad(const char* calibrateFile)
{
    FileStorage fs;
    fs.open(String(calibrateFile), FileStorage::READ);
    if (fs.isOpened()) {
        fs["fovx"] >> fovx;
        fs["fovy"] >> fovy;
        fs["focalLength"] >> focalLength;
        fs["nonfix_aspectRatio"] >> aspectRatio;
        fs["principalPointX"] >> principalPointX;
        fs["principalPointY"] >> principalPointY;
        
        fs["camera_matrix"]           >> cameraMatrix;
        fs["distortion_coefficients"] >> distCoeffs;
        
        fs.release();
        return true;
    }
    return false;
}

bool BARManager::calibrateCam(Mat& image, const char* calibrateFile, int width, int height, float unit,
                              Pattern patternType, int flags, bool RANSAC,
                              double sensorWidth, double sensorHeight)
{
    boardSize = Size_<int>(width, height);
    unitSize  = unit;
    pattern   = patternType;
    
    estimateFlags = flags;
    useRANSAC = RANSAC;
    
    Mat gray;
    cvtColor(image, gray, COLOR_BGRA2GRAY);
    
    if (detect(gray)) {
        //root mean square
        double RMS = calibrate(gray);
        
        if(RMS < 0.0 || RMS > 1.0 || !checkRange(cameraMatrix) || !checkRange(distCoeffs)){
            return false;
            
        } else {
            //iPhone5s/6 4.8mm x 3.6mm
            double fovx, fovy, focalLength, aspectRatio;
            Point2d principalPoint;
            
            cv::calibrationMatrixValues(cameraMatrix, Size_<int>(image.rows, image.cols), sensorWidth, sensorHeight,
                                    fovx, fovy, focalLength, principalPoint, aspectRatio);

            FileStorage fs;
            fs.open(String(calibrateFile), FileStorage::WRITE);
            if (fs.isOpened()) {
                CvMat cam = cameraMatrix;
                CvMat dis = distCoeffs;
                fs.write("avg_reprojection_error", RMS);
                
                fs.write("fovx", fovx);
                fs.write("fovy", fovy);
                fs.write("focalLength", focalLength);
                fs.write("nonfix_aspectRatio", aspectRatio);
                fs.write("principalPointX", principalPoint.x);
                fs.write("principalPointY", principalPoint.y);

                fs.writeObj("camera_matrix", &cam);
                fs.writeObj("distortion_coefficients", &dis);
                fs.release();
                
                cout << "CAMERA CALIBRATE SUCCESS: " << RMS << "\n";
                return true;
            }
        }
    }
    
    return false;
}

//bool BARManager::processImage(Mat& image, float extrinsicMatColumnMajor[16]) {
//    Mat RGB;
//    try {
//        cvtColor(image, RGB, COLOR_BGRA2RGB);
//        int count = markerDetector->detect(RGB);
//        if (count > 0) {
//            for (int i=0; i<count; i++) {
//                markerDetector->estimate(RGB, i, extrinsicMatColumnMajor);
//                markerId = markerDetector->getId(i);
//            }
//            cvtColor(RGB, image, COLOR_RGB2BGRA);
//            return true;
//        }
//
//    } catch (exception& e) {
//        cout << e.what() << '\n';
//    }
//    
//    markerId = -1;
//    cvtColor(RGB, image, COLOR_RGB2BGRA);
//    return false;
//}

int BARManager::detectMarkers(Mat& rgbImage)
{
    return markerDetector->detect(rgbImage);
}

int BARManager::getMarkerId(int index)
{
    return markerDetector->getId(index);
}

void BARManager::estimateMarkers(Mat& rgbImage)
{
    markerDetector->estimate(rgbImage);
}

void BARManager::getMarkerPose(int index, double* poseMat)
{
    markerDetector->getPoseMat(index, poseMat);
}





