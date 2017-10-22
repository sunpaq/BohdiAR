//
//  MCGLContext.c
//  monkcGame
//
//  Created by SunYuLi on 16/2/23.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MCGLContext.h"
#include "MC3DBase.h"
#include "MCGLEngine.h"
#include "BEAssetsManager.h"
#include "MCGLRenderer.h"

oninit(MCGLContext)
{
    if (init(MCObject)) {
        var(pid) = glCreateProgram();
        var(uniformCount) = 0;
        var(drawMode) = MCDrawNone;
        return obj;
    }else{
        return null;
    }
}

function(int, fillUniformLocation, MCGLUniform* uniform)
{
    as(MCGLContext);
    if (uniform->location == MC3DErrUniformNotFound) {
        uniform->location = glGetUniformLocation(var(pid), uniform->name);
    }
    return uniform->location;
}

method(MCGLContext, void, bye, voida)
{
    glDeleteProgram(var(pid));
}

method(MCGLContext, MCGLContext*, initWithShaderCode, const char* vcode, const char* fcode,
       const char* attribs[], size_t acount, MCGLUniformType types[], const char* uniforms[], size_t ucount)
{
    MCGLEngine_tryUseShaderProgram(var(pid));
    
    //attribute
    for (int i=0; i<acount; i++) {
        glBindAttribLocation(obj->pid, i, attribs[i]);
    }
    
    MCGLEngine_prepareShader(obj->pid, vcode, fcode, "#version 300 es\n");

    //uniforms
    for (int i=0; i<ucount; i++) {
        MCGLUniform* f = &obj->uniforms[obj->uniformCount++];
        MCGLUniformSetName(f, uniforms[i]);
        f->type = types[i];
        f->location = glGetUniformLocation(var(pid), uniforms[i]);
        obj->uniformsDirty[i] = false;
    }
    
    return obj;
}

method(MCGLContext, MCGLContext*, initWithShaderName, const char* vname, const char* fname,
       const char* attribs[], size_t acount, MCGLUniformType types[], const char* uniforms[], size_t ucount)
{
    char vpath[LINE_MAX] = {0};
    if(MCFileGetPath(vname, vpath))
        return null;
    const char* vcode = MCFileCopyContentWithPath(vpath);
    
    char fpath[LINE_MAX] = {0};
    if(MCFileGetPath(fname, fpath))
        return null;
    const char* fcode = MCFileCopyContentWithPath(fpath);
    
    MCGLContext_initWithShaderCode(obj, vcode, fcode, attribs, acount, types, uniforms, ucount);
    
    free((void*)vcode);
    free((void*)fcode);
    return obj;
}

method(MCGLContext, void, activateShaderProgram, voida)
{
    MCGLEngine_tryUseShaderProgram(var(pid));
}

method(MCGLContext, int, getUniformLocation, const char* name)
{
    for (MCUInt i=0; i<obj->uniformCount; i++) {
        if (strcmp(name, obj->uniforms[i].name)==0) {
            return fillUniformLocation(obj, &obj->uniforms[i]);
        }
    }
    return MC3DErrUniformNotFound;
}

//MCGLUniformScalar,
//MCGLUniformVec2,
//MCGLUniformVec3,
//MCGLUniformVec4,
//MCGLUniformMat3,
//MCGLUniformMat4
function(int, setUniform, const char* name, int loc, MCGLUniform* uniform)
{
    as(MCGLContext);
    if (var(pid) == 0) {
        return -1;
    }
    if (name != null) {
        loc = glGetUniformLocation(var(pid), name);
    }
    if (loc != MC3DErrUniformNotFound) {
        switch (uniform->type) {
            case MCGLUniformScalar:
                glUniform1i(loc, uniform->data.scalar);
                break;
            case MCGLUniformVec1:
                glUniform1f(loc, uniform->data.vec1);
                break;
            case MCGLUniformVec2:
                glUniform2f(loc, uniform->data.vec2.x, uniform->data.vec2.y);
                break;
            case MCGLUniformVec3:
                glUniform3f(loc, uniform->data.vec3.x, uniform->data.vec3.y,
                            uniform->data.vec3.z);
                break;
            case MCGLUniformVec4:
                glUniform4f(loc, uniform->data.vec4.x, uniform->data.vec4.y,
                            uniform->data.vec4.z, uniform->data.vec4.w);
                break;
            case MCGLUniformMat3:
                glUniformMatrix3fv(loc, 1, 0, uniform->data.mat3.m);
                break;
            case MCGLUniformMat4:
                glUniformMatrix4fv(loc, 1, 0, uniform->data.mat4.m);
                break;
            default:
                break;
        }
    }
    return loc;
}

