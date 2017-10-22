//
//  MC3DModelCache.c
//  Pods
//
//  Created by YuliSun on 01/06/2017.
//
//

#include "MC3DModelCache.h"

static MC3DModelCache* instance = null;

oninit(MC3DModelCache)
{
    if (init(MCObject)) {
        obj->map = new(MCMap);
        return obj;
    } else {
        return null;
    }
}

utility(MC3DModelCache, MC3DModelCache*, shared, voida)
{
    if (!instance) {
        instance = new(MC3DModelCache);
    }
    return instance;
}

method(MC3DModelCache, MC3DModel*, loadModelNamed, const char* name)
{
    //MC3DModel* model = MC3DModelCache_fetchModelNamed(obj, name);
    MCGeneric result;
    MCMap_getValueForKey(var(map), &result, name);
    
    if (result.mcobject) {
        return (MC3DModel*)result.mcobject;
    } else {
        MCObject* mobj = ff(new(MC3DModel), initWithFileName, name);
        MCMap_setValueForKey(obj->map, MCGenericO(mobj), name);
        return (MC3DModel*)mobj;
    }
}

method(MC3DModelCache, MC3DModel*, fetchModelNamed, const char* name)
{
    if (name) {
        MCGeneric result;
        MCMap_getValueForKey(obj->map, &result, name);
        if (result.mcobject) {
            MC3DModel* model = (MC3DModel*)result.mcobject;
            return model;
        }
    }
    return null;
}

onload(MC3DModelCache)
{
    if (load(MCObject)) {
        binding(MC3DModelCache, MC3DModel*, loadModelNamed, const char* name);
        binding(MC3DModelCache, MC3DModel*, fetchModelNamed, const char* name);
        return cla;
    } else {
        return null;
    }
}
