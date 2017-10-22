//
//  BEMotionManager.h
//  Pods
//
//  Created by Sun YuLi on 2017/5/16.
//
//

#import <Foundation/Foundation.h>
#import <CoreMotion/CoreMotion.h>

@interface BEMotionManager : NSObject

@property (atomic, readwrite) BOOL useDeltaRotationData;

+(instancetype)shared;

-(void)startDeviceMotion;

-(void)stopDeviceMotion;

-(void)resetAttitude;

-(CMAttitude*)getDeltaAttitude;

@end
