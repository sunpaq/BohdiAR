//
//  MCLight.c
//  monkcGame
//
//  Created by SunYuLi on 16/2/24.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MCLight.h"
#include "MCGLEngine.h"
#include "MCGLRenderer.h"

oninit(MCLight)
{
    if (init(MC3DNode)) {
        obj->next = null;
        obj->ambientLightStrength  = MCVector3Make(0.2, 0.2, 0.2);
        obj->diffuseLightStrength  = MCVector3Make(1.0, 1.0, 1.0);
        obj->specularLightStrength = MCVector3Make(0.1, 0.1, 0.1);

        obj->lightColor    = MCVector3Make(1.0, 1.0, 1.0);
        obj->lightPosition = MCVector3Make(0, 0, -1000);

        obj->dataChanged = true;
        return obj;
    }else{
        return null;
    }
}

method(MCLight, void, printDebugInfo, voida)
{
    debug_log("MCLight: lightColor=%.2f/%.2f/%.2f lightPosition=%.2f/%.2f/%.2f\n",
              obj->lightColor.x, obj->lightColor.y, obj->lightColor.z,
              obj->lightPosition.x, obj->lightPosition.y, obj->lightPosition.z);
}

method(MCLight, void, update, MCGLContext* ctx)
{
    if (obj->dataChanged == true) {
        MCGLContext_activateShaderProgram(ctx, 0);
        
        MCGLUniformData data;
        
        data.vec3 = obj->ambientLightStrength;
        MCGLContext_updateUniform(ctx, light_ambient, data);
        
        data.vec3 = obj->diffuseLightStrength;
        MCGLContext_updateUniform(ctx, light_diffuse, data);
        
        data.vec3 = obj->specularLightStrength;
        MCGLContext_updateUniform(ctx, light_specular, data);
        
        data.vec3 = obj->lightColor;
        MCGLContext_updateUniform(ctx, light_color, data);
        
        data.vec3 = obj->lightPosition;
        MCGLContext_updateUniform(ctx, light_position, data);
        
        obj->dataChanged = false;
    }
}

onload(MCLight)
{
    if (load(MC3DNode)) {
        binding(MCLight, void, update, MCGLContext* ctx);
        binding(MCLight, void, printDebugInfo, voida);
        return cla;
    }else{
        return null;
    }
}

