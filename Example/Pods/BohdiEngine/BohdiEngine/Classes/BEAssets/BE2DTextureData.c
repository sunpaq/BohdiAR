//
//  BE2DTextureData.c
//  Sapindus
//
//  Created by YuliSun on 24/02/2017.
//  Copyright © 2017 oreisoft. All rights reserved.
//

#include "BEAssetsManager.h"
#include "BE2DTextureData.h"
#include "MCString.h"
#include "SOIL.h"

oninit(BE2DTextureData)
{
    if (init(MCObject)) {
        //const types
        obj->AUTO = 0;
        obj->L    = 1;
        obj->LA   = 2;
        obj->RGB  = 3;
        obj->RGBA = 4;
        
        //input
        //obj->type = obj->RGB;
        obj->path = "";
        
        //output
        obj->raw = null;
        return obj;
    }else{
        return null;
    }
}

utility(BE2DTextureData, BE2DTextureData*, newWithPathnameType, const char* path, unsigned type)
{
    BE2DTextureData* data = new(BE2DTextureData);
    
    char decodepath[PATH_MAX] = {0};
    MCString_percentDecode(path, decodepath);
    
    size_t psize = strlen(decodepath) * sizeof(char);
    data->path = strcpy(malloc(psize), decodepath);
    
    //    if (type >= data->AUTO && type<= data->RGBA ) {
    //        data->type = type;
    //    }else{
    //        data->type = data->RGB;
    //    }
    
    data->raw = SOIL_load_image(data->path, &data->width, &data->height, &data->channels, SOIL_LOAD_AUTO);
    if (!data->raw) {
        error_log("BE2DTextureData - load texture failed: %s\n", SOIL_last_result());
        release(data);
        return null;
    }
    return data;
}

utility(BE2DTextureData, BE2DTextureData*, newWithPathname, const char* path)
{
    return BE2DTextureData_newWithPathnameType(path, 3);//default RGB
}

utility(BE2DTextureData, BE2DTextureData*, newWithFilename, const char* file)
{
    char path[PATH_MAX] = {0};
    MCFileGetPath(file, path);
    if (path[0]) {
        return BE2DTextureData_newWithPathname(path);
    }
    return null;
}

method(BE2DTextureData, void, bye, voida)
{
    if (obj->path) {
        free(obj->path);
        obj->path = null;
    }
    if (obj->raw) {
        SOIL_free_image_data(obj->raw);
        obj->raw = null;
    }
    superbye(MCObject);
}

onload(BE2DTextureData)
{
    if (load(MCObject)) {
        binding(BE2DTextureData, void, bye, voida);
        return cla;
    }else{
        return null;
    }
}
