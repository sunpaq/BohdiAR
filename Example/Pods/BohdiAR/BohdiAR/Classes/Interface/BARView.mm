
#import "BARView.hpp"
#import "../Core/BARManager.hpp"

@interface BARView()
{
    int markerId;
    BOOL cameraCalibrated;

    BARManager* cvManager;
    BARVideoCamera* videoSource;
    CGSize videoSize;
    
    NSString* calibrateFilePath;
}
@end

@implementation BARView

@synthesize videoContainer;
@synthesize openglContainer;
@synthesize uiContainer;

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

-(void)setDrawDebugInfo:(BOOL)drawDebugInfo
{
    if (drawDebugInfo) {
        cvManager->drawMarker = true;
        cvManager->drawAxis = true;
    } else {
        cvManager->drawMarker = false;
        cvManager->drawAxis = false;
    }
}

-(BOOL)drawDebugInfo
{
    return cvManager->drawMarker || cvManager->drawAxis;
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

-(void)dealloc
{
    if (cvManager) {
        delete cvManager;
    }
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

- (instancetype)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    if (self) {
        [self setupWithFrame:self.bounds];
    }
    return self;
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self setupWithFrame:frame];
    }
    return self;
}

- (void)setupWithFrame:(CGRect)frame
{
    markerId = -1;
    cvManager = nil;
    
    self.backgroundColor = [UIColor blackColor];
    //CGSize size = self.view.frame.size;
    //CGFloat scale = [UIScreen mainScreen].scale;
    
    videoContainer = [[UIView alloc] initWithFrame:frame];
    openglContainer = [[UIView alloc] initWithFrame:frame];
    uiContainer = [[UIView alloc] initWithFrame:frame];
    videoContainer.tag  = 99;
    openglContainer.tag = 99;
    uiContainer.tag     = 99;
    
    videoSource = [[BARVideoCamera alloc] initWithParentView:videoContainer];
    videoSource.defaultAVCaptureDevicePosition   = AVCaptureDevicePositionBack;
    videoSource.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;
    
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        //OpenCV allow max 1280x720 resolution
        videoSource.defaultAVCaptureSessionPreset = AVCaptureSessionPreset1280x720;
    }
    else if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad) {
        videoSource.defaultAVCaptureSessionPreset = AVCaptureSessionPreset1280x720;
    }
    
    videoSource.recordVideo = NO;
    videoSource.rotateVideo = NO;
    videoSource.defaultFPS = 60;//max
    videoSource.delegate = self;
    
    videoSource.videoCaptureConnection.preferredVideoStabilizationMode = AVCaptureVideoStabilizationModeOff;
    //videoSource.useAVCaptureVideoPreviewLayer = YES;
    
    [self addSubview:videoContainer];
    [self addSubview:openglContainer];
    [self addSubview:uiContainer];
    
    //move all the existing subviews (add by storyboard) into UI container
    for (UIView* view in self.subviews) {
        if (view.tag != 99) {
            [uiContainer addSubview:view];
        }
    }
}

//conform CvVideoCameraDelegate, image colorspace is BGRA
- (void)processImage:(cv::Mat&)image
{
    if (cvManager && self.delegate) {
        Mat RGB;
        cvtColor(image, RGB, COLOR_BGRA2RGB);
        int count = cvManager->detectMarkers(RGB);
        cvManager->estimateMarkers(RGB);
        if (count > 0) {
            for (int i=0; i<count; i++) {
                float mat4[16] = {0};
                int mid = cvManager->getMarkerId(i);
                cvManager->getMarkerPose(i, mat4);
                [self.delegate onDetectMarker:mid Index:i];
                [self.delegate onUpdateMarker:mid Index:i Pose:mat4];
            }
        }
        cvtColor(RGB, image, COLOR_RGB2BGRA);
        [self.delegate onImageProcessDone];
    }
}

-(void) showHideSubview:(UIView*)view showOrHide:(BOOL)showOrHide
{
    dispatch_async(dispatch_get_main_queue(), ^{
        view.hidden = !showOrHide;
    });
}

@end

