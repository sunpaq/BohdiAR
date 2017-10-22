//
//  MCPanel.c
//  monkcGame
//
//  Created by SunYuLi on 16/2/24.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MCPanel.h"
#include "MCTexture.h"

const GLfloat U = 1.0f;

GLfloat vertices[] = {
    0.5f*U,  0.5f*U,  0.5f*U,         1.0f, 0.0f, 0.0f,        1.0f, 1.0f,
   -0.5f*U,  0.5f*U,  0.5f*U,         1.0f, 0.0f, 0.0f,        0.0f, 1.0f,
   -0.5f*U, -0.5f*U,  0.5f*U,         1.0f, 0.0f, 0.0f,        0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,         1.0f, 0.0f, 0.0f,        1.0f, 1.0f,
   -0.5f, -0.5f,  0.5f,         1.0f, 0.0f, 0.0f,        0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,         1.0f, 0.0f, 0.0f,        1.0f, 0.0f,
};

oninit(MCPanel)
{
    if (init(MC3DNode)) {
        
        MCMesh* mesh = new(MCMesh);
        mesh->vertexCount = 6;
        mesh->vertexAttribArray[0] = (MCVertexAttribute){0,  3, GL_FLOAT, GL_FALSE, 32, MCBUFFER_OFFSET(0)};
        mesh->vertexAttribArray[1] = (MCVertexAttribute){1,  3, GL_FLOAT, GL_FALSE, 32, MCBUFFER_OFFSET(12)};
        mesh->vertexAttribArray[2] = (MCVertexAttribute){2,  2, GL_FLOAT, GL_FALSE, 32, MCBUFFER_OFFSET(24)};
        
        mesh->vertexDataPtr = vertices;
        mesh->vertexDataSize = sizeof(vertices);
        
        MCLinkedList_addItem(svar(meshes), (MCItem*)mesh);
        svar(material) = new(MCMaterial);
        svar(diffuseTexture) = new(MCTexture);//will initialized outside
        
        return obj;
    }else{
        return null;
    }
}

onload(MCPanel)
{
    if (load(MC3DNode)) {
        return cla;
    }else{
        return null;
    }
}
