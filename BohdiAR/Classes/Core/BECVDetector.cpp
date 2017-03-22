#include "BECVDetector.hpp"

BECVDetector::BECVDetector(int width, int height, float unit, Pattern patternType, int flags, bool RANSAC)
{
    markerDetector = new BECVMarkers(unit);
    
    boardSize = Size_<int>(width, height);
    unitSize  = unit;
    pattern   = patternType;
    
    cameraCalibrated = false;
    estimateFlags = flags;
    useRANSAC = RANSAC;
    
    cameraMatrix = Mat::eye(3, 3, CV_64FC1);
    distCoeffs   = Mat::zeros(8, 1, CV_64FC1);
    
    R = Mat::zeros(3, 1, CV_64FC1);
    T = Mat::zeros(3, 1, CV_64FC1);
    
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

bool BECVDetector::detect(Mat& image)
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
        cornerSubPix(image, points2D, boardSize, Size_<int>(-1,-1), TermCriteria(CV_TERMCRIT_ITER, 30, 0.1));
    }

    return found;
}

double BECVDetector::calibrate(Mat& image)
{
    vector<vector<Point2f>> points2DArray;
    vector<vector<Point3f>> points3DArray;
    points2DArray.push_back(points2D);
    points3DArray.push_back(points3D);
    points3DArray.resize(points2DArray.size(), points3DArray[0]);

    return calibrateCamera(points3DArray, points2DArray, image.size(), cameraMatrix, distCoeffs, noArray(), noArray());
}

bool BECVDetector::estimate(int flags)
{
    bool OK;
    if (useRANSAC) {
        OK = solvePnPRansac(points3D, points2D, cameraMatrix, distCoeffs, R, T, false, flags);
    }
    else {
        OK = solvePnP(points3D, points2D, cameraMatrix, distCoeffs, R, T, false, flags);
    }
    
    if (OK) {
        calculateExtrinsicMat(true);
    }
    
    return OK;
}

void BECVDetector::calculateExtrinsicMat(bool flip)
{
    Mat Rod(3,3,DataType<double>::type);
    Mat Rotate, Translate;
    
    Rodrigues(R, Rod);
    //cout << "Rodrigues = "<< endl << " "  << Rod << endl << endl;
    
    if (flip) {
        static double flip[] = {
            1, 0, 0,
            0,-1, 0,
            0, 0,-1
        };
        Mat_<double> flipX(3,3,flip);
        
        Rotate = flipX * Rod;
        Translate = flipX * T;
    } else {
        Rotate = Rod;
        Translate = T;
    }
    
    float scale = 1;
    
    extrinsicMatColumnMajor[0] = Rotate.at<double>(0, 0);
    extrinsicMatColumnMajor[1] = Rotate.at<double>(1, 0);
    extrinsicMatColumnMajor[2] = Rotate.at<double>(2, 0);
    extrinsicMatColumnMajor[3] = 0.0f;
    
    extrinsicMatColumnMajor[4] = Rotate.at<double>(0, 1);
    extrinsicMatColumnMajor[5] = Rotate.at<double>(1, 1);
    extrinsicMatColumnMajor[6] = Rotate.at<double>(2, 1);
    extrinsicMatColumnMajor[7] = 0.0f;
    
    extrinsicMatColumnMajor[8]  = Rotate.at<double>(0, 2);
    extrinsicMatColumnMajor[9]  = Rotate.at<double>(1, 2);
    extrinsicMatColumnMajor[10] = Rotate.at<double>(2, 2);
    extrinsicMatColumnMajor[11] = 0.0f;
    
    extrinsicMatColumnMajor[12] = scale * Translate.at<double>(0, 0);
    extrinsicMatColumnMajor[13] = scale * Translate.at<double>(1, 0);
    extrinsicMatColumnMajor[14] = scale * Translate.at<double>(2, 0);
    extrinsicMatColumnMajor[15] = 1.0f;
    
}

void BECVDetector::calibrateCam(Mat& image, const char* calibrateFile)
{
    FileStorage fs;
    fs.open(calibrateFile, FileStorage::READ);
    if (fs.isOpened()) {
        fs["Camera_Matrix"]           >> cameraMatrix;
        fs["Distortion_Coefficients"] >> distCoeffs;
        fs.release();
        return;
    }
    
    Mat gray;
    cvtColor(image, gray, COLOR_BGRA2GRAY);
    
    if (detect(gray)) {
        //root mean square
        double RMS = calibrate(gray);
        if(RMS < 0.1 || RMS > 1.0 || !checkRange(cameraMatrix) || !checkRange(distCoeffs)){
            fs.open(calibrateFile, FileStorage::WRITE);
            if (fs.isOpened()) {
                CvMat cam = cameraMatrix;
                CvMat dis = distCoeffs;
                fs.write("Avg_Reprojection_Error", RMS);
                fs.writeObj("Camera_Matrix", &cam);
                fs.writeObj("Distortion_Coefficients", &dis);
                fs.release();
                return;
            }
            
            cameraCalibrated = true;
            return;
        }
    }
}

bool BECVDetector::processImage(Mat& image) {
    try {
        Mat rgb;
        cvtColor(image, rgb, COLOR_BGRA2RGB);
        if (markerDetector->detect(rgb)) {
            markerDetector->estimate(cameraMatrix, distCoeffs, R, T);
            //draw
            markerDetector->draw(rgb);
            markerDetector->axis(rgb, cameraMatrix, distCoeffs, R, T);
            calculateExtrinsicMat(true);
            cvtColor(rgb, image, COLOR_RGB2BGRA);
            
            markerId = markerDetector->getId();
            return true;
        }
        
    } catch (exception& e) {
        cout << e.what() << '\n';
    }
    return false;
}

