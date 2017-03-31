#ifndef BEARViewController_h
#define BEARViewController_h

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <opencv2/videoio/cap_ios.h>

@protocol BARDelegate <NSObject>

-(void) onDetectArUcoMarker:(int)markerId;
-(void) onUpdateExtrinsicMat;

@end

@interface BARBaseController : UIViewController <CvVideoCameraDelegate>

@property (nonatomic, weak) id<BARDelegate> delegate;

-(void) startDetect;

@end

#endif /* BEARViewController_h */
