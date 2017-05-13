//
//  MC3DNode.h
//  monkcGame
//
//  Created by Sun YuLi on 16/2/20.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MC3DNode_h
#define MC3DNode_h

#include "monkc.h"
#include "MC3DBase.h"
#include "MCMesh.h"
#include "MCTexture.h"
#include "MCMaterial.h"
#include "MCTexture.h"
#include "MCGLContext.h"

class(MC3DNode, MCItem,
      MCUInt index;
      MCInt zorder;
      MCBool visible;
      MCVector3 center;
      
      MCMatrix4 transform;
      MCMatrix4 viewtrans;

      MCMaterial* material;
      MCTexture* diffuseTexture;
      MCTexture* specularTexture;
      MCLinkedList* meshes;
      MCLinkedList* children;
);

method(MC3DNode, void, bye, voida);
//0=success
method(MC3DNode, MC3DErrCode, addChild, MC3DNode* child);
method(MC3DNode, MC3DErrCode, addChildAtIndex, MC3DNode* child, int index);
method(MC3DNode, MC3DErrCode, removeChild, MC3DNode* child);

method(MC3DNode, void, copyChildrenFrom, MC3DNode* node);
method(MC3DNode, void, cleanUnvisibleChild, voida);
method(MC3DNode, int,  childCount, voida);
method(MC3DNode, void, setAllVisible, MCBool visible);

method(MC3DNode, void, changeMatrial, MCMaterial* material);
method(MC3DNode, void, changeTexture, MCTexture* texture);
//pass null use identity matrix
method(MC3DNode, void, resetTransform, MCMatrix4* transform);
method(MC3DNode, void, translateVec3, MCVector3* position, MCBool incremental);
method(MC3DNode, void, rotateMat3, float mat3[9], MCBool incremental);
method(MC3DNode, void, rotateMat4, float mat4[16], MCBool incremental);
method(MC3DNode, void, scaleVec3, MCVector3* factors, MCBool incremental);

//draw
method(MC3DNode, void, update, MCGLContext* ctx);
method(MC3DNode, void, draw, MCGLContext* ctx);
method(MC3DNode, void, hide, voida);
method(MC3DNode, void, show, voida);

#endif /* MC3DNode_h */
