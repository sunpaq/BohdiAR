#import "BARBaseController.hpp"
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

-(void)setDrawDebugAxis:(BOOL)drawDebugAxis
{
    if (cvManager) {
        cvManager->drawAxis = (_Bool)drawDebugAxis;
    }
}

-(void)setDrawDebugRect:(BOOL)drawDebugRect
{
    if (cvManager) {
        cvManager->drawRect = (_Bool)drawDebugRect;
    }
}

-(CALayer *)cvlayer
{
    return videoSource.captureVideoPreviewLayer;
}

-(void)setup
{
    markerId = 0;
    cvManager = nil;
    
    self.drawDebugAxis = true;
    self.drawDebugRect = true;
    
    //CGRect frame = [[UIScreen mainScreen] bounds];
    //cvView = [[UIView alloc] initWithFrame:frame];
    
    //videoSource = [[CvVideoCamera alloc] initWithParentView:self.view];
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
               rotateUpdateRatio:(float)rratio
                transUpdateRatio:(float)tratio
                         Pattern:(BARCalibratePattern)pattern
               CalibrateFilePath:(NSString*)path
{
    if (cvManager) {
        delete cvManager;
    }
    cvManager = new BARDetector(size.width, size.height, unit, (BARDetector::Pattern)pattern);
    cvManager->rotateUpdateRatio = rratio;
    cvManager->transUpdateRatio  = tratio;
    calibrateFilePath = path;
    
}

-(void) startDetectorWithOverlay:(CALayer*)overlay
{
    [videoSource start];
    [videoSource.captureVideoPreviewLayer addSublayer:overlay];
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
    self.view.backgroundColor = [UIColor blackColor];
    
    videoSource = [[CvVideoCamera alloc] initWithParentView:self.view];
    videoSource.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;
    videoSource.defaultAVCaptureDevicePosition   = AVCaptureDevicePositionBack;
    videoSource.defaultAVCaptureSessionPreset    = AVCaptureSessionPresetPhoto;
    videoSource.useAVCaptureVideoPreviewLayer    = YES;
    
    //CGRect frame = [[UIScreen mainScreen] bounds];
    //videoSource.imageWidth  = frame.size.width;
    //videoSource.imageHeight = frame.size.height;
    
    videoSource.recordVideo = NO;
    videoSource.defaultFPS = 30;//max
    videoSource.delegate = self;
    
    //[self.view addSubview:cvView];
}

//conform CvVideoCameraDelegate, image colorspace is BGRA
- (void)processImage:(cv::Mat&)mat
{
    if (cvManager && self.delegate) {
        if (cameraCalibrated == NO) {
            const char* camCalibrateFile = [calibrateFilePath cStringUsingEncoding:NSUTF8StringEncoding];
            cameraCalibrated = cvManager->calibrateCam(mat, camCalibrateFile);
            
        } else {
            if (cvManager->processImage(mat)) {
                if (cvManager->markerId != markerId) {
                    markerId = cvManager->markerId;
                    [self.delegate onDetectArUcoMarker:cvManager->markerId];
                }
                //update extrinsic matrix
                [self.delegate onUpdateExtrinsicMat:(float*)&cvManager->extrinsicMatColumnMajor[0]];
            }
        }
    }
}

@end

