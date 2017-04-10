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

@property (nonatomic, weak) id<BARDelegate> delegate;
@property (atomic, readwrite) BOOL drawDebugRect;
@property (atomic, readwrite) BOOL drawDebugAxis;

-(void) configDetectorWithMarker:(CGSize)size Unit:(float)unit Pattern:(BARCalibratePattern)pattern CalibrateFilePath:(NSString*)path;
-(void) startDetector;
-(void) stopDetector;

@end

#endif /* BEARViewController_h */
