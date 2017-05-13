//
//  MCLight.h
//  monkcGame
//
//  Created by SunYuLi on 16/2/24.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MCLight_h
#define MCLight_h

#include "monkc.h"
#include "MC3DNode.h"

class(MCLight, MC3DNode,
      struct MCLightStruct *next;
      MCBool    dataChanged;
      MCVector3 ambientLightStrength;
      MCVector3 diffuseLightStrength;
      MCVector3 specularLightStrength;
      MCVector3 lightColor;
      MCVector3 lightPosition;
);

method(MCLight, void, update, MCGLContext* ctx);
method(MCLight, void, printDebugInfo, voida);

#endif /* MCLight_h */
