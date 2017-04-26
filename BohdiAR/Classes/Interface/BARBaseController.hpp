#ifndef BEARViewController_h
#define BEARViewController_h

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "BARVideoCamera.hpp"
//#import <opencv2/videoio/cap_ios.h>

enum BARCalibratePattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };

@protocol BARDelegate <NSObject>

-(void) onDetectArUcoMarker:(int)markerId Index:(int)index;
-(void) onUpdateExtrinsicMat:(double*)extMat Index:(int)index;
-(void) onImageProcessDone;

@end

@interface BARBaseController : UIViewController <BARVideoCameraDelegate>

@property (atomic, weak) id<BARDelegate> delegate;
@property (atomic, readonly) CALayer* cvlayer;
@property (atomic, readonly) CGSize videoSize;
@property (atomic, readonly) float videoAspect;

@property (atomic, readonly) float fieldOfView;
@property (atomic, readonly) float fieldOfViewCalibratedX;
@property (atomic, readonly) float fieldOfViewCalibratedY;
@property (atomic, readonly) CGPoint principalPoint;

-(void) addOverview:(UIView*)view;
-(void) configDetectorWithCameraParameters:(NSString*)filePath MarkerLength:(float)length;
-(void) configDetectorStabilier:(BOOL)use Rotate:(float)rotate Translate:(float)translate;

-(void) startDetector;
-(void) stopDetector;

-(void) lockFocus;
-(void) unlockFocus;

@end

#endif /* BEARViewController_h */
