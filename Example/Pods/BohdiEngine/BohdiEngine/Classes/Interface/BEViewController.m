//
//  GLView.m
//  BEDemo
//
//  Created by YuliSun on 02/03/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#import "BEViewController.h"

#import <Foundation/Foundation.h>
#import <BohdiEngine/MCDirector.h>
#import <BohdiEngine/MCGLEngine.h>

@interface BEViewController()
{
    MCDirector* director;
    float pinch_scale;
    
    CMMotionManager* motionManager;
    CMAttitude* referenceAtt;
    
    UITapGestureRecognizer* tap;
}
@end

@implementation BEViewController

@dynamic glView;
@dynamic glFrame;
@dynamic useTransparentBackground;
@dynamic deviceRotateMat3;
@synthesize doesRotateCamera;
@synthesize doesDrawWireFrame;

-(void)initialization
{
    tap = [[UITapGestureRecognizer alloc] init];
}

-(instancetype)init
{
    self = [super init];
    if (self) {
        [self initialization];
    }
    return self;
}

-(instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        [self initialization];
    }
    return self;
}

-(instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        [self initialization];
    }
    return self;
}

-(void)viewDidLoad
{
    [super viewDidLoad];
    [self glviewSetup:self.glFrame];
    
    _touchDelegate = nil;
    pinch_scale = 10.0;
    
    motionManager = nil;
    _indicator = nil;
    
    _useDeltaRotationData = NO;
    
    tap.delegate = self;
    [self.view addGestureRecognizer:tap];
}

-(void)dealloc
{
    release(director);
    director = null;
    tap = null;
}

-(BOOL)canBecomeFirstResponder
{
    return YES;
}

-(void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    [self becomeFirstResponder];
    
    director = new(MCDirector);
    unsigned width = self.glFrame.size.width;
    unsigned height = self.glFrame.size.height;
    ff(director, setupMainScene, width, height);
    ff(director, setBackgroudColor, 0, 0, 0, 0);
    [self setCameraRotateMode:BECameraRotateAR];
    
    MCGLEngine_setClearScreenColor((MCColorf){0.0,0.0,0.0,0.0});
}

-(void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    [self stopDeviceMotion];
    //[self removeCurrentSkysph];
    self.glView.delegate = null;
    release(director);
    director = null;
}

-(void)viewDidDisappear:(BOOL)animated
{
    [self stopDeviceMotion];
    [self resignFirstResponder];
    [super viewDidDisappear:animated];
    //[self removeCurrentSkysph];
    self.glView.delegate = null;
    release(director);
    director = null;
}

-(void)motionEnded:(UIEventSubtype)motion withEvent:(UIEvent *)event
{
    if (motion == UIEventSubtypeMotionShake) {
        [self onClose:nil];
    }
}

#pragma mark - Properties

-(GLKView *)glView
{
    return (GLKView*)self.view;
}

-(CGRect)glFrame
{
    return self.view.frame;
}

-(void) setUseTransparentBackground:(BOOL)useTransparentBackground
{
    if (useTransparentBackground) {
        self.view.opaque = NO;
        self.view.backgroundColor = [UIColor clearColor];
        self.modalPresentationStyle = UIModalPresentationOverCurrentContext;
        self.navigationController.modalPresentationStyle = UIModalPresentationOverCurrentContext;
    } else {
        self.view.opaque = YES;
    }
}

-(BOOL)useMultisampleAntiAlias
{
    return self.glView.drawableMultisample;
}

-(void)setUseMultisampleAntiAlias:(BOOL)useMultisampleAntiAlias
{
    if (self.glView) {
        if (useMultisampleAntiAlias) {
            self.glView.drawableMultisample = GLKViewDrawableMultisample4X;
        } else {
            self.glView.drawableMultisample = GLKViewDrawableMultisampleNone;
        }
    }
}

-(BECameraRotateMode)cameraRotateMode
{
    return (BECameraRotateMode)computed(director, cameraHandler)->rotateMode;
}

-(void) setCameraRotateMode:(BECameraRotateMode)cameraRotateMode
{
    if (cameraRotateMode == BECameraRotateAroundModelByGyroscope) {
        director->gyroscopeMode = true;
    } else {
        director->gyroscopeMode = false;
    }
    MCCamera* cam = computed(director, cameraHandler);
    if (cam) {
        cam->rotateMode = (MCCameraRotateMode)cameraRotateMode;
    }
}

-(BOOL)doesRotateCamera
{
    //assuming ObjC YES/NO equal to true/false
    return computed(director, cameraHandler)->isLockRotation;
}

-(void) setDoesRotateCamera:(BOOL)doesRotate
{
    if (director) {
        MCCamera* cam = computed(director, cameraHandler);
        if (cam) {
            cam->isLockRotation = doesRotate? false : true;
        }
    }
}

-(BOOL)doesDrawWireFrame
{
    return computed(director, contextHandler)->drawMode == MCLineStrip ? YES:NO;
}

-(void) setDoesDrawWireFrame:(BOOL)doesDrawWF
{
    if (director) {
        MCGLContext* ctx = computed(director, contextHandler);
        if (ctx) {
            ctx->drawMode = doesDrawWF ? MCLineStrip : MCTriAngles;
        }
    }
}

-(void) cameraReset:(float*)mat4
{
    if (!director) return;
    MCCamera* cam = computed(director, cameraHandler);
    if (cam) {
        if (mat4) {
            MCMatrix4Copy(mat4, &cam->Super.transform);
        } else {
            cam->Super.transform = MCMatrix4Identity;
        }
    }
}

