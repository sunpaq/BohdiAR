
#import "BARBaseController.h"
#import "BARDetector.hpp"

@interface BARBaseController()
{
    int markerId;
    BOOL modelLoaded;
    BARDetector* cvManager;
    
    CvVideoCamera* videoSource;
    UIView* cvView;
}
@end

@implementation BARBaseController

-(void) startDetect
{
    [videoSource start];
}

//conform CvVideoCameraDelegate, image colorspace is BGRA
- (void)processImage:(cv::Mat&)mat
{
    if (cvManager) {
        if (cvManager->processImage(mat)) {
            if (cvManager->markerId != markerId) {
                
                if (delegate) {
                    [delegate]
                }
                
                markerId = cvManager->markerId;

//                GLKVector3 lpos = {0,0,-2000};
//                [_beViewCtl lightReset:&lpos];
//                if (markerId == 33) {
//                    [_beViewCtl removeCurrentModel];
//                    [_beViewCtl addModelNamed:@"arcanegolem.obj"];
//                }
//                else if (markerId == 847) {
//                    [_beViewCtl removeCurrentModel];
//                    [_beViewCtl addModelNamed:@"2.obj"];
//                }
            }
//            if (!modelLoaded) {
//                modelLoaded = YES;
//            }
        }
        [beViewCtl cameraReset:&cvManager->extrinsicMatColumnMajor[0]];
    }
}

-(void)viewDidLoad
{
    [super viewDidLoad];
    
    markerId = 0;
    modelLoaded = NO;
    cvManager = new BARDetector(5,4,10,BARDetector::CHESSBOARD);
    //cvManager = new BECVDetector(5,3,0.04,BECVDetector::ASYMMETRIC_CIRCLES_GRID);
    
    CGRect frame = [[UIScreen mainScreen] bounds];
    cvView = [[UIView alloc] initWithFrame:frame];

    videoSource = [[CvVideoCamera alloc] initWithParentView:cvView];
    videoSource.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;
    videoSource.defaultAVCaptureDevicePosition = AVCaptureDevicePositionBack;
    videoSource.defaultAVCaptureSessionPreset  = AVCaptureSessionPreset640x480; //AVCaptureSessionPresetLow;
    videoSource.imageWidth  = frame.size.width;
    videoSource.imageHeight = frame.size.height;
    videoSource.defaultFPS = 30;
    videoSource.delegate = self;
    
    [self.view addSubview:cvView];

//    _beViewCtl = [[BEViewController alloc] init];
//    _beViewCtl.useTransparentBackground = YES;
}

-(void)viewDidAppear:(BOOL)animated
{
    //start openCV
//    [self presentViewController:_beViewCtl animated:NO completion:^{
//        
//    }];
}

@end

