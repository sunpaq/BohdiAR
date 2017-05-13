//
//  MCOrbit.h
//  monkcGame
//
//  Created by SunYuLi on 5/24/15.
//  Copyright (c) 2015 oreisoft. All rights reserved.
//

#ifndef __monkcGame__MCOrbit__
#define __monkcGame__MCOrbit__

#include "monkc.h"
#include "MCGLBase.h"
#include "MC3DNode.h"

class(MCOrbit, MC3DNode,
    double R;
    MCUInt segments;
    MCVector3 center;
    MCVector3 up;
    //vertex array
    MCUInt vertexArrayId;
    MCUInt vertexBufferId;
);

#endif /* defined(__monkcGame__MCOrbit__) */
