#ifndef BEARViewController_h
#define BEARViewController_h

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <opencv2/videoio/cap_ios.h>

enum BARCalibratePattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };

@protocol BARDelegate <NSObject>

-(void) onDetectArUcoMarker:(int)markerId;
-(void) onUpdateExtrinsicMat:(float*)extMat;

@end

@interface BARBaseController : UIViewController <CvVideoCameraDelegate>

@property (atomic, weak) id<BARDelegate> delegate;
@property (atomic, readonly) CALayer* cvlayer;
@property (atomic, readonly) CGSize videoSize;

@property (atomic, readonly) float fieldOfView;
@property (atomic, readonly) float fieldOfViewCalibratedX;
@property (atomic, readonly) float fieldOfViewCalibratedY;

-(void) configDetectorWithMarker:(CGSize)size
                            Unit:(float)unit
                         Pattern:(BARCalibratePattern)pattern
               CalibrateFilePath:(NSString*)path;

-(void) configDetectorStabilier:(BOOL)use Rotate:(float)rotate Translate:(float)translate;

-(void) startDetectorWithOverLayer:(CALayer*)layer;
-(void) startDetectorWithOverView:(UIView*)view;
-(void) stopDetector;

-(void) lockFocus;
-(void) unlockFocus;
-(void) useAVCaptureVideoPreviewLayer:(BOOL)usePreview drawDebugRect:(BOOL)debug;

@end

#endif /* BEARViewController_h */
