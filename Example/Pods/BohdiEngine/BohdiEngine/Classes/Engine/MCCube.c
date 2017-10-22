//
//  MCCube.c
//  monkcGame
//
//  Created by SunYuLi on 5/18/15.
//  Copyright (c) 2015 oreisoft. All rights reserved.
//

#include "MCCube.h"
#include "MC3DBase.h"

static float gCubeVertexData[11*6*6] = {
    // Data layout for each line below is:
    // positionX, positionY, positionZ,     normalX, normalY, normalZ,     colorR, colorG, colorB     texCoordX texCoordY
    0.5f, -0.5f, -0.5f,        1.0f, 0.0f, 0.0f,        0.5f, 0.0f, 0.0f,           0.0f, 0.0f,
    0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 0.0f,        0.5f, 0.0f, 0.0f,           1.0f, 0.0f,
    0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,        0.5f, 0.0f, 0.0f,           0.0f, 1.0f,
    0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,        0.5f, 0.0f, 0.0f,           0.0f, 1.0f,
    0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 0.0f,        0.5f, 0.0f, 0.0f,           1.0f, 0.0f,
    0.5f, 0.5f, 0.5f,          1.0f, 0.0f, 0.0f,        0.5f, 0.0f, 0.0f,           1.0f, 1.0f,
    
    0.5f, 0.5f, -0.5f,         0.0f, 1.0f, 0.0f,        0.0f, 0.5f, 0.0f,           1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,        0.0f, 0.5f, 0.0f,           0.0f, 0.0f,
    0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,        0.0f, 0.5f, 0.0f,           1.0f, 1.0f,
    0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,        0.0f, 0.5f, 0.0f,           1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,        0.0f, 0.5f, 0.0f,           0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,         0.0f, 1.0f, 0.0f,        0.0f, 0.5f, 0.0f,           0.0f, 1.0f,
    
    -0.5f, 0.5f, -0.5f,        -1.0f, 0.0f, 0.0f,       0.0f, 0.0f, 0.5f,           1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,       0.0f, 0.0f, 0.5f,           0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,       0.0f, 0.0f, 0.5f,           1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,       0.0f, 0.0f, 0.5f,           1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,       0.0f, 0.0f, 0.5f,           0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,        -1.0f, 0.0f, 0.0f,       0.0f, 0.0f, 0.5f,           0.0f, 1.0f,
    
    -0.5f, -0.5f, -0.5f,       0.0f, -1.0f, 0.0f,       0.0f, 0.5f, 1.0f,           0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,       0.0f, 0.5f, 1.0f,           1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,       0.0f, 0.5f, 1.0f,           0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,       0.0f, 0.5f, 1.0f,           0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,       0.0f, 0.5f, 1.0f,           1.0f, 0.0f,
    0.5f, -0.5f, 0.5f,         0.0f, -1.0f, 0.0f,       0.0f, 0.5f, 1.0f,           1.0f, 1.0f,

    0.5f, 0.5f, 0.5f,          0.0f, 0.0f, 1.0f,        1.0f, 0.0f, 0.5f,           1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,        1.0f, 0.0f, 0.5f,           0.0f, 1.0f,
    0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,        1.0f, 0.0f, 0.5f,           1.0f, 0.0f,
    0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,        1.0f, 0.0f, 0.5f,           1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,        1.0f, 0.0f, 0.5f,           0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,        1.0f, 0.0f, 0.5f,           0.0f, 0.0f,

    0.5f, -0.5f, -0.5f,        0.0f, 0.0f, -1.0f,       0.5f, 1.0f, 0.0f,           1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,       0.5f, 1.0f, 0.0f,           0.0f, 0.0f,
    0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,       0.5f, 1.0f, 0.0f,           1.0f, 1.0f,
    0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,       0.5f, 1.0f, 0.0f,           1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,       0.5f, 1.0f, 0.0f,           0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f,        0.0f, 0.0f, -1.0f,       0.5f, 1.0f, 0.0f,           0.0f, 1.0f,
};

oninit(MCCube)
{
    if (init(MC3DNode)) {
        
        MCMesh* mesh = new(MCMesh);
        mesh->vertexCount = 36;
        mesh->vertexAttribArray[0] = (MCVertexAttribute){MCVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 44, MCBUFFER_OFFSET(0)};
        mesh->vertexAttribArray[1] = (MCVertexAttribute){MCVertexAttribNormal,   3, GL_FLOAT, GL_FALSE, 44, MCBUFFER_OFFSET(12)};
        mesh->vertexAttribArray[2] = (MCVertexAttribute){MCVertexAttribColor,    3, GL_FLOAT, GL_FALSE, 44, MCBUFFER_OFFSET(24)};
        mesh->vertexAttribArray[3] = (MCVertexAttribute){MCVertexAttribTexCoord0,2, GL_FLOAT, GL_FALSE, 44, MCBUFFER_OFFSET(36)};
        
        mesh->vertexDataNeedRelease = false;
        mesh->vertexDataPtr = gCubeVertexData;
        mesh->vertexDataSize = sizeof(gCubeVertexData);
                
        MCLinkedList_addItem(svar(meshes), (MCItem*)mesh);
        sobj->material = new(MCMaterial);
        sobj->diffuseTexture = MCTexture_initWithFileName(new(MCTexture), "tex8.bmp");
        
        return obj;
    }else{
        return null;
    }
}

onload(MCCube)
{
    if (load(MC3DNode)) {
        return cla;
    }else{
        return null;
    }
}

