//
//  MC3DModelCache.h
//  Pods
//
//  Created by YuliSun on 01/06/2017.
//
//

#ifndef MC3DModelCache_h
#define MC3DModelCache_h

#include "monkc.h"
#include "MCMap.h"
#include "MC3DModel.h"

class(MC3DModelCache, MCObject,
      MCMap* map);

utility(MC3DModelCache, MC3DModelCache*, shared, voida);

method(MC3DModelCache, MC3DModel*, loadModelNamed, const char* name);
method(MC3DModelCache, MC3DModel*, fetchModelNamed, const char* name);

#endif /* MC3DModelCache_h */
