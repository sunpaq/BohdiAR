//
//  MCDirector.c
//  monkcGame
//
//  Created by Sun YuLi on 16/3/19.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MCDirector.h"
#include "MCGLEngine.h"
#include "MCThread.h"
#include "MCCube.h"
#include "MC3DModelCache.h"

compute(MCLight*, lightHandler)
{
    as(MCDirector);
    if (var(lastScene) != null && var(lastScene)->light != null) {
        return var(lastScene)->light;
    }
    return null;
}

compute(MCCamera*, cameraHandler)
{
    as(MCDirector);
    if (var(lastScene) != null && var(lastScene)->mainCamera != null) {
        return var(lastScene)->mainCamera;
    }
    return null;
}

compute(MCGLContext*, contextHandler)
{
    as(MCDirector);
    return var(lastScene)->renderer->context;
}

oninit(MCDirector)
{
    if (init(MCObject)) {
        var(lastScene) = null;
        var(currentWidth) = 0;
        var(currentHeight) = 0;
        
        var(pause) = false;
        var(lightFollowCamera) = true;
        var(deviceRotationMat3) = MCMatrix3Identity;
        
        var(lightHandler) = lightHandler;
        var(cameraHandler) = cameraHandler;
        var(contextHandler) = contextHandler;
        
        //var(skybox) = null;
        //var(skysph) = null;
        
        var(skyboxThread) = new(MCThread);
        var(modelThread) = new(MCThread);
        return obj;
    }else{
        return null;
    }
}

function(void, releaseScenes, MC3DScene* scene)
{
    as(MCDirector);
    if (scene!= null) {
        if (scene->prev != null) {
            releaseScenes(obj, scene->prev);
        }else{
            release(scene);
        }
    }
}

method(MCDirector, void, bye, voida)
{
    obj->pause = true;
    if (obj->lastScene != null) {
        releaseScenes(obj, obj->lastScene);
        obj->lastScene = null;
    }
    //release(var(skybox));
    //release(var(skysph));
    release(var(skyboxThread));
    release(var(modelThread));

    superbye(MCObject);
}

method(MCDirector, void, updateAll, voida)
{
    if (obj && var(lastScene) != null && var(pause) == false) {
        if (cpt(cameraHandler)->rotateMode == MCCameraRotateAroundModelByGyroscope
            || cpt(cameraHandler)->rotateMode == MCCameraRotateAroundModelByGyroscopeReverse) {
            MC3DScene_setRotationMat3(var(lastScene), obj->deviceRotationMat3.m);
        }
        if (var(lightFollowCamera) && cpt(lightHandler) && cpt(cameraHandler)) {
            cpt(lightHandler)->lightPosition = computed(cpt(cameraHandler), currentPosition);
            cpt(lightHandler)->dataChanged = true;
        }
        MC3DScene_updateScene(var(lastScene), 0);
    }
}

method(MCDirector, int, drawAll, voida)
{
    int fps = -1;
    if (obj && var(lastScene) != null && var(pause) == false) {
        fps = MC3DScene_drawScene(var(lastScene), 0);
    }
    return fps;
}

method(MCDirector, void, setupMainScene, unsigned width, unsigned height)
{
    MC3DScene* scene = ff(new(MC3DScene), initWithWidthHeightDefaultShader, width, height);
    if (scene) {
        releaseScenes(obj, obj->lastScene);
        MCDirector_pushScene(obj, scene);
        release(scene);
        //test
        //MCDirector_addNode(obj, new(MCCube));
    }
}

method(MCDirector, void, setBackgroudColor, float R, float G, float B, float A)
{
    if (obj->lastScene) {
        obj->lastScene->bgcolor = (MCColorf){R,G,B,A};
    }
}

method(MCDirector, void, pushScene, MC3DScene* scene)
{
    if (var(lastScene) == null) {
        scene->next = null;
        scene->prev = null;
        var(lastScene) = scene;
        retain(scene);
    }else{
        scene->next = null;
        scene->prev = var(lastScene);
        var(lastScene)->next = scene;
        var(lastScene) = scene;
        retain(scene);
    }
}

