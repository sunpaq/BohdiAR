//
//  MCGLEngine.c
//  monkcGame
//
//  Created by SunYuLi on 15/12/26.
//  Copyright © 2015年 oreisoft. All rights reserved.
//

#include "MCGLEngine.h"
#include "BEAssetsManager.h"

//Global
utility(MCGLEngine, MCBool, isFeatureOn, MCGLFeature feature)
{
    return (MCBool)glIsEnabled(feature);
}

utility(MCGLEngine, void, featureSwith, MCGLFeature feature, MCBool onOrOff)
{
    MCBool isOn = (MCBool)glIsEnabled(feature);
    if (onOrOff) {
        if (!isOn) glEnable(feature);
    }else{
        if (isOn) glDisable(feature);
    }
}

utility(MCGLEngine, void, flushCommandAsync, voida)
{
    glFlush();
}

utility(MCGLEngine, void, flushCommandBlock, voida)
{
    glFinish();
}

utility(MCGLEngine, void, clearScreen, voida)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

utility(MCGLEngine, void, clearScreenWithColor, MCColorf color)
{
    glClearColor(color.R.f, color.G.f, color.B.f, color.A.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

utility(MCGLEngine, void, clearDepthBuffer, voida)
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

utility(MCGLEngine, void, clearStencilBuffer, voida)
{
    glClear(GL_STENCIL_BUFFER_BIT);
}

utility(MCGLEngine, void, setClearScreenColor, MCColorf color)
{
    glClearColor(color.R.f, color.G.f, color.B.f, color.A.f);
}

utility(MCGLEngine, void, setPointSize, double pointsize)
{
	//glPointSize is replaced by the gl_PointSize variable in the vertex shader.
	//glPointSize((GLfloat)pointsize);
}

utility(MCGLEngine, void, setLineWidth, double linewidth)
{
    glLineWidth((GLfloat)linewidth);
}

utility(MCGLEngine, void, setFrontCounterClockWise, MCBool isCCW)
{
    if (isCCW) {
        glFrontFace(GL_CCW);
    }else{
        glFrontFace(GL_CW);
    }
}

utility(MCGLEngine, void, cullFace, MCGLFace face)
{
    glCullFace(face);
}

utility(MCGLEngine, void, cullBackFace, voida)
{
    MCGLEngine_cullFace(MCGLBack);
}

//Texture
utility(MCGLEngine, MCUInt, getMaxTextureUnits, voida)
{
    return (MCUInt)GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS;
}

utility(MCGLEngine, void, activeTextureUnit, MCUInt index)
{
    static int texCache = -1;
    if (index != texCache) {
        glActiveTexture(GL_TEXTURE0 + (0x0001*index));
        texCache = index;
    }
}

utility(MCGLEngine, void, bindCubeTexture, MCUInt tid)
{
    static int texCache = -1;
    if (tid != texCache) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, tid);
        texCache = tid;
    }
}

utility(MCGLEngine, void, bind2DTexture, MCUInt tid)
{
    static int texCache = -1;
    if (tid != texCache) {
        glBindTexture(GL_TEXTURE_2D, tid);
        texCache = tid;
    }
}

utility(MCGLEngine, GLuint, createShader, voida)
{
    return glCreateProgram();
}

