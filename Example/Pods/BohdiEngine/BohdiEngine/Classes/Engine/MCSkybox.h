//
//  MCSkybox.h
//  monkcGame
//
//  Created by Sun YuLi on 16/4/13.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MCSkybox_h
#define MCSkybox_h

#include "monkc.h"
#include "MC3DNode.h"
#include "BECubeTextureData.h"

class(MCSkybox, MC3DNode,
      MCGLContext* ctx;
      
      MCMatrix4 boxViewMatrix;
      MCMatrix4 boxProjectionMatrix;
      double boxCameraRatio;
      double boxCameraAngle;
      
      MCUInt vaoid;
      MCUInt vboid;
      MCUInt eboid;
      MCUInt texid);

method(MCSkybox, void, bye, voida);
method(MCSkybox, MCSkybox*, initWithCubeTexture, BECubeTextureData* cubetex);
method(MCSkybox, MCSkybox*, initWithFileNames, const char* namelist[]);
method(MCSkybox, MCSkybox*, initWithDefaultFiles, voida);
//override
method(MCSkybox, void, setRotationMat3, float mat3[9]);
method(MCSkybox, void, setRotationMat4, float mat4[16]);
method(MCSkybox, void, update, MCGLContext* ctx);
method(MCSkybox, void, draw, MCGLContext* ctx);

#endif /* MCSkybox_h */