method(MCDirector, void, popScene, voida)
{
    if (var(lastScene)) {
        ff(var(lastScene), lockCamera, true);
        MC3DScene* current = var(lastScene);
        //first scene
        if (current->prev == null) {
            var(lastScene) = null;
            release(current);
        }
        else {
            var(lastScene) = current->next;
            release(current);
        }
        ff(var(lastScene), lockCamera, false);
    }
}

method(MCDirector, void, resizeAllScene, int width, int height)
{
    if (var(currentWidth) == width && var(currentHeight) == height) {
        //no need to update
        return;
    }
    MC3DScene* iter = null;
    for (iter=var(lastScene); iter!=null; iter=iter->prev) {
        MC3DScene_resizeScene(iter, width, height);
    }
    var(currentWidth) = width;
    var(currentHeight) = height;
}

method(MCDirector, void, scissorAllScene, int x, int y, int width, int height)
{
    MCGLEngine_setViewport(x, y, width, height);
    MCGLEngine_setScissor(x, y, width, height);
    //call resize scene
    MCDirector_resizeAllScene(obj, width, height);
}

method(MCDirector, void, addNode, MC3DNode* node)
{
    if(node && obj->lastScene && obj->lastScene->rootnode) {
        MC3DNode_addChild(obj->lastScene->rootnode, (MC3DNode*)node);
    }else{
        error_log("MCDirector add node(%p) failed [lastScene=%p]\n",
                  node, obj->lastScene);
    }
}

method(MCDirector, void, addModel, MC3DModel* model, MCFloat maxsize)
{
    MCDirector_addModelAtIndex(obj, model, maxsize, -1);
}

method(MCDirector, void, addModelAtIndex, MC3DModel* model, MCFloat maxsize, int index)
{
    if(model && obj->lastScene && obj->lastScene->rootnode) {
        MC3DModel_resizeToFit(model, maxsize.f);
        MC3DNode_addChildAtIndex(obj->lastScene->rootnode, (MC3DNode*)model, index);
    }else{
        error_log("MCDirector add model(%p) failed [lastScene=%p]\n",
                  model, obj->lastScene);
    }
}

method(MCDirector, MC3DModel*, addModelNamed, const char* name, MCFloat maxsize)
{
    MC3DModel* model = MC3DModelCache_fetchModelNamed(MC3DModelCache_shared(0), name);
    if (model) {
        MCDirector_addModelAtIndex(obj, model, maxsize, -1);
        return model;
    }
    return MCDirector_addModelNamedAtIndex(obj, name, maxsize, -1);
}

method(MCDirector, MC3DModel*, addModelNamedAtIndex, const char* name, MCFloat maxsize, int index)
{
    MC3DModel* model = new(MC3DModel);
    MC3DModel_initWithFileName(model, name);
    MCDirector_addModelAtIndex(obj, model, maxsize, index);
    return model;
}

method(MCDirector, void, removeCurrentModel, voida)
{
    if (obj->lastScene) {
        obj->pause = true;
        MCLinkedList* list = obj->lastScene->rootnode->children;
        MCLinkedList_popItem(list, 0);
        obj->pause = false;
    }
}

method(MCDirector, void, addSkyboxNamed, const char* names[6])
{
    if (obj->lastScene) {
        MCSkybox* box = ff(new(MCSkybox), initWithFileNames, names);
        if (box) {
            MC3DScene_addSkybox(obj->lastScene, box);
            release(box);
        }
    }
}

method(MCDirector, void, addSkysphereNamed, const char* name)
{
    if (obj->lastScene) {
        MCSkysphere* sph = ff(new(MCSkysphere), initWithFileName, name);
        if (sph) {
            MC3DScene_addSkysph(obj->lastScene, sph);
            release(sph);
        }
    }
}

method(MCDirector, void, removeCurrentSkybox, voida)
{
    if (obj->lastScene) {
        MC3DScene_removeSkybox(obj->lastScene, 0);
    }
}

