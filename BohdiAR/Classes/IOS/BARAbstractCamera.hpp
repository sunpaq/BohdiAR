//
//  BARAbstractCamera.h
//  Pods
//
//  Created by YuliSun on 26/04/2017.
//
//

#include <opencv2/core.hpp>

#import <UIKit/UIKit.h>
#import <Accelerate/Accelerate.h>
#import <AVFoundation/AVFoundation.h>
#import <ImageIO/ImageIO.h>

@class BARAbstractCamera;

@interface BARAbstractCamera : NSObject
{
    UIDeviceOrientation currentDeviceOrientation;
    
    BOOL cameraAvailable;
}

@property (nonatomic, strong) AVCaptureSession* captureSession;
@property (nonatomic, strong) AVCaptureConnection* videoCaptureConnection;

@property (nonatomic, readonly) BOOL running;
@property (nonatomic, readonly) BOOL captureSessionLoaded;

@property (nonatomic, assign) int defaultFPS;
@property (nonatomic, readonly) AVCaptureVideoPreviewLayer *captureVideoPreviewLayer;
@property (nonatomic, assign) AVCaptureDevicePosition defaultAVCaptureDevicePosition;
@property (nonatomic, assign) AVCaptureVideoOrientation defaultAVCaptureVideoOrientation;
@property (nonatomic, assign) BOOL useAVCaptureVideoPreviewLayer;
@property (nonatomic, strong) NSString *const defaultAVCaptureSessionPreset;

@property (nonatomic, assign) int imageWidth;
@property (nonatomic, assign) int imageHeight;

@property (nonatomic, strong) UIView* parentView;

- (void)start;
- (void)stop;
- (void)switchCameras;

- (id)initWithParentView:(UIView*)parent;

- (void)createCaptureOutput;
- (void)createVideoPreviewLayer;
- (void)updateOrientation;

- (void)lockFocus;
- (void)unlockFocus;
- (void)lockExposure;
- (void)unlockExposure;
- (void)lockBalance;
- (void)unlockBalance;

@end
