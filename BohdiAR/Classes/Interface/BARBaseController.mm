#import "BARBaseController.hpp"
#import "../Core/BARManager.hpp"

@interface BARBaseController()
{
    int markerId;
    BOOL cameraCalibrated;

    UIView* videoPreview;
    
    BARManager* cvManager;
    BARVideoCamera* videoSource;
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

-(CGPoint)principalPoint
{
    if (cvManager) {
        return CGPointMake(cvManager->principalPointX, cvManager->principalPointY);
    }
    return CGPointZero;
}

-(CGSize)videoSize
{
    CGFloat scale = [[UIScreen mainScreen] scale];
    return CGSizeMake(videoSize.width / scale,
                      videoSize.height / scale);
}

-(float)videoAspect
{
    return (float) videoSize.height / videoSize.width;
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

-(void) addOverview:(UIView*)view
{
    [self.view insertSubview:view aboveSubview:videoPreview];
}

//int width, int height, float unit, Pattern patternType, int flags = CV_ITERATIVE, bool RANSAC = true
//cvManager = new BARDetector(5,4,10,BARDetector::CHESSBOARD);
-(void) configDetectorWithCameraParameters:(NSString*)filePath MarkerLength:(float)length
{
    const char* path = [filePath cStringUsingEncoding:NSUTF8StringEncoding];
    if (path) {
        if (cvManager) {
            delete cvManager;
        }
        cvManager = new BARManager(path, length);
    }
}

-(void) configDetectorStabilier:(BOOL)use Rotate:(float)rotate Translate:(float)translate
{
    if (cvManager) {
        cvManager->useStabilizer = use ? true : false;
        cvManager->rotateStabilizer = rotate;
        cvManager->translateStabilizer = translate;
    }
}

-(void) startDetector
{
    cvManager->markerId = -1;
    [videoSource start];
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
    
    videoPreview = [[UIView alloc] initWithFrame:self.view.frame];
    [self.view addSubview:videoPreview];
    
    videoSource = [[BARVideoCamera alloc] initWithParentView:videoPreview];
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
    videoSource.rotateVideo = YES;
    videoSource.defaultFPS = 60;//max
    videoSource.delegate = self;
    
    videoSource.videoCaptureConnection.preferredVideoStabilizationMode = AVCaptureVideoStabilizationModeAuto;
    //videoSource.useAVCaptureVideoPreviewLayer = YES;
}

//conform CvVideoCameraDelegate, image colorspace is BGRA
- (void)processImage:(cv::Mat&)image
{
    if (cvManager && self.delegate) {
//        if (cameraCalibrated == NO) {
//            const char* camCalibrateFile = [calibrateFilePath cStringUsingEncoding:NSUTF8StringEncoding];
//            cameraCalibrated = cvManager->calibrateCam(image, camCalibrateFile) ? YES : NO;
//            
//        } else {
            float mat4[16];
            if (cvManager->processImage(image, mat4)) {
                //[self lockFocus];
                if (cvManager->markerId != markerId) {
                    markerId = cvManager->markerId;
                    [self.delegate onDetectArUcoMarker:cvManager->markerId];
                }
                //update extrinsic matrix
                [self.delegate onUpdateExtrinsicMat:mat4];
            } else {
                //[self unlockFocus];
            }
        //}
    }
}

@end

