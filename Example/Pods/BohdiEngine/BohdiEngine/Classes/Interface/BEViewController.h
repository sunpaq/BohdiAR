//
//  GLView.h
//  BEDemo
//
//  Created by YuliSun on 02/03/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#ifndef BEViewController_h
#define BEViewController_h

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <CoreMotion/CoreMotion.h>
#import "BEInterface.h"

@protocol BEViewDelegate
-(void)onBETouched:(id)sender;
@end

@interface BEViewController : GLKViewController <GLKViewDelegate, UIGestureRecognizerDelegate>

@property (atomic, readwrite) id<BEViewDelegate> touchDelegate;

@property (atomic, readonly) GLKView* glView;
@property (atomic, readonly) CGRect glFrame;

@property (atomic, readwrite) BOOL useTransparentBackground;
@property (atomic, readwrite) BOOL useMultisampleAntiAlias;
@property (atomic, readwrite) BOOL useDeltaRotationData;
@property (atomic, readwrite) BOOL doesRotateCamera;
@property (atomic, readwrite) BOOL doesDrawWireFrame;

@property (atomic, readwrite) CMRotationMatrix deviceRotateMat3;
@property (atomic, readwrite) BECameraRotateMode cameraRotateMode;

@property (nonatomic, strong) UIActivityIndicatorView* indicator;

-(instancetype)init;
-(instancetype)initWithCoder:(NSCoder *)aDecoder;
-(instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil;

-(void) glviewResize:(CGRect)frame;

-(void) removeCurrentModel;
-(void) addModelNamed:(NSString*)modelName;

//use default if names/name is null
-(void) addSkyboxNamed:(NSArray*)texnames;
-(void) addSkysphNamed:(NSString*)texname;
-(void) removeCurrentSkybox;
-(void) removeCurrentSkysph;

//need 4x4 column major matrix
-(void) cameraReset:(float*)mat4;
-(void) cameraRotate:(GLKMatrix3)mat3 Incremental:(BOOL)inc;
-(void) cameraTranslate:(GLKVector3)vec3 Incremental:(BOOL)inc;

//light pos follow camera if pos is null
-(void) lightReset:(GLKVector3*)pos;

-(void) handlePanGesture:(CGPoint)offset;
-(void) handlePinchGesture:(float)scale;

//motion
-(void)startDeviceMotion;
-(void)stopDeviceMotion;

//Add 'layer' to the end of the receiver's sublayers array. If 'layer'
//already has a superlayer, it will be removed before being added.
-(void)attachGLLayerTo:(CALayer*)superlayer;
-(void)detachGLLayer;
@end


#endif /* BEViewController_h */
