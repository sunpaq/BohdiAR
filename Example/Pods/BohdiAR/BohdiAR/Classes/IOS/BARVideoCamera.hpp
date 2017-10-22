#import "BARAbstractCamera.hpp"

@class BARVideoCamera;

@protocol BARVideoCameraDelegate <NSObject>

#ifdef __cplusplus
// delegate method for processing image frames
- (void)processImage:(cv::Mat&)image;
#endif

@end

@interface BARVideoCamera : BARAbstractCamera<AVCaptureVideoDataOutputSampleBufferDelegate>
{
    AVCaptureVideoDataOutput *videoDataOutput;
    
    dispatch_queue_t videoDataOutputQueue;
    CALayer *customPreviewLayer;
    
    CMTime lastSampleTime;
    
}

@property (nonatomic, weak) id<BARVideoCameraDelegate> delegate;
@property (nonatomic, assign) BOOL grayscaleMode;

@property (nonatomic, assign) BOOL recordVideo;
@property (nonatomic, assign) BOOL rotateVideo;
@property (nonatomic, strong) AVAssetWriterInput* recordAssetWriterInput;
@property (nonatomic, strong) AVAssetWriterInputPixelBufferAdaptor* recordPixelBufferAdaptor;
@property (nonatomic, strong) AVAssetWriter* recordAssetWriter;

- (void)adjustLayoutToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation;
- (void)layoutPreviewLayer;
- (void)saveVideo;
- (NSURL *)videoFileURL;
- (NSString *)videoFileString;


@end
