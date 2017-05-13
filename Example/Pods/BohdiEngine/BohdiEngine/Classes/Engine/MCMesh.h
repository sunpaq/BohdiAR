//
//  MCMesh.h
//  monkcGame
//
//  Created by Sun YuLi on 16/2/20.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MCMesh_h
#define MCMesh_h

#include "monkc.h"
#include "MCGLContext.h"
#include "MCLinkedList.h"
#include "MCMath.h"
#include "BAObjParser.h"

typedef union {
    struct {
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat nx;
        GLfloat ny;
        GLfloat nz;
        GLfloat r;
        GLfloat g;
        GLfloat b;
        GLfloat u;
        GLfloat v;
    };
    GLfloat data[11];
} MCMeshVertexData;

class(MCMesh, MCItem,
      MCBool     isDataLoaded;
      MCBool     calculatedNormal;
      
      MC3DFrame  Frame;
      GLenum     useage;
      
      GLuint     VAO;  //VAO
      GLuint     VBO;  //VBO
      GLuint     EBO;  //EBO
      
      MCBool     vertexDataNeedRelease;
      GLfloat*   vertexDataPtr;   //gCubeVertexData
      GLsizeiptr vertexDataSize;  //sizeof(gCubeVertexData)
      GLuint*    vertexIndexes;
      GLsizei    vertexCount;

      MCVertexAttribute vertexAttribArray[MCVertexAttribIndexMax];
      MCDrawMode mode;

      MCTexture* diffuseTextureRef;
      MCTexture* specularTextureRef;
);

method(MCMesh, void, bye, voida);
method(MCMesh, MCMesh*, initWithDefaultVertexAttributes, GLsizei vertexCount);
method(MCMesh, void, setVertex, GLuint offset, MCMeshVertexData* data);
method(MCMesh, void, normalizeNormals, voida);
method(MCMesh, void, prepareMesh, MCGLContext* ctx);
method(MCMesh, void, drawMesh, MCGLContext* ctx);
method(MCMesh, void, dump, voida);

#endif /* MCMesh_h */
