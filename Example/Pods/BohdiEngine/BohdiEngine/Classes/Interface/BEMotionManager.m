//
//  BEMotionManager.m
//  Pods
//
//  Created by Sun YuLi on 2017/5/16.
//
//

#import "BEMotionManager.h"

@interface BEMotionManager()
{
    CMMotionManager* motionManager;
    CMAttitude* referenceAtt;
}
@end

@implementation BEMotionManager

@synthesize useDeltaRotationData;

static BEMotionManager* _instance = nil;

+(instancetype)shared
{
    if (_instance == nil) {
        _instance = [[BEMotionManager alloc] init];
        _instance.useDeltaRotationData = NO;
        _instance->referenceAtt = nil;
        _instance->motionManager = nil;
    }
    return _instance;
}

-(void)startDeviceMotion
{
    if (!motionManager) {
        motionManager = [[CMMotionManager alloc] init];
    }
    if (!motionManager.isDeviceMotionActive) {
        motionManager.deviceMotionUpdateInterval = 1.0/60.0;
        [motionManager startDeviceMotionUpdatesUsingReferenceFrame:CMAttitudeReferenceFrameXArbitraryZVertical];
    }
}

-(void)stopDeviceMotion
{
    [motionManager stopDeviceMotionUpdates];
}

-(void)resetAttitude
{
    CMDeviceMotion* m;
    if ((m=motionManager.deviceMotion)) {
        referenceAtt = m.attitude;
    }
    else {
        referenceAtt = nil;
    }
}

-(CMAttitude*)getDeltaAttitude
{
    if (!referenceAtt) {
        [self resetAttitude];
    }
    
    CMDeviceMotion* m;
    if ((m=motionManager.deviceMotion)) {
        if (m.attitude && referenceAtt) {
            if (self.useDeltaRotationData) {
                [m.attitude multiplyByInverseOfAttitude:referenceAtt];
            }
            return m.attitude;
        }
    }
    return nil;
}

@end
