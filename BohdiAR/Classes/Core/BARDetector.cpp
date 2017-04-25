#include "BARDetector.hpp"

BARDetector::BARDetector(int width, int height, float unit, Pattern patternType, int flags, bool RANSAC)
{
    markerId = -1;
    markerDetector = new BARMarkers(unit, DICT_ARUCO_ORIGINAL, RANSAC, flags);
    drawChessboard = true;
    drawMarker = true;
    drawAxis = true;
    
    boardSize = Size_<int>(width, height);
    unitSize  = unit;
    pattern   = patternType;
    
    estimateFlags = flags;
    useRANSAC = RANSAC;
    
    useStabilizer = true;
    rotateStabilizer    = 0.6;
    translateStabilizer = 0.4;
    frameCount = 0;
    
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

bool BARDetector::detect(Mat& image)
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

double BARDetector::calibrate(Mat& image)
{
    vector<vector<Point2f>> points2DArray;
    vector<vector<Point3f>> points3DArray;
    points2DArray.push_back(points2D);
    points3DArray.push_back(points3D);
    points3DArray.resize(points2DArray.size(), points3DArray[0]);

    return calibrateCamera(points3DArray, points2DArray, image.size(), cameraMatrix, distCoeffs, noArray(), noArray());
}

bool BARDetector::estimate(int flags)
{
    bool OK;
    if (useRANSAC) {
        OK = solvePnPRansac(points3D, points2D, cameraMatrix, distCoeffs, R, T, false, flags);
    }
    else {
        OK = solvePnP(points3D, points2D, cameraMatrix, distCoeffs, R, T, false, flags);
    }
    
    //if (OK) {
        calculateExtrinsicMat(true);
    //} else {
        
    //}
    
    return OK;
}

void BARDetector::matrix4AddValue(float* mat, float* newmat, float rotateRatio, float transRatio)
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

void BARDetector::calculateExtrinsicMat(bool flip)
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
    
    float scale = 1.0;
    float NewMat[16] = {
        (float)Rotate.at<double>(0, 0),
        (float)Rotate.at<double>(1, 0),
        (float)Rotate.at<double>(2, 0),
        0.0,
        
        (float)Rotate.at<double>(0, 1),
        (float)Rotate.at<double>(1, 1),
        (float)Rotate.at<double>(2, 1),
        0.0,
        
        (float)Rotate.at<double>(0, 2),
        (float)Rotate.at<double>(1, 2),
        (float)Rotate.at<double>(2, 2),
        0.0f,
        
        scale * (float)Translate.at<double>(0, 0),
        scale * (float)Translate.at<double>(1, 0),
        scale * (float)Translate.at<double>(2, 0),
        1.0
    };
    
    if (useStabilizer && frameCount > 0) {
        matrix4AddValue(&extrinsicMatColumnMajor[0], &NewMat[0], rotateStabilizer, translateStabilizer);
    } else {
        frameCount = 1;
        for (int i=0; i<16; i++) {
            extrinsicMatColumnMajor[i] = NewMat[i];
        }
    }
}

bool BARDetector::calibrateFileLoad(const char* calibrateFile)
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

bool BARDetector::calibrateCam(Mat& image, const char* calibrateFile, double sensorWidth, double sensorHeight)
{
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

bool BARDetector::processImage(Mat& image) {
    try {
        Mat copy, rgb, gray;
        //image.copyTo(copy);

        cvtColor(image, rgb, COLOR_BGRA2RGB);
        //cvtColor(copy, gray, COLOR_BGRA2GRAY);
        if (markerDetector->detect(rgb)) {
            markerDetector->estimate(cameraMatrix, distCoeffs, R, T);
            //draw
            if(drawMarker) markerDetector->draw(rgb);
            if(drawAxis) markerDetector->axis(rgb, cameraMatrix, distCoeffs, R, T);
            calculateExtrinsicMat(true);
            cvtColor(rgb, image, COLOR_RGB2BGRA);
            //cvtColor(gray, image, COLOR_GRAY2BGRA);
            markerId = markerDetector->getId();
            return true;
        }
        
    } catch (exception& e) {
        cout << e.what() << '\n';
    }
    markerId = -1;
    return false;
}

