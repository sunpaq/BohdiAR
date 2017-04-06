#import <AVFoundation/AVFoundation.h>
#import "BARBaseController.h"
#import "../Core/BARDetector.hpp"

@interface BARBaseController()
{
    int markerId;
    BOOL cameraCalibrated;

    BARDetector* cvManager;
    CvVideoCamera* videoSource;
    UIView* cvView;
    
    NSString* calibrateFilePath;
}
@end

@implementation BARBaseController

-(void)setup
{
    markerId = 0;
    cvManager = nil;
    
    CGRect frame = [[UIScreen mainScreen] bounds];
    cvView = [[UIView alloc] initWithFrame:frame];
    
    videoSource = [[CvVideoCamera alloc] initWithParentView:cvView];
}

-(void)dealloc
{
    if (cvManager) {
        delete cvManager;
    }
}

//int width, int height, float unit, Pattern patternType, int flags = CV_ITERATIVE, bool RANSAC = true
//cvManager = new BARDetector(5,4,10,BARDetector::CHESSBOARD);
-(void) configDetectorWithMarker:(CGSize)size
                            Unit:(float)unit
                         Pattern:(BARCalibratePattern)pattern
               CalibrateFilePath:(NSString*)path
{
    if (cvManager) {
        delete cvManager;
    }
    cvManager = new BARDetector(size.width, size.height, unit, (BARDetector::Pattern)pattern);
    calibrateFilePath = path;
}

-(void)startDetector
{
    [videoSource start];
}

-(void) stopDetector
{
    [videoSource stop];
}

-(instancetype)init
{
    self = [super init];
    if (self) {
        [self setup];
    }
    return self;
}

-(instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        [self setup];
    }
    return self;
}

-(instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        [self setup];
    }
    return self;
}

-(void)viewDidLoad
{
    [super viewDidLoad];
    
    videoSource.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;
    videoSource.defaultAVCaptureDevicePosition   = AVCaptureDevicePositionBack;
    videoSource.defaultAVCaptureSessionPreset    = AVCaptureSessionPreset640x480;
    
    CGRect frame = [[UIScreen mainScreen] bounds];
    videoSource.imageWidth  = frame.size.width;
    videoSource.imageHeight = frame.size.height;
    
    videoSource.defaultFPS = 30;
    videoSource.delegate = self;
    
    [self.view addSubview:cvView];
}

//conform CvVideoCameraDelegate, image colorspace is BGRA
- (void)processImage:(cv::Mat&)mat
{
    if (cvManager && self.delegate) {
        if (cameraCalibrated == NO) {
            const char* camCalibrateFile = [calibrateFilePath cStringUsingEncoding:kCFStringEncodingUTF8];
            cameraCalibrated = cvManager->calibrateCam(mat, camCalibrateFile);
            
        } else {
            if (cvManager->processImage(mat)) {
                if (cvManager->markerId != markerId) {
                    markerId = cvManager->markerId;
                    [self.delegate onDetectArUcoMarker:cvManager->markerId];
                }
                //update extrinsic matrix
                [self.delegate onUpdateExtrinsicMat:&cvManager->extrinsicMatColumnMajor[0]];
            }
        }
    }
}

@end

