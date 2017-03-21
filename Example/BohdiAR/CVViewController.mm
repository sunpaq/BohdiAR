//#ifdef USE_CVCONTROLLER

#import "CVViewController.h"
#import <BohdiAR/BohdiAR-umbrella.h>

@interface CVViewController()
{
    int markerId;
    BOOL modelLoaded;
    BECVDetector* cvManager;
}
@end

@implementation CVViewController

//conform CvVideoCameraDelegate, image colorspace is BGRA
- (void)processImage:(cv::Mat&)mat
{
    if (cvManager) {
        if (cvManager->processImage(mat)) {
            if (cvManager->markerId != markerId) {
                markerId = cvManager->markerId;

                GLKVector3 lpos = {0,0,-2000};
                [_beViewCtl lightReset:&lpos];
                if (markerId == 33) {
                    [_beViewCtl removeCurrentModel];
                    [_beViewCtl addModelNamed:@"arcanegolem.obj"];
                }
                else if (markerId == 847) {
                    [_beViewCtl removeCurrentModel];
                    [_beViewCtl addModelNamed:@"2.obj"];
                }
            }
//            if (!modelLoaded) {
//                modelLoaded = YES;
//            }
        }
        [_beViewCtl cameraReset:&cvManager->extrinsicMatColumnMajor[0]];
    }
}

-(void)viewDidLoad
{
    [super viewDidLoad];
    
    markerId = 0;
    modelLoaded = NO;
    cvManager = new BECVDetector(5,4,10,BECVDetector::CHESSBOARD);
    //cvManager = new BECVDetector(5,3,0.04,BECVDetector::ASYMMETRIC_CIRCLES_GRID);
    
    //CGRect frame = [[UIScreen mainScreen] bounds];
    CGRect frame = self.view.frame;
    _cvView = [[UIView alloc] initWithFrame:frame];

    _videoSource = [[CvVideoCamera alloc] initWithParentView:_cvView];
    _videoSource.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;
    _videoSource.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    _videoSource.defaultAVCaptureSessionPreset  = AVCaptureSessionPreset640x480; //AVCaptureSessionPresetLow;
    _videoSource.imageWidth  = frame.size.width;
    _videoSource.imageHeight = frame.size.height;
    _videoSource.defaultFPS = 30;
    _videoSource.delegate = self;
    
    [self.view addSubview:_cvView];

    _beViewCtl = [[BEViewController alloc] init];
    _beViewCtl.useTransparentBackground = YES;
}

-(void)viewDidAppear:(BOOL)animated
{
    //start openCV
    [self.videoSource start];
    [self presentViewController:_beViewCtl animated:NO completion:^{
        
    }];
}

@end

//#endif


