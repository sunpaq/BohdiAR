/*
 please subclass this base view controller
 and integrate it into your UI flow
 */

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

/*
 as GLKView and CALayer not act as normal UIViews
 please use the following designated containers to customize your UI
 videoContainer in the lowest layer openglContainer in the middle
 add your 3D content view as a subview of openglContainer
 add your UI element view as a subview of uiContainer
 */

@interface BARBaseController : UIViewController <BARVideoCameraDelegate>

@property (atomic, readonly) UIView* videoContainer;
@property (atomic, readonly) UIView* openglContainer;
@property (atomic, readonly) UIView* uiContainer;

@property (atomic, weak) id<BARDelegate> delegate;
@property (atomic, readonly) CALayer* cvlayer;
@property (atomic, readonly) CGSize videoSize;
@property (atomic, readonly) float videoAspect;

@property (atomic, readonly) float fieldOfView;
@property (atomic, readonly) float fieldOfViewCalibratedX;
@property (atomic, readonly) float fieldOfViewCalibratedY;
@property (atomic, readonly) CGPoint principalPoint;

@property (atomic, readwrite) BOOL drawDebugInfo;

-(void) configDetectorWithCameraParameters:(NSString*)filePath MarkerLength:(float)length;
-(void) configDetectorStabilier:(BOOL)use Rotate:(float)rotate Translate:(float)translate;

-(void) startDetector;
-(void) stopDetector;

-(void) lockFocus;
-(void) unlockFocus;

@end

#endif /* BEARViewController_h */
