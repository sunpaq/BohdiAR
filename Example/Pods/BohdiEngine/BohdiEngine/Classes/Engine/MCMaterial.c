//
//  MCMatrial.c
//  monkcGame
//
//  Created by SunYuLi on 16/2/24.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MCMaterial.h"
#include "MC3DBase.h"
#include "MCGLRenderer.h"

oninit(MCMaterial)
{
    if (init(MCObject)) {
        obj->ambientLightColor  = MCVector3Make(1.0, 1.0, 1.0);
        obj->diffuseLightColor  = MCVector3Make(1.0, 1.0, 1.0);
        obj->specularLightColor = MCVector3Make(1.0, 1.0, 1.0);
        obj->specularLightPower = 32.0;
        obj->dissolve = 1.0;
        obj->hidden   = 0;
        obj->illum    = 0;
        obj->dataChanged = true;
        
        obj->tag[0] = NUL;
        return obj;
    }else{
        return null;
    }
}

method(MCMaterial, void, prepareMatrial, MCGLContext* ctx)
{
    //set up once part
    if (obj->dataChanged == true) {
        MCGLContext_activateShaderProgram(ctx, 0);
        
        MCGLUniform f;
        f.type = MCGLUniformVec3;
        f.data.vec3 = obj->ambientLightColor;
        MCGLContext_updateUniform(ctx, material_ambient, f.data);

        f.data.vec3 = obj->diffuseLightColor;
        f.type = MCGLUniformVec3;
        MCGLContext_updateUniform(ctx, material_diffuse, f.data);
    
        f.type = MCGLUniformVec3;
        f.data.vec3 = obj->specularLightColor;
        MCGLContext_updateUniform(ctx, material_specular, f.data);
        
        f.data.vec1 = obj->specularLightPower;
        f.type = MCGLUniformVec1;
        MCGLContext_updateUniform(ctx, material_shininess, f.data);
    
        f.data.vec1 = obj->dissolve;
        f.type = MCGLUniformVec1;
        MCGLContext_updateUniform(ctx, material_dissolve, f.data);

        obj->dataChanged = false;
    }
    //set each time
    glUniform1i(glGetUniformLocation(ctx->pid, "illum"), obj->illum);

}

onload(MCMaterial)
{
    if (load(MCObject)) {
        binding(MCMaterial, void, prepareMatrial, MCGLContext* ctx);
        return cla;
    }else{
        return null;
    }
}
