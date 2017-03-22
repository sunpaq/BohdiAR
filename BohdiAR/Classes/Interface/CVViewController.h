#ifdef CVLOCK

#ifndef CVBridge_h
#define CVBridge_h

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <opencv2/videoio/cap_ios.h>

#import <BohdiEngine/BEViewController.h>

@interface CVViewController : UIViewController <CvVideoCameraDelegate>
@property (nonatomic, strong) CvVideoCamera* videoSource;
@property (nonatomic, strong) UIView* cvView;
@property (nonatomic, strong) BEViewController* beViewCtl;

@end

#endif /* CVBridge_h */

#endif
