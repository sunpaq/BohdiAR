//
//  MC3DScene.h
//  monkcGame
//
//  Created by Sun YuLi on 16/2/20.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MC3DScene_h
#define MC3DScene_h

#include "monkc.h"
#include "MC3DNode.h"
#include "MCGLRenderer.h"
#include "MCCamera.h"
#include "MCClock.h"
#include "MCLight.h"
#include "MCSkybox.h"
#include "MCSkysphere.h"

typedef enum {
    MC3DSceneModelOnly,
    MC3DSceneModelWithSkybox,
    MC3DSceneModelWithSkysph,
    MC3DSceneSkyboxOnly,
    MC3DSceneSkysphOnly
} MC3DSceneCombineMode;

class(MC3DScene, MCObject,
      struct MC3DSceneStruct* next;
      struct MC3DSceneStruct* prev;
      
      MCColorf      bgcolor;
      unsigned      scenewidth;
      unsigned      sceneheight;
      
      MCSkybox*     skybox;
      MCSkysphere*  skysph;
      
      MCGLRenderer* renderer;
      MC3DNode*     rootnode;
      MCCamera*     mainCamera;
      MCLight*      light;
      MCBool        cameraLock;
      MCClock*      clock;
      
      computing(MCBool, isDrawSky);
      MC3DSceneCombineMode combineMode;
);

method(MC3DScene, void, bye, voida);
method(MC3DScene, MC3DScene*, initWithWidthHeightVSourceFSource, unsigned width, unsigned height, const char* vsource, const char* fsource);
method(MC3DScene, MC3DScene*, initWithWidthHeightVNameFName, unsigned width, unsigned height, const char* vname, const char* fname);
method(MC3DScene, MC3DScene*, initWithWidthHeightVNameFNameInBundle, unsigned width, unsigned height, const char* bundleId, const char* vname, const char* fname);
method(MC3DScene, MC3DScene*, initWithWidthHeightDefaultShader, unsigned width, unsigned height);
method(MC3DScene, void, resizeScene, unsigned width, unsigned height);
method(MC3DScene, void, addSkybox, MCSkybox* box);
method(MC3DScene, void, addSkysph, MCSkysphere* sph);
method(MC3DScene, void, removeSkybox, voida);
method(MC3DScene, void, removeSkysph, voida);
method(MC3DScene, void, updateScene, voida);
//return -1 or positive fps number
//please update your view only when it is not -1
method(MC3DScene, int, drawScene, voida);

method(MC3DScene, void, lockCamera, MCBool lock);
method(MC3DScene, MCCamera*, getCamera, voida);
method(MC3DScene, void, moveCameraOneStep, MCFloat deltaFai, MCFloat deltaTht);
method(MC3DScene, void, moveSkyboxCamera, MCFloat deltaFai, MCFloat deltaTht);
method(MC3DScene, void, setRotationMat3, float mat3[9]);
method(MC3DScene, void, setRotationMat4, float mat4[16]);
method(MC3DScene, void, printDebugInfo, voida);

#endif /* MC3DScene_h */
