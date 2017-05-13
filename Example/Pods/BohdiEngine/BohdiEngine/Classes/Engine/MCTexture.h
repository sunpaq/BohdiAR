//
//  MCTexture.h
//  monkcGame
//
//  Created by Sun YuLi on 16/2/20.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MCTexture_h
#define MCTexture_h

#include "monkc.h"
#include "MCGLBase.h"
#include "BE2DTextureData.h"

typedef enum {
    MCTextureRepeat      = GL_REPEAT,
    MCTextureClampToEdge = GL_CLAMP_TO_EDGE
} MCTextureDisplayMode;

class(MCTexture, MCObject,
      GLuint Id;
      int width;
      int height;
      unsigned textureUnit;
      BE2DTextureData* data;
      MCTextureDisplayMode displayMode;
);

method(MCTexture, MCTexture*, initWithFileNameMode, const char* name, MCTextureDisplayMode mode);
method(MCTexture, MCTexture*, initWithFileName, const char* name);
method(MCTexture, MCTexture*, initWith2DTexture, BE2DTextureData* tex);
method(MCTexture, void, loadToGLBuffer, voida);
method(MCTexture, void, active, GLuint pid, const char* uniformName);

#endif /* MCTexture_h */
