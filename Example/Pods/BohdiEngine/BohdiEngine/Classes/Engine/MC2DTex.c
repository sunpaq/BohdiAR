//
//  MC2DTex.c
//  monkcGame
//
//  Created by Sun YuLi on 16/4/13.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MC2DTex.h"
#include "MCGLEngine.h"

oninit(MC2DTex)
{
    if (init(MCTexture)) {
        return obj;
    }else{
        return null;
    }
}

method(MC2DTex, MC2DTex*, initWithFileName, const char* name)
{
    return obj;
}

onload(MC2DTex)
{
    if (load(MCTexture)) {
        
        binding(MC2DTex, MC2DTex*, initWithFileName, const char* name);
        
        return cla;
    }else{
        return null;
    }
}