utility(MCGLEngine, GLuint, prepareShader, GLuint Id, const char* vcode, const char* fcode, const char* version)
{
    GLuint vertShader=0, fragShader=0;
    MCGLEngine_compileShader(&vertShader, GL_VERTEX_SHADER, vcode, version);
    glAttachShader(Id, vertShader);

    MCGLEngine_compileShader(&fragShader, GL_FRAGMENT_SHADER, fcode, version);
    glAttachShader(Id, fragShader);

    // Create shader program.
    //Id = glCreateProgram();
    
    // Attach vertex shader to program.
    
    // Attach fragment shader to program.
    
//    if (context != null) {
//        MCGLContext_beforeLinkProgram(0, context, Id);
//    }
    
    // Link program.
    if (MCGLEngine_linkProgram(Id) == 0) {
        error_log("Failed to link program: %d", Id);
        
        if (vertShader) {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader) {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (Id) {
            glDeleteProgram(Id);
            Id = 0;
        }
    }
    
//    if (context != null) {
//        MCGLContext_afterLinkProgram(0, context, Id);
//    }
    
    // Release vertex and fragment shaders.
    if (vertShader) {
        glDetachShader(Id, vertShader);
        glDeleteShader(vertShader);
    }
    if (fragShader) {
        glDetachShader(Id, fragShader);
        glDeleteShader(fragShader);
    }
    
    return Id;
}

utility(MCGLEngine, int, prepareShaderName, GLuint Id, const char* bundlename, const char* vname, const char* fname, const char* version)
{
    char vpath[PATH_MAX] = {0};
    if(MCFileGetPathFromBundle(bundlename, vname, vpath)) return -1;
    char* vcode = (char*)MCFileCopyContentWithPath(vpath);
    
    char fpath[PATH_MAX] = {0};
    if(MCFileGetPathFromBundle(bundlename, fname, fpath)) return -1;
    char* fcode = (char*)MCFileCopyContentWithPath(fpath);
    
    MCGLEngine_prepareShader(Id, vcode, fcode, version);
    if (vcode) {
        free((void*)vcode);
    }
    if (fcode) {
        free((void*)fcode);
    }
    return 0;
}

utility(MCGLEngine, void, tryUseShaderProgram, GLuint Id)
{
    GLint cid = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &cid);
    if (Id != 0 && Id != (GLuint)cid && glIsProgram(Id) == GL_TRUE) {
        glUseProgram(Id);
    }
}

utility(MCGLEngine, void, enableTransparency, MCBool enable)
{
    if (enable) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else {
        glDisable(GL_BLEND);
    }
}

utility(MCGLEngine, void, enablePolygonOffset, MCBool enable)
{
    if (enable) {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(0, -1.0);
    }
    else {
        glDisable(GL_POLYGON_OFFSET_FILL);
    }
}

//Frame Rate (FPS)
utility(MCGLEngine, int, tickFPS, MCClock* clock)
{
    static unsigned fcount = 0;
    static clock_t elapse = 0;
    static clock_t time, lastime;
    
    MCClock_getCPUClocksSinceStart(clock, &time);
    if (elapse >= CLOCKS_PER_SEC ) {
        unsigned result = fcount;
        //reset
        elapse = 0;
        fcount = 0;
        lastime = time;
        
        return result;
    }else{
        elapse += (time - lastime);
        fcount++;
        return -1;
    }
}

//Shader
utility(MCGLEngine, MCBool, compileShader, GLuint* shader, GLenum type, const GLchar *source, const GLchar *version)
{
    if (!source) {
        return false;
    }
    GLint status = 0;
    
    const char* sources[] = {version, source};
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 2, sources, NULL);
    glCompileShader(*shader);
    
    GLint logLength = 0;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        printf("Shader compile log:\n%s", log);
        free(log);
        
        //dump source
        //error_log(source);
    }
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(*shader);
        return false;
    }
    
    return true;
}

utility(MCGLEngine, int, linkProgram, GLuint prog)
{
    GLint status;
    glLinkProgram(prog);
    
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        printf("Program link log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0) {
        return 0;
    }
    
    return 1;
}

utility(MCGLEngine, int, validateProgram, GLuint prog)
{
    GLint logLength, status;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        printf("Program validate log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0) {
        return 0;
    }
    
    return 1;
}

utility(MCGLEngine, void, setViewport, int x, int y, int width, int height)
{
    glEnable(GL_DEPTH_TEST);//this is for Google cardboard
    glViewport(x, y, width, height);
}

utility(MCGLEngine, void, setScissor, int x, int y, int width, int height)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, width, height);
}

