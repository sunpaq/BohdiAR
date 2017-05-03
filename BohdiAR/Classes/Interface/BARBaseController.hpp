#ifndef BEARViewController_h
#define BEARViewController_h

//import C++ code before all the others
#import "BARVideoCamera.hpp"

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

enum BARCalibratePattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };

@protocol BARDelegate <NSObject>

-(void) onDetectMarker:(int)markerId Index:(int)index;
-(void) onUpdateMarker:(int)markerId Index:(int)index Pose:(float*)mat4;
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

@property (atomic, readwrite) BOOL drawDebugInfo;

-(void) addOverview:(UIView*)view;
-(void) configDetectorWithCameraParameters:(NSString*)filePath MarkerLength:(float)length;
-(void) configDetectorStabilier:(BOOL)use Rotate:(float)rotate Translate:(float)translate;

-(void) startDetector;
-(void) stopDetector;

-(void) lockFocus;
-(void) unlockFocus;

@end

#endif /* BEARViewController_h */
