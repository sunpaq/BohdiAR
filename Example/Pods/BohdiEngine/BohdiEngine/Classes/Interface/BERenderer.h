//
//  BERenderer.h
//  Pods
//
//  Created by YuliSun on 19/04/2017.
//
//

#import <Foundation/Foundation.h>
#import <CoreMotion/CoreMotion.h>
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

typedef enum {
    BECameraFixedAtOrigin = 0,
    BECameraRotateAroundModelManual,
    BECameraRotateAroundModelByGyroscope,
    BECameraRotateAroundModelByGyroscopeReverse
} BECameraRotateMode;

@interface BERenderer : NSObject

@property (atomic, readwrite) BOOL doesAutoRotateCamera;
@property (atomic, readwrite) BOOL doesDrawWireFrame;
@property (atomic, readwrite) CMRotationMatrix deviceRotateMat3;

+(GLKView*) createDefaultGLView:(CGRect)frame;
+(void) createFramebuffersWithContext:(EAGLContext*)ctx AndLayer:(CAEAGLLayer*)lyr;

-(instancetype) init __unavailable;
-(instancetype) initWithFrame:(CGRect)frame;

-(instancetype) setCameraRotateMode:(BECameraRotateMode)rmode;
-(instancetype) setBackgroundColor:(UIColor*)color;
-(instancetype) resizeAllScene:(CGSize)frameSize;
-(instancetype) scissorAllScene:(CGRect)frame;

-(void) removeCurrentModel;
-(void) addModelNamed:(NSString*)modelName;
-(void) addModelNamed:(NSString*)modelName Scale:(double)scale;
-(void) addModelNamed:(NSString*)modelName Scale:(double)scale RotateX:(double)ccwRadian;
-(void) addModelNamed:(NSString*)modelName Scale:(double)scale RotateX:(double)ccwRadian Tag:(int)tag;

//use default if names/name is null
-(void) addSkyboxNamed:(NSArray*)texnames;
-(void) addSkysphNamed:(NSString*)texname;
-(void) removeCurrentSkybox;
-(void) removeCurrentSkysph;

//need 4x4 column major matrix
-(void) cameraReset:(float*)mat4;
-(void) cameraReset:(float*)mat4 isRowMajor:(BOOL)rowm;

-(void) cameraRotate:(GLKMatrix3)mat3 Incremental:(BOOL)inc;
-(void) cameraTranslate:(GLKVector3)vec3 Incremental:(BOOL)inc;
-(void) cameraAspectRatioReset:(float)aspectRatio;
-(void) cameraFOVReset:(float)fov;

-(void) cameraTransformWorld:(GLKMatrix4)mat4;
-(void) cameraTransformSelf:(GLKMatrix4)mat4;

//light pos follow camera if pos is null
-(void) lightReset:(GLKVector3*)pos;

-(void) rotateModelByPanGesture:(CGPoint)offset;
-(void) rotateSkysphByPanGesture:(CGPoint)offset;
-(void) zoomModelByPinchGesture:(CGFloat)scale;

-(void) updateModelTag:(int)tag PoseMat4D:(double*)mat4;
-(void) updateModelTag:(int)tag PoseMat4F:(float*)mat4;

-(void) drawFrame;
-(void) drawFrameOnCALayer:(CALayer*)calayer;
-(void) drawFrameOnGLView:(GLKView*)glview;

-(void) pauseDraw:(BOOL)pause;

@end
