//
//  MCGLShader.h
//  monkcGame
//
//  Created by SunYuLi on 15/11/22.
//  Copyright © 2015年 oreisoft. All rights reserved.
//

/*
 Notice:
 
 You must set the window system on both platform
 as below: (RGBA=8888 Depth=24bit)
 
 //iOS
 <GLKViewController>
 view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
 view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
 
 //Android
 <GLSurfaceView>
 setEGLConfigChooser(8, 8, 8, 8, 24, 0);
 setEGLContextClientVersion(3);
 */

#ifndef MCGLRenderer_h
#define MCGLRenderer_h

#include "monkc.h"
#include "MCGLBase.h"
#include "MC3DNode.h"
#include "MCGLContext.h"
#include "MCMesh.h"
#include "MCMaterial.h"
#include "MCTexture.h"

MCGlobalKey view_view       = "view_view";
MCGlobalKey view_projection = "view_projection";
MCGlobalKey view_position   = "view_position";

MCGlobalKey model_model     = "model_model";
MCGlobalKey model_normal    = "model_normal";

MCGlobalKey light_ambient   = "light_ambient";
MCGlobalKey light_diffuse   = "light_diffuse";
MCGlobalKey light_specular  = "light_specular";
MCGlobalKey light_color     = "light_color";
MCGlobalKey light_position  = "light_position";

MCGlobalKey material_ambient   = "material_ambient";
MCGlobalKey material_diffuse   = "material_diffuse";
MCGlobalKey material_specular  = "material_specular";
MCGlobalKey material_dissolve  = "material_dissolve";
MCGlobalKey material_shininess = "material_shininess";

MCGlobalKey diffuse_sampler = "diffuse_sampler";
MCGlobalKey specular_sampler = "specular_sampler";

class(MCGLRenderer, MCObject,
      MCGLContext* context;
);
//life cycle
method(MCGLRenderer, void, bye, voida);
method(MCGLRenderer, MCGLRenderer*, initWithShaderCodeString, const char* vcode, const char* fcode);
method(MCGLRenderer, MCGLRenderer*, initWithShaderFileName, const char* vshader, const char* fshader);
method(MCGLRenderer, MCGLRenderer*, initWithDefaultShader, voida);
//draw
method(MCGLRenderer, void, updateNodes, MC3DNode* rootnode);
method(MCGLRenderer, void, drawNodes, MC3DNode* rootnode);

method(MCGLRenderer, void, drawMesh, MCMesh* mesh);
method(MCGLRenderer, void, drawMaterial, MCMaterial* material);
method(MCGLRenderer, void, drawTexture, MCTexture* texture);

#endif /* MCGLRenderer_h */