method(MCGLContext, void, updateUniform, const char* name, MCGLUniformData udata)
{
    MCGLUniform* u = null;
    int f = -1;
    for (int i=0; i<var(uniformCount); i++) {
        u = &var(uniforms)[i];
        if (strcmp(name, u->name) == 0) {
            f = i;
            break;
        }
    }
    
    if (u != null && f != -1 && !MCGLUniformDataEqual(u->type, &u->data, &udata)) {
        var(uniformsDirty)[f] = true;
        var(uniforms)[f].data = udata;
    }
    else if (f != -1){
        var(uniformsDirty)[f] = false;
    }
}

method(MCGLContext, void,  setUniforms, voida)
{
    for (int i=0; i<var(uniformCount); i++) {
        if (var(uniformsDirty)[i] == true) {
            MCGLUniform* f = &var(uniforms)[i];
            setUniform(obj, null, f->location, f);
            var(uniformsDirty)[i] = false;
        }
    }
    
}

method(MCGLContext, int, getUniformVector, const char* name, GLfloat* params)
{
    int loc = (int)ff(obj, getUniformLocation, name);
    glGetUniformfv(var(pid), loc, params);
    return loc;
}

method(MCGLContext, void, printUniforms, voida)
{
    MCLogTypeSet(MC_DEBUG);
    
    GLfloat buff[16];
    for (int i=0; i<var(uniformCount); i++) {
        MCGLUniform* f = &var(uniforms)[i];
        if (f) {
            ff(obj, getUniformVector, f->name, buff);
            if (f->type == MCGLUniformMat4) {
                //mat4
                debug_log("mat4:%s\n [%f/%f/%f/%f]\n [%f/%f/%f/%f]\n [%f/%f/%f/%f]\n [%f/%f/%f/%f]\n",
                          f->name,
                          buff[0],  buff[1],  buff[2],  buff[3],
                          buff[4],  buff[5],  buff[6],  buff[7],
                          buff[8],  buff[9],  buff[10], buff[11],
                          buff[12], buff[13], buff[14], buff[15]);
            }
            if (f->type == MCGLUniformMat3) {
                //mat3
                debug_log("mat3:%s\n [%f/%f/%f]\n [%f/%f/%f]\n [%f/%f/%f]\n",
                          f->name,
                          buff[0], buff[1], buff[2],
                          buff[3], buff[4], buff[5],
                          buff[6], buff[7], buff[8]);
            }
            if (f->type == MCGLUniformVec4) {
                //vec4
                debug_log("vec4:%s [%f/%f/%f/%f]\n", f->name, buff[0], buff[1], buff[2], buff[3]);
            }
            if (f->type == MCGLUniformVec3) {
                //vec3
                debug_log("vec3:%s [%f/%f/%f]\n", f->name, buff[0], buff[1], buff[2]);
            }
            if (f->type == MCGLUniformVec2) {
                //vec2
                debug_log("vec2:%s [%f/%f/%f]\n", f->name, buff[0], buff[1]);
            }
            if (f->type == MCGLUniformVec1) {
                //vec1
                debug_log("vec1:%s [%f]\n", f->name, buff[0]);
            }
            if (f->type == MCGLUniformScalar) {
                //scalar
                int value;
                ff(obj, getUniformVector, f->name, &value);
                debug_log("scalar:%s [%d]\n", f->name, value);
            }
        }
    }
}

onload(MCGLContext)
{
    if (load(MCObject)) {
        mixing(int, setUniform, const char* name, int loc, MCGLUniform* uniform);

        binding(MCGLContext, void, bye, voida);
        binding(MCGLContext, void, activateShaderProgram, voida);
        
        binding(MCGLContext, MCGLContext*, initWithShaderCode, const char* vcode, const char* fcode,
               const char* attribs[], size_t acount, MCGLUniformType types[], const char* uniforms[], size_t ucount);
        binding(MCGLContext, MCGLContext*, initWithShaderName, const char* vname, const char* fname,
                const char* attribs[], size_t acount, MCGLUniformType types[], const char* uniforms[], size_t ucount);
        
        binding(MCGLContext, void, updateUniform, const char* name, MCGLUniformData udata);
        binding(MCGLContext, void, setUniforms, voida);
        binding(MCGLContext, int,  getUniformVector,  const char* name, GLfloat* params);
        binding(MCGLContext, int,  getUniformLocation, const char* name);
        binding(MCGLContext, void, printUniforms, voida);

        return cla;
    }else{
        return null;
    }
}
