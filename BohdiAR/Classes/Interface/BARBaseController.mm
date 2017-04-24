#import "BARBaseController.hpp"
#import "../Core/BARDetector.hpp"

@interface BARBaseController()
{
    int markerId;
    BOOL cameraCalibrated;

    BARDetector* cvManager;
    CvVideoCamera* videoSource;
    CGSize videoSize;
    
    NSString* calibrateFilePath;
}
@end

@implementation BARBaseController

-(float)fieldOfView
{
    AVCaptureDevice* device =[AVCaptureDevice defaultDeviceWithDeviceType:AVCaptureDeviceTypeBuiltInWideAngleCamera
                                                                mediaType:AVMediaTypeVideo
                                                                 position:AVCaptureDevicePositionBack];
    float fov = device.activeFormat.videoFieldOfView;
    NSLog(@"VIDEO FOV:%f\n", fov);
    return fov;
}

-(float)fieldOfViewCalibratedX
{
    if (cvManager) {
        float fov = (float)cvManager->fovx;
        return fov;
    }
    return [self fieldOfView];
}

-(float)fieldOfViewCalibratedY
{
    if (cvManager) {
        float fov = (float)cvManager->fovy;
        return fov;
    }
    return [self fieldOfView];
}

-(CGSize)videoSize
{
    CGFloat scale = [[UIScreen mainScreen] scale];
    return CGSizeMake(videoSize.width / scale,
                      videoSize.height / scale);
}

-(CALayer *)cvlayer
{
    return videoSource.captureVideoPreviewLayer;
}

-(void)setup
{
    markerId = -1;
    cvManager = nil;
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
    cvManager->drawChessboard = true;
    cvManager->drawMarker = false;
    cvManager->drawAxis = false;
    calibrateFilePath = path;
}

-(void) configDetectorStabilier:(BOOL)use Rotate:(float)rotate Translate:(float)translate
{
    if (cvManager) {
        cvManager->useStabilizer = use ? true : false;
        cvManager->rotateStabilizer = rotate;
        cvManager->translateStabilizer = translate;
    }
}

-(void) startDetectorWithOverLayer:(CALayer*)layer
{
    cvManager->markerId = -1;
    
    const char* camCalibrateFile = [calibrateFilePath cStringUsingEncoding:NSUTF8StringEncoding];
    cameraCalibrated = cvManager->calibrateFileLoad(camCalibrateFile) ? YES : NO;
    
    [videoSource start];
    [videoSource.captureVideoPreviewLayer addSublayer:layer];
}

-(void) startDetectorWithOverView:(UIView*)view
{
    cvManager->markerId = -1;
    
    const char* camCalibrateFile = [calibrateFilePath cStringUsingEncoding:NSUTF8StringEncoding];
    cameraCalibrated = cvManager->calibrateFileLoad(camCalibrateFile) ? YES : NO;
    
    [videoSource start];
    [self.view addSubview:view];
}

-(void) stopDetector
{
    [videoSource stop];
}

-(void) lockFocus
{
    [videoSource lockFocus];
    [videoSource lockExposure];
    [videoSource lockBalance];
}

-(void) unlockFocus
{
    [videoSource unlockFocus];
    [videoSource unlockExposure];
    [videoSource unlockBalance];
}

-(void) useAVCaptureVideoPreviewLayer:(BOOL)usePreview drawDebugRect:(BOOL)debug
{
    videoSource.useAVCaptureVideoPreviewLayer = usePreview;
    if (cvManager) {
        cvManager->drawMarker = (debug == YES) ? true : false;
        cvManager->drawAxis   = (debug == YES) ? true : false;
    }
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
    CGSize size = self.view.frame.size;
    CGFloat scale = [UIScreen mainScreen].scale;
    
    videoSource = [[CvVideoCamera alloc] initWithParentView:self.view];
    videoSource.defaultAVCaptureDevicePosition   = AVCaptureDevicePositionBack;
    videoSource.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;

    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        //OpenCV allow max 1280x720 resolution
        videoSource.defaultAVCaptureSessionPreset = AVCaptureSessionPreset1280x720;
        videoSize.width  = 720;
        videoSize.height = 1280;
    }
    else if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad) {
        videoSource.defaultAVCaptureSessionPreset = AVCaptureSessionPreset1280x720;
        videoSize.width  = 720;
        videoSize.height = 1280;
    }

    videoSource.recordVideo = NO;
    videoSource.rotateVideo = NO;
    videoSource.defaultFPS = 60;//max
    videoSource.delegate = self;
    
    videoSource.videoCaptureConnection.preferredVideoStabilizationMode = AVCaptureVideoStabilizationModeCinematic;
    [self useAVCaptureVideoPreviewLayer:YES drawDebugRect:YES];
}

//conform CvVideoCameraDelegate, image colorspace is BGRA
- (void)processImage:(cv::Mat&)mat
{
    if (cvManager && self.delegate) {
        if (cameraCalibrated == NO) {
            const char* camCalibrateFile = [calibrateFilePath cStringUsingEncoding:NSUTF8StringEncoding];
            cameraCalibrated = cvManager->calibrateCam(mat, camCalibrateFile) ? YES : NO;
            
        } else {
            if (cvManager->processImage(mat)) {
                //[self lockFocus];
                if (cvManager->markerId != markerId) {
                    markerId = cvManager->markerId;
                    [self.delegate onDetectArUcoMarker:cvManager->markerId];
                }
                //update extrinsic matrix
                [self.delegate onUpdateExtrinsicMat:&cvManager->extrinsicMatColumnMajor[0]];
            } else {
                //[self unlockFocus];
            }
        }
    }
}

@end

