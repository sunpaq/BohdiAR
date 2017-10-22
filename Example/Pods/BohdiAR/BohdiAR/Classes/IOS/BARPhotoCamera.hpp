#import "BARAbstractCamera.hpp"

@class BARPhotoCamera;

@protocol BARPhotoCameraDelegate <NSObject>

- (void)photoCamera:(BARPhotoCamera*)photoCamera capturedImage:(UIImage *)image;
- (void)photoCameraCancel:(BARPhotoCamera*)photoCamera;

@end

@interface BARPhotoCamera : BARAbstractCamera
{
    AVCaptureStillImageOutput *stillImageOutput;
}

@property (nonatomic, weak) id<BARPhotoCameraDelegate> delegate;

- (void)takePicture;

@end
