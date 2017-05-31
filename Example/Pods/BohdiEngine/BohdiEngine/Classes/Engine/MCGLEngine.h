//
//  MCGLEngine.h
//  monkcGame
//
//  Created by SunYuLi on 15/12/26.
//  Copyright © 2015年 oreisoft. All rights reserved.
//

#ifndef MCGLEngine_h
#define MCGLEngine_h

#include "monkc.h"
#include "MCGLBase.h"
#include "MC3DBase.h"
#include "MC3DShapeBase.h"
#include "MCClock.h"
#include "MCGLContext.h"

typedef enum {
    MCGLErrNone = GL_NO_ERROR,
    MCGLErrInvalidEnum = GL_INVALID_ENUM,
    MCGLErrInvalidValue = GL_INVALID_VALUE,
    MCGLErrInvalidOperation = GL_INVALID_OPERATION,
    MCGLErrOutOfMemory = GL_OUT_OF_MEMORY,
} MCGLEngineErr;

typedef struct {
    MCBool success;
    MCUInt vaoId;
    MCUInt vboId;
} MCGLEngineResponse;

//Global
utility(MCGLEngine, MCBool, isFeatureOn, MCGLFeature feature);
utility(MCGLEngine, void, featureSwith, MCGLFeature feature, MCBool onOrOff);
utility(MCGLEngine, void, flushCommandAsync, voida);
utility(MCGLEngine, void, flushCommandBlock, voida);
utility(MCGLEngine, void, clearScreen, voida);
utility(MCGLEngine, void, clearScreenWithColor, MCColorf color);
utility(MCGLEngine, void, clearDepthBuffer, voida);
utility(MCGLEngine, void, clearStencilBuffer, voida);
utility(MCGLEngine, void, setClearScreenColor, MCColorf color);
utility(MCGLEngine, void, setPointSize, double pointsize);
utility(MCGLEngine, void, setLineWidth, double linewidth);
utility(MCGLEngine, void, setFrontCounterClockWise, MCBool isCCW);
utility(MCGLEngine, void, cullFace, MCGLFace face);
utility(MCGLEngine, void, cullBackFace, voida);
//Texture
utility(MCGLEngine, MCUInt, getMaxTextureUnits, voida);
utility(MCGLEngine, void, activeTextureUnit, MCUInt index);
utility(MCGLEngine, void, bindCubeTexture, MCUInt tid);
utility(MCGLEngine, void, bind2DTexture, MCUInt tid);
//Shader
utility(MCGLEngine, GLuint, createShader, voida);
utility(MCGLEngine, GLuint, prepareShader, GLuint Id, const char* vcode, const char* fcode, const char* version);
//pass bundlename = null to get main bundle
utility(MCGLEngine, int, prepareShaderName, GLuint Id, const char* bundlename, const char* vname, const char* fname, const char* version);
utility(MCGLEngine, void, tryUseShaderProgram, GLuint Id);
//Alpha Blend
utility(MCGLEngine, void, enableTransparency, MCBool enable);
//Z-Fighting
utility(MCGLEngine, void, enablePolygonOffset, MCBool enable);

//Frame Rate (FPS)
utility(MCGLEngine, int, tickFPS, MCClock* clock);

//Shader
utility(MCGLEngine, MCBool, compileShader, GLuint* shader, GLenum type, const GLchar *source, const GLchar *version);
utility(MCGLEngine, int, linkProgram, GLuint prog);
utility(MCGLEngine, int, validateProgram, GLuint prog);

//Viewport
utility(MCGLEngine, void, setViewport, int x, int y, int width, int height);
utility(MCGLEngine, void, setScissor, int x, int y, int width, int height);

#endif /* MCGLEngine_h */
