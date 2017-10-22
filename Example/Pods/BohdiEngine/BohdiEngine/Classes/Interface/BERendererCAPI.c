//
//  BEMainLoop.c
//  BEDemo
//
//  Created by YuliSun on 16/01/2017.
//  Copyright © 2017 SODEC. All rights reserved.
//

#import "BERendererCAPI.h"
#import "MCDirector.h"
#import "MCGLEngine.h"

static MCDirector* director = null;
static float pinch_scale = 10.0;

void BETeardownGL()
{
    if (director) {
        release(director);
        director = null;
    }
}

void BESetupGL(unsigned width, unsigned height)
{
    if (!director) {
        director = new(MCDirector);
    }
    
    ff(director, setupMainScene, width, height);
    director->lastScene->mainCamera->rotateMode = MCCameraRotateAroundModelByGyroscope;
}

void BEResizeGL(unsigned width, unsigned height)
{
    if (director) {
        ff(director, resizeAllScene, width, height);
    }
}

void BEUpdateGL()
{
    if (director) {
        ff(director, updateAll, 0);
    }
}

void BEDrawGL()
{
    if (director) {
        ff(director, drawAll, 0);
    }
}

void BEPanGesture(float x, float y)
{
    if (director) {
        computed(director, cameraHandler)->tht += (y/16.0);
        computed(director, cameraHandler)->fai += -(x/9.0);
    }
}

void BEPinchGesture(float scale)
{
    pinch_scale *= scale;
    pinch_scale = MAX(10.0, MIN(pinch_scale, 100.0));
    
    MCCamera* camera = computed(director, cameraHandler);
    if (director) {
        MCCamera_distanceScale(camera, MCFloatF(20.0/pinch_scale));
    }
}

void BEAddModelNamed(const char* fname)
{
    ff(director, addModelNamed, fname);
    ff(director, cameraFocusOn, director->lastScene->rootnode->children->headItem);
}

void BEAddSkyboxNames(const char* negx, const char* posx,
                      const char* negy, const char* posy,
                      const char* negz, const char* posz)
{
    if (negx) {
        const char* names[6] = {negx, posx, negy, posy, negz, posz};
        ff(director, addSkyboxNamed, names);
    } else {
        ff(director, addSkyboxNamed, null);
    }
}

void BEAddSkysphNamed(const char* name)
{
    if (name) {
        ff(director, addSkysphereNamed, name);
    } else {
        ff(director, addSkysphereNamed, null);
    }
}

void BERemoveCurrentModel()
{
    ff(director, removeCurrentModel, 0);
}

void BERemoveCurrentSkybox()
{
    MCDirector_removeCurrentSkybox(director, 0);
}

void BERemoveCurrentSkysph()
{
    MCDirector_removeCurrentSkysph(director, 0);
}

void BESetClearScreenColor(float R, float G, float B, float A)
{
    MCGLEngine_setClearScreenColor((MCColorf){R,G,B,A});
}

void BESetDeviceRotateMat3(float m00, float m01, float m02,
                           float m10, float m11, float m12,
                           float m20, float m21, float m22)
{
    director->deviceRotationMat3.m00 = m00;
    director->deviceRotationMat3.m01 = m01;
    director->deviceRotationMat3.m02 = m02;
    
    director->deviceRotationMat3.m10 = m10;
    director->deviceRotationMat3.m11 = m11;
    director->deviceRotationMat3.m12 = m12;
    
    director->deviceRotationMat3.m20 = m20;
    director->deviceRotationMat3.m21 = m21;
    director->deviceRotationMat3.m22 = m22;
}

void BESetDoesRotateCamera(_Bool doesRotate)
{
    computed(director, cameraHandler)->isLockRotation = doesRotate? false : true;
}

void BESetDoesDrawWireFrame(_Bool doesDrawWF)
{
    computed(director, contextHandler)->drawMode = doesDrawWF ? MCLineStrip : MCTriAngles;
}

void BESetLightFollowCamera(_Bool lightFollowCam)
{
    director->lightFollowCamera = lightFollowCam;
}

void BECameraReset(float m00, float m01, float m02, float m03,
                   float m10, float m11, float m12, float m13,
                   float m20, float m21, float m22, float m23,
                   float m30, float m31, float m32, float m33, _Bool identity)
{
    MCCamera* cam = computed(director, cameraHandler);
    if (cam) {
        if (!identity) {
            cam->Super.transform.m00 = m00;
            cam->Super.transform.m01 = m01;
            cam->Super.transform.m02 = m02;
            cam->Super.transform.m03 = m03;

            cam->Super.transform.m10 = m10;
            cam->Super.transform.m11 = m11;
            cam->Super.transform.m12 = m12;
            cam->Super.transform.m13 = m13;

            cam->Super.transform.m20 = m20;
            cam->Super.transform.m21 = m21;
            cam->Super.transform.m22 = m22;
            cam->Super.transform.m23 = m23;

            cam->Super.transform.m30 = m30;
            cam->Super.transform.m31 = m31;
            cam->Super.transform.m32 = m32;
            cam->Super.transform.m33 = m33;

        } else {
            cam->Super.transform = MCMatrix4Identity;
        }
    }
}

void BECameraRotate(float m00, float m01, float m02,
                    float m10, float m11, float m12,
                    float m20, float m21, float m22, _Bool incremental)
{
    MCCamera* cam = computed(director, cameraHandler);
    if (cam) {
        MCMatrix3 mat3 = {
            m00, m01, m02,
            m10, m11, m12,
            m20, m21, m22
        };
        MC3DNode_rotateMat3(&cam->Super, mat3.m, incremental?true:false);
    }
}

void BESkysphCameraReset(float m00, float m01, float m02, float m03,
                         float m10, float m11, float m12, float m13,
                         float m20, float m21, float m22, float m23,
                         float m30, float m31, float m32, float m33, _Bool identity)
{
    MC3DScene* scene = director->lastScene;
    if (scene) {
        float mat4[16] = {
            m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33
        };
        MC3DScene_setRotationMat4(scene, &mat4[0]);
    }
}

void BECameraTranslate(float x, float y, float z, _Bool incremental)
{
    MCCamera* cam = computed(director, cameraHandler);
    if (cam) {
        MCVector3 eye = MCVector3Make(x, y, z);
        cam->R_value = MCVector3Length(eye);
        cam->eye = eye;

        MC3DNode_translateVec3(&cam->Super, &eye, incremental?true:false);
    }
}

void BELightReset(float x, float y, float z)
{
    MCLight* light = computed(director, lightHandler);
    if (light) {
        light->lightPosition = MCVector3Make(x, y, z);
    }
}





