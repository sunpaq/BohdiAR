//
//  BECubeTextureData.h
//  Sapindus
//
//  Created by YuliSun on 24/02/2017.
//  Copyright © 2017 oreisoft. All rights reserved.
//

#ifndef BECubeTextureData_h
#define BECubeTextureData_h

#include "monkc.h"
#include "BE2DTextureData.h"

//usual face order: "right","left","top","bottom","back","front"
class(BECubeTextureData, MCObject,
      BE2DTextureData* faces[6]
);

utility(BECubeTextureData, BECubeTextureData*, newWithFacePaths, const char* facepaths[6]);
utility(BECubeTextureData, BECubeTextureData*, newWithFaces, const char* faces[6]);
method(BECubeTextureData, void, bye, voida);

//BEAssetsManager is an utility class
//utility(BEAssetsManager, BE2DTextureData*, load2DTexture, BE2DTextureData* texture);

#endif /* BECubeTextureData_h */