method(MCDirector, void, removeCurrentSkysph, voida)
{
    if (obj->lastScene) {
        MC3DScene_removeSkysph(obj->lastScene, 0);
    }
}

method(MCDirector, void, cameraFocusOn, MCVector4 vertex)
{
    MCCamera* c = computed(obj, cameraHandler);
    if (c != null) {
        c->lookat.x = vertex.x;
        c->lookat.y = vertex.y;
        c->lookat.z = vertex.z;
        c->R_value  = vertex.w;
        c->R_percent= 1.0;
    }
}

method(MCDirector, void, cameraFocusOnModel, MC3DModel* model)
{
    cpt(cameraHandler)->lookat  = computed(model, center);
}

method(MCDirector, void, cameraZoomToFitModel, MC3DModel* model)
{
    cpt(cameraHandler)->R_value = computed(model, maxlength) * 2.0f;
}

method(MCDirector, void, moveModelToOrigin, MC3DModel* model)
{
    MC3DModel_translateToOrigin(model, 0);
}

method(MCDirector, void, setDeviceRotationMat3, float mat3[9])
{
    if (mat3) {
        for (int i=0; i<9; i++) {
            obj->deviceRotationMat3.m[i] = mat3[i];
        }
    }
}

method(MCDirector, void, setCameraRotateMode, MCCameraRotateMode mode)
{
    if (cpt(cameraHandler)) {
        cpt(cameraHandler)->rotateMode = mode;
    }
}

method(MCDirector, void, printDebugInfo, voida)
{
    debug_log("MCDirector currentWidth=%d currentHeight=%d\n", obj->currentWidth, obj->currentHeight);
    MCCamera* cam = cpt(cameraHandler);
    if (cam) {
        ff(cam, printDebugInfo, 0);
    }
    if (obj->lastScene) {
        ff(obj->lastScene, printDebugInfo, 0);
    }
}

onload(MCDirector)
{
    if (load(MCObject)) {
        mixing(void, releaseScenes, MC3DScene* scene);
        
        binding(MCDirector, void, bye, voida);
        binding(MCDirector, void, updateAll, voida);
        binding(MCDirector, void, drawAll, voida);
        binding(MCDirector, void, setupMainScene, unsigned width, unsigned height);
        binding(MCDirector, void, setBackgroudColor, float R, float G, float B, float A);
        binding(MCDirector, void, pushScene, MC3DScene* scene);
        binding(MCDirector, void, popScene, voida);
        binding(MCDirector, void, resizeAllScene, int width, int height);
        binding(MCDirector, void, scissorAllScene, int x, int y, int width, int height);
        binding(MCDirector, void, addNode, MC3DNode* node);
        binding(MCDirector, void, addModel, MC3DModel* model, int maxsize);
        binding(MCDirector, void, addModelAtIndex, MC3DModel* model, MCFloat maxsize, int index);
        binding(MCDirector, MC3DModel*, addModelNamed, const char* name, MCFloat maxsize);
        binding(MCDirector, MC3DModel*, addModelNamedAtIndex, const char* name, MCFloat maxsize, int index);
        binding(MCDirector, void, removeCurrentModel, voida);
        binding(MCDirector, void, addSkyboxNamed, const char* names[6]);
        binding(MCDirector, void, addSkysphereNamed, const char* name);
        binding(MCDirector, void, removeCurrentSkybox, voida);
        binding(MCDirector, void, removeCurrentSkysph, voida);
        binding(MCDirector, void, cameraFocusOn, MCVector3 vertex);
        binding(MCDirector, void, cameraFocusOnModel, MC3DModel* model);
        binding(MCDirector, void, cameraZoomToFitModel, MC3DModel* model);
        binding(MCDirector, void, moveModelToOrigin, MC3DModel* model);
        binding(MCDirector, void, setDeviceRotationMat3, float mat3[9]);
        binding(MCDirector, void, setCameraRotateMode, MCCameraRotateMode mode);
        binding(MCDirector, void, printDebugInfo, voida);

        return cla;
    }else{
        return null;
    }
}
