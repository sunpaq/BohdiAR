//
//  MCMesh.c
//  monkcGame
//
//  Created by Sun YuLi on 16/2/20.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MCMesh.h"
#include "MC3DBase.h"

oninit(MCMesh)
{
    if (init(MCItem)) {
        var(isDataLoaded) = false;
        var(calculatedNormal) = false;
        
        var(Frame) = (MC3DFrame){0,0,0,0,0,0};
        var(useage) = GL_STATIC_DRAW;
        var(mode) = MCTriAngles;

        var(vertexDataNeedRelease) = true;
        var(vertexDataPtr) = null;
        var(vertexDataSize)= 0;
        var(vertexIndexes) = null;
        var(vertexCount)   = 0;

        var(diffuseTextureRef) = null;
        var(specularTextureRef)= null;
        
        memset(var(vertexAttribArray), (int)null, sizeof(var(vertexAttribArray)));
        //debug_log("MCMesh - init finished\n");
        return obj;
    }else{
        return null;
    }
}

method(MCMesh, void, bye, voida)
{
    glDeleteBuffers(1, &obj->VBO);
    glDeleteVertexArrays(1, &obj->VAO);
    if (obj->vertexDataNeedRelease && obj->vertexDataPtr) {
        free(obj->vertexDataPtr);
    }
}

method(MCMesh, void, allocVertexBuffer, GLsizei vertexCount)
{
    obj->vertexCount = vertexCount ;
    obj->vertexDataSize = obj->vertexCount * 11 * sizeof(GLfloat);
    if (obj->vertexDataSize != 0) {
        obj->vertexDataPtr = (GLfloat*)malloc(obj->vertexDataSize);
        memset(obj->vertexDataPtr, 0, obj->vertexDataSize);
    }else{
        obj->vertexDataPtr = null;
    }
}

method(MCMesh, MCMesh*, initWithDefaultVertexAttributes, GLsizei vertexCount)
{
    //debug_log("MCMesh - initWithDefaultVertexAttributes\n");
    obj->vertexAttribArray[0] = (MCVertexAttribute){
        MCVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 44, MCBUFFER_OFFSET(0)};
    obj->vertexAttribArray[1] = (MCVertexAttribute){
        MCVertexAttribNormal,   3, GL_FLOAT, GL_FALSE, 44, MCBUFFER_OFFSET(12)};
    obj->vertexAttribArray[2] = (MCVertexAttribute){
        MCVertexAttribColor,    3, GL_FLOAT, GL_FALSE, 44, MCBUFFER_OFFSET(24)};
    obj->vertexAttribArray[3] = (MCVertexAttribute){
        MCVertexAttribTexCoord0,2, GL_FLOAT, GL_FALSE, 44, MCBUFFER_OFFSET(36)};
    
    //alloc vertex buffer
    MCMesh_allocVertexBuffer(obj, vertexCount);
    //obj->vertexIndexes = (GLuint*)malloc(sizeof(GLuint)*obj->vforertexCount);
    
    return obj;
}

method(MCMesh, void, setVertex, GLuint offset, MCMeshVertexData* data)
{
    obj->vertexDataPtr[offset+0] = data->x;
    obj->vertexDataPtr[offset+1] = data->y;
    obj->vertexDataPtr[offset+2] = data->z;
    
    if (obj->calculatedNormal) {
        obj->vertexDataPtr[offset+3] += data->nx;
        obj->vertexDataPtr[offset+4] += data->ny;
        obj->vertexDataPtr[offset+5] += data->nz;
    } else {
        obj->vertexDataPtr[offset+3] = data->nx;
        obj->vertexDataPtr[offset+4] = data->ny;
        obj->vertexDataPtr[offset+5] = data->nz;
    }
    
    obj->vertexDataPtr[offset+6] = data->r;
    obj->vertexDataPtr[offset+7] = data->g;
    obj->vertexDataPtr[offset+8] = data->b;
    
    obj->vertexDataPtr[offset+9]  = data->u;
    obj->vertexDataPtr[offset+10] = data->v;
}

method(MCMesh, void, normalizeNormals, voida)
{
    if (!obj->calculatedNormal) {
        return;
    }
    for (int i=0; i<obj->vertexCount; i++) {
        size_t offset = i * 11;
        GLfloat x = obj->vertexDataPtr[offset+3];
        GLfloat y = obj->vertexDataPtr[offset+4];
        GLfloat z = obj->vertexDataPtr[offset+5];
        
        MCVector3 n = MCVector3Normalize(MCVector3Make(x, y, z));
        obj->vertexDataPtr[offset+3] = n.x;
        obj->vertexDataPtr[offset+4] = n.y;
        obj->vertexDataPtr[offset+5] = n.z;
    }
}

method(MCMesh, void, prepareMesh, MCGLContext* ctx)
{
    if (var(isDataLoaded) == false) {
        glGenVertexArrays(1, &obj->VAO);
        glGenBuffers(1, &obj->VBO);
        //VAO
        glBindVertexArray(obj->VAO);
        //VBO
        glBindBuffer(GL_ARRAY_BUFFER, obj->VBO);
        glBufferData(GL_ARRAY_BUFFER, obj->vertexDataSize, obj->vertexDataPtr, obj->useage);
        //EBO
        if (var(vertexIndexes) != null) {
            glGenBuffers(1, &obj->EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*obj->vertexCount, obj->vertexIndexes, obj->useage);
        }
        //VAttributes
        int i;
        for (i=0; i<MCVertexAttribIndexMax-1; i++) {
            MCVertexAttribute attr = obj->vertexAttribArray[i];
            if (attr.vectorsize != (GLint)null) {
                MCVertexAttributeLoad(&obj->vertexAttribArray[i]);
            }
        }
        //Texture
        if (obj->diffuseTextureRef) {
            MCTexture_loadToGLBuffer(obj->diffuseTextureRef, 0);
        }
        if (obj->specularTextureRef) {
            MCTexture_loadToGLBuffer(obj->specularTextureRef, 0);
        }
        //Unbind
        glBindVertexArray(0);
        var(isDataLoaded) = true;
    }
}

method(MCMesh, void, drawMesh, MCGLContext* ctx)
{
    glBindVertexArray(obj->VAO);
    //texture
    if (obj->diffuseTextureRef) {
        MCTexture_active(obj->diffuseTextureRef, ctx->pid, "diffuse_sampler");
    }
    if (obj->specularTextureRef) {
        MCTexture_active(obj->specularTextureRef, ctx->pid, "specular_sampler");
    }
    //override draw mode
    GLenum mode = var(mode);
    if (ctx->drawMode != MCDrawNone) {
        mode = ctx->drawMode;
    }
    //draw
    if (mode != MCDrawNone) {
        if (var(vertexIndexes) != null) {
            glDrawElements(mode, 100, GL_UNSIGNED_INT, (GLvoid*)0);
        }else{
            glDrawArrays(mode, 0, var(vertexCount));
        }
    }
    //Unbind
    glBindVertexArray(0);
}

onload(MCMesh)
{
    if (load(MCItem)) {
        binding(MCMesh, void, bye, voida);
        binding(MCMesh, MCMesh*, initWithDefaultVertexAttributes, GLsizei vertexCount);
        binding(MCMesh, void, setVertex, GLuint offset, MCMeshVertexData* data);
        binding(MCMesh, void, normalizeNormals, voida);
        binding(MCMesh, void, prepareMesh, voida);
        binding(MCMesh, void, drawMesh, voida);
        return cla;
    }else{
        return null;
    }
}


