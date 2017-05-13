//
//  BEMainLoop.h
//  BEDemo
//
//  Created by YuliSun on 16/01/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#ifndef BEMainLoop_h
#define BEMainLoop_h

typedef enum {
    BECameraFixedAtOrigin,
    BECameraRotateAroundModelManual,
    BECameraRotateAroundModelByGyroscope,
    BECameraRotateAR,
    BECameraRotateARWall
} BECameraRotateMode;

void BETeardownGL();
void BESetupGL(unsigned width, unsigned height);
void BEResizeGL(unsigned width, unsigned height);
void BEUpdateGL();
void BEDrawGL();

void BEPanGesture(float x, float y);
void BEPinchGesture(float scale);

void BEAddModelNamed(const char* fname);
void BEAddSkyboxNames(const char* negx, const char* posx,
                      const char* negy, const char* posy,
                      const char* negz, const char* posz);
void BEAddSkysphNamed(const char* name);

void BERemoveCurrentModel();
void BERemoveCurrentSkybox();
void BERemoveCurrentSkysph();

void BESetClearScreenColor(float R, float G, float B, float A);
void BESetDeviceRotateMat3(float m00, float m01, float m02,
                           float m10, float m11, float m12,
                           float m20, float m21, float m22);
void BESetDoesRotateCamera(_Bool doesRotate);
void BESetDoesDrawWireFrame(_Bool doesDrawWF);
void BESetLightFollowCamera(_Bool lightFollowCam);

void BECameraReset(float m00, float m01, float m02, float m03,
                   float m10, float m11, float m12, float m13,
                   float m20, float m21, float m22, float m23,
                   float m30, float m31, float m32, float m33, _Bool identity);

void BECameraRotate(float m00, float m01, float m02,
                    float m10, float m11, float m12,
                    float m20, float m21, float m22, _Bool incremental);

void BESkysphCameraReset(float m00, float m01, float m02, float m03,
                         float m10, float m11, float m12, float m13,
                         float m20, float m21, float m22, float m23,
                         float m30, float m31, float m32, float m33, _Bool identity);

void BECameraTranslate(float x, float y, float z, _Bool incremental);
void BELightReset(float x, float y, float z);

#endif /* BEMainLoop_h */
