//
//  MCDirector.h
//  monkcGame
//
//  Created by Sun YuLi on 16/3/19.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MCDirector_h
#define MCDirector_h

#include "monkc.h"
#include "MC3DScene.h"
#include "MC3DModel.h"
#include "MCThread.h"
#include "MCLight.h"

#include "MCSkybox.h"
#include "MCSkysphere.h"

class(MCDirector, MCObject,
      MC3DScene* lastScene;
      
      MCThread* skyboxThread;
      MCThread* modelThread;
      
      int currentWidth;
      int currentHeight;
      
      MCBool lightFollowCamera;
      //MCBool gyroscopeOnOff;
      MCBool pause;
      
      MCMatrix3 deviceRotationMat3;
      
      //common skybox for many scenes
      //MCSkybox* skybox;
      //MCSkysphere* skysph;
      
      computing(MCLight*, lightHandler);
      computing(MCCamera*, cameraHandler);
      computing(MCGLContext*, contextHandler);
      //computing(MCSkyboxCamera*, skyboxCameraHandler);
);

method(MCDirector, void, bye, voida);
method(MCDirector, void, updateAll, voida);

//return -1 or positive fps number
//please update your view only when it is not -1
method(MCDirector, int, drawAll, voida);

method(MCDirector, void, setupMainScene, unsigned width, unsigned height);
method(MCDirector, void, setBackgroudColor, float R, float G, float B, float A);

method(MCDirector, void, pushScene, MC3DScene* scene);
method(MCDirector, void, popScene, voida);
method(MCDirector, void, resizeAllScene, int width, int height);
method(MCDirector, void, scissorAllScene, int x, int y, int width, int height);

method(MCDirector, void, addNode, MC3DNode* node);
method(MCDirector, void, addModel, MC3DModel* model, MCFloat maxsize);
method(MCDirector, void, addModelAtIndex, MC3DModel* model, MCFloat maxsize, int index);

method(MCDirector, MC3DModel*, addModelNamed, const char* name, MCFloat maxsize);
method(MCDirector, MC3DModel*, addModelNamedAtIndex, const char* name, MCFloat maxsize, int index);

method(MCDirector, void, removeCurrentModel, voida);
//use default if names/name is null
method(MCDirector, void, addSkyboxNamed, const char* names[6]);
method(MCDirector, void, addSkysphereNamed, const char* name);
method(MCDirector, void, removeCurrentSkybox, voida);
method(MCDirector, void, removeCurrentSkysph, voida);

method(MCDirector, void, cameraFocusOn, MCVector4 vertex);
method(MCDirector, void, cameraFocusOnModel, MC3DModel* model);
method(MCDirector, void, cameraZoomToFitModel, MC3DModel* model);
method(MCDirector, void, moveModelToOrigin, MC3DModel* model);

method(MCDirector, void, setDeviceRotationMat3, float mat3[9]);
method(MCDirector, void, setCameraRotateMode, MCCameraRotateMode mode);
method(MCDirector, void, printDebugInfo, voida);

//iOS callbacks
//void onRootViewLoad(void* rootview);
//void onOpenExternalFile(const char* filepath);
//void onReceiveMemoryWarning();
//void onSetupGL(unsigned windowWidth, unsigned windowHeight, const char** filePathArray);
//void onTearDownGL();
//void onUpdate();
//void onDraw();

#endif /* MCDirector_h */
