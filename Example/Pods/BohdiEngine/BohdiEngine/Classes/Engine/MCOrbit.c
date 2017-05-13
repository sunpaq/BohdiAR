//
//  MCOrbit.c
//  monkcGame
//
//  Created by SunYuLi on 5/24/15.
//  Copyright (c) 2015 oreisoft. All rights reserved.
//

#include "MCOrbit.h"
#include "MC3DShapeBase.h"

static double gCircleVertexData[360*3];

oninit(MCOrbit)
{
    if (init(MC3DNode)) {
        var(R) = 10.0;
        var(segments) = 360.0;
        var(center) = MCVector3Make(0, 0, 0);
        var(up) = MCVector3Make(0, 0, 1.0);
        
        MakeCircleData(0, 0, 0, var(R), var(segments), gCircleVertexData);
        
        //var(vertexArrayId) = MCGLAddVertexArray();
        //var(vertexBufferId) = MCGLAddVertexBufferNoNormal(gCircleVertexData, sizeof(gCircleVertexData));
        
        //test
        //var(vertexBufferId) = MCDrawLinePrepare(MCVertexMake(0, 0, 0), MCVertexMake(100, 100, 100));
        
        return obj;
    }else{
        return null;
    }
}

method(MCOrbit, void, bye, voida)
{
    //MCGLDelVertexArray(var(vertexArrayId));
    //MCGLDelVertexBuffer(var(vertexBufferId));
}

//override
method(MCOrbit, void, draw, voida)
{
    //MCGLDrawVertexArray(var(vertexBufferId), MCLines, 0, 360*3);
    
    //MCDrawLine(var(vertexBufferId));
    //MCGLDrawElements(MCLines, 360*3, GL_FLOAT, gCircleVertexData);
    //ff(obj, MCOrbit.makeCircleData, 0);
}

onload(MCOrbit)
{
    if (load(MC3DNode)) {
        binding(MCOrbit, void, bye);
        binding(MCOrbit, void, draw);
        return cla;
    }else{
        return null;
    }
}
