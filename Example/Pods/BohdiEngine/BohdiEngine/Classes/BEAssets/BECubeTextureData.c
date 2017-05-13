//
//  BECubeTextureData.c
//  Sapindus
//
//  Created by YuliSun on 24/02/2017.
//  Copyright © 2017 oreisoft. All rights reserved.
//

#include "BEAssetsManager.h"
#include "BECubeTextureData.h"

oninit(BECubeTextureData)
{
    if (init(MCObject)) {
        return obj;
    }else{
        return null;
    }
}

utility(BECubeTextureData, BECubeTextureData*, newWithFacePaths, const char* facepaths[6])
{
    BECubeTextureData* data = new(BECubeTextureData);
    for (int i=0; i<6; i++) {
        BE2DTextureData* aface = BE2DTextureData_newWithPathname(facepaths[i]);
        if (aface != null) {
            data->faces[i] = aface;
        }else{
            error_log("BECubeTextureData image %s data is null!\n", facepaths[i]);
            return null;
        }
    }
    return data;
}

utility(BECubeTextureData, BECubeTextureData*, newWithFaces, const char* faces[6])
{
    BECubeTextureData* data = new(BECubeTextureData);
    char pathbuff[PATH_MAX] = {0};
    for (int i=0; i<6; i++) {
        if(MCFileGetPath(faces[i], pathbuff)){
            return null;
        }
        pathbuff[PATH_MAX-1] = NUL;
        data->faces[i] = BE2DTextureData_newWithPathname(pathbuff);
    }
    return data;
}

method(BECubeTextureData, void, bye, voida)
{
    for (int i=0; i<6; i++) {
        BE2DTextureData* face = obj->faces[i];
        if (face != null) {
            release(face);
        }
    }
    superbye(MCObject);
}

onload(BECubeTextureData)
{
    if (load(MCObject)) {
        binding(BECubeTextureData, void, bye, voida);
        return cla;
    }else{
        return null;
    }
}