-(void) cameraRotate:(GLKMatrix3)mat3 Incremental:(BOOL)inc
{
    if (!director) return;
    MCCamera* cam = computed(director, cameraHandler);
    if (cam) {
        MC3DNode_rotateMat3(0, &cam->Super, mat3.m, inc?true:false);
    }
}

-(void) cameraTranslate:(GLKVector3)vec3 Incremental:(BOOL)inc
{
    if (!director) return;
    MCCamera* cam = computed(director, cameraHandler);
    if (cam) {
        MCVector3 eye = MCVector3Make(vec3.x, vec3.y, vec3.z);
        cam->R_value = MCVector3Length(eye);
        cam->eye = eye;
        
        MCVector3 mcv3 = {vec3.x, vec3.y, vec3.z};
        MC3DNode_translateVec3(0, &cam->Super, &mcv3, inc?true:false);
    }
}

-(void) lightReset:(GLKVector3*)pos
{
    if (!director) return;
    if (pos) {
        MCLight* light = computed(director, lightHandler);
        if (light) {
            light->lightPosition = MCVector3Make(pos->x, pos->y, pos->z);
        }
    } else {
        director->lightFollowCamera = true;
    }
}

-(void) setDeviceRotateMat3:(CMRotationMatrix)mat3
{
    if (director) {
        director->deviceRotationMat3.m00 = mat3.m11;
        director->deviceRotationMat3.m01 = mat3.m12;
        director->deviceRotationMat3.m02 = mat3.m13;
        
        director->deviceRotationMat3.m10 = mat3.m21;
        director->deviceRotationMat3.m11 = mat3.m22;
        director->deviceRotationMat3.m12 = mat3.m23;
        
        director->deviceRotationMat3.m20 = mat3.m31;
        director->deviceRotationMat3.m21 = mat3.m32;
        director->deviceRotationMat3.m22 = mat3.m33;
    }
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

-(void)saveReferenceAtt
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
        [self saveReferenceAtt];
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

-(void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    CMAttitude* att;
    if ((att=[self getDeltaAttitude])) {
        self.deviceRotateMat3 = att.rotationMatrix;
    }
    
    MCDirector_updateAll(0, director, 0);
    MCDirector_drawAll(0, director, 0);
}

-(void) glviewSetup:(CGRect)frame
{
    [EAGLContext setCurrentContext:[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3]];
    self.glView.context = [EAGLContext currentContext];
    self.glView.context.multiThreaded = NO;
    self.glView.delegate = self;
    self.glView.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    self.glView.drawableDepthFormat = GLKViewDrawableDepthFormat16;
    self.glView.drawableStencilFormat = GLKViewDrawableStencilFormat8;
    self.preferredFramesPerSecond = 60;
}

-(void) glviewResize:(CGRect)frame
{
    self.view.frame = frame;
    unsigned width = frame.size.width;
    unsigned height = frame.size.height;
    ff(director, resizeAllScene, width, height);
}

-(void) startLoadingAnimation
{
    if (!_indicator) {
        _indicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
    }
    _indicator.center = self.view.center;
    [self.view addSubview:_indicator];
    [_indicator startAnimating];
}

-(void) stopLoadingAnimation
{
    dispatch_async(dispatch_get_main_queue(), ^{
        if (_indicator) {
            [_indicator stopAnimating];
            _indicator = nil;
        }
    });
}

-(void) removeCurrentModel
{
    ff(director, removeCurrentModel, 0);
}

-(void) addModelNamed:(NSString*)modelName
{
    //[self startLoadingAnimation];
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^{
        const char* name = [modelName cStringUsingEncoding:NSUTF8StringEncoding];
        ff(director, addModelNamed, name, MCFloatF(50.0));
        ff(director, cameraFocusOn, MCVector4Make(0, 0, 0, 50));
        //[self stopLoadingAnimation];
    });
}

-(void) addSkyboxNamed:(NSArray*)texnames
{
    if (texnames) {
        int i = 0;
        const char* names[6] = {0};
        for (NSString* str in texnames) {
            const char* name = [str cStringUsingEncoding:NSUTF8StringEncoding];
            names[i++] = name;
        }
        ff(director, addSkyboxNamed, names);
    } else {
        ff(director, addSkyboxNamed, null);
    }
}

-(void) addSkysphNamed:(NSString*)texname
{
    if (texname) {
        const char* name = [texname cStringUsingEncoding:NSUTF8StringEncoding];
        MCDirector_addSkysphereNamed(0, director, name);
    } else {
        MCDirector_addSkysphereNamed(0, director, null);
    }
}

-(void) removeCurrentSkybox
{
    MCDirector_removeCurrentSkybox(0, director, 0);
}

-(void) removeCurrentSkysph
{
    MCDirector_removeCurrentSkysph(0, director, 0);
}

-(void) handlePanGesture:(CGPoint)offset
{
    float x = offset.x;
    float y = offset.y;
    computed(director, cameraHandler)->tht += (y/16.0);
    computed(director, cameraHandler)->fai += -(x/9.0);
}

-(void) handlePinchGesture:(float)scale
{
    pinch_scale *= scale;
    pinch_scale = MAX(10.0, MIN(pinch_scale, 100.0));
    
    MCCamera* camera = computed(director, cameraHandler);
    MCCamera_distanceScale(0, camera, MCFloatF(20.0/pinch_scale));
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    if (self.touchDelegate) {
        [self.touchDelegate onBETouched:self];
    }

    return YES;
}

-(void)onClose:(id)sender
{
    [self dismissViewControllerAnimated:YES completion:^{
        
    }];
}

-(void)attachGLLayerTo:(CALayer*)superlayer
{
    [superlayer addSublayer:self.glView.layer];
}

-(void)detachGLLayer
{
    [self.glView.layer removeFromSuperlayer];
}

@end
