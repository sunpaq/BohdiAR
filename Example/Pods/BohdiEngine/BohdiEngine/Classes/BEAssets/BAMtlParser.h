//
//  MC3DMtlParser.h
//  monkcGame
//
//  Created by Sun YuLi on 16/4/5.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MC3DMtlParser_h
#define MC3DMtlParser_h

#include "monkc.h"
#include "MCMath.h"
#include "MCGeometry.h"
#include "MCString.h"
#include "MCLexer.h"

/* illum_0 -> illum_10
0   Color on and Ambient off 
1   Color on and Ambient on 
2   Highlight on 
3   Reflection on and Ray trace on 
4   Transparency: Glass on Reflection: Ray trace on
5   Reflection: Fresnel on and Ray trace on 
6   Transparency: Refraction on Reflection: Fresnel off and Ray trace on
7   Transparency: Refraction on Reflection: Fresnel on and Ray trace on
8   Reflection on and Ray trace off 
9   Transparency: Glass on Reflection: Ray trace off
10  Casts shadows onto invisible surfaces
*/

typedef enum {
    Ambient = 0,
    Diffuse,
    Specular,
    Emissive,
    //transmission filter
    TFilter
} BALightType;

typedef enum {
    RGB,         //Ka r g b
    XYZ,         //Ka x y z (CIEXYZ color space)
    SpectralFile //Ka spectral file.rfl factor
} BAColorType;

typedef enum {
    SpecularExponent, //Ns
    DissolveFactor,   //d
    Disp,             //disp (surface roughness)
    Decal,            //decal
    Bump              //bump
} BAScalarType;

typedef struct {
    BAColorType Ctype;
    double factor;//default=1.0
    union {
        double rgbxyz[3];
        char spectral[256];
    } data;
} BALightColor;

/* texture map options
-blendu on | off 
-blendv on | off 
-cc on | off 
-clamp on | off 
-mm base gain 
-o u v w 
-s u v w 
-t u v w 
-texres value
*/

//map_Ka, map_Kd, map_Ks
typedef struct {
    BALightType Ltype;
    BAColorType Ctype;
    char filename[256];
    char options[256];
} BALightColorMap;

/* scalar map options
-blendu on | off 
-blendv on | off 
-clamp on | off 
-imfchan r | g | b | m | l | z 
-mm base gain 
-o u v w 
-s u v w 
-t u v w 
-texres value 
*/

//map_Ns, map_d, disp, decal, bump
typedef struct {
    BAScalarType type;
    char filename[256];
    char options[256];
} BAScalarMap;

/* reflection map options
-blendu on | off 
-blendv on | off 
-cc on | off 
-clamp on | off 
-mm base gain 
-o u v w 
-s u v w 
-t u v w 
-texres value 
*/

//refl -type
typedef struct {
    char filename[256];
    char options[256];
} BAReflectionMap;

typedef struct BAMaterialStruct {
    struct BAMaterialStruct* next;
    //newmtl
    char name[256];
    //map_Kd
    char diffuseMapName[256];
    //map_Ks
    char specularMapName[256];
    //light color
    BALightColor lightColors[5];
    //illumination model 0->10
    int illumModelNum;
    //dissolve (d/d -halo)
    double dissolveFactor;
    double dissolveHaloFactor;
    //specular exponent (Ns) 0->1000
    double specularExponent;
    //sharpness value 0->1000 default 60
    int sharpnessValue;
    //index of refraction (Ni) 0.001->10
    int indexOfRefraction;    
    //texture map
    BALightColorMap lightColorMaps[10];
    BAScalarMap     scalarMaps[10];
    BAReflectionMap reflectionMaps[10];
    //extension
    int hidden;
} BAMaterial;

MCInline BAMaterial* BAMaterialNew(const char* name) {
    BAMaterial* mtl = (BAMaterial*)malloc(sizeof(BAMaterial));
    mtl->next = null;
    MCStringFill(mtl->name, name);
    return mtl;
}

MCInline MCVector3 BAMaterialLightColor(BAMaterial* mat, BALightType type) {
    double R = mat->lightColors[type].data.rgbxyz[0];
    double G = mat->lightColors[type].data.rgbxyz[1];
    double B = mat->lightColors[type].data.rgbxyz[2];
    return MCVector3Make(R, G, B);
}

typedef struct BATextureStruct {
    struct BATextureStruct* next;
    char filename[LINE_MAX];
    char attachedGroup[LINE_MAX];
    char attachedObject[LINE_MAX];
} BATexture;

MCInline BATexture* BATextureNew(const char* filename, const char* groupname, const char* objectname) {
    BATexture* tex = (BATexture*)malloc(sizeof(BATexture));
    tex->next = null;
    tex->filename[0] = NUL;
    tex->attachedGroup[0] = NUL;
    tex->attachedObject[0]= NUL;
    if (filename) {
        MCStringFill(tex->filename, filename);
    }
    if (groupname) {
        MCStringFill(tex->attachedGroup, filename);
    }
    if (objectname) {
        MCStringFill(tex->attachedObject, filename);
    }
    return tex;
}

typedef struct BAMtlLibraryStruct {
    struct BAMtlLibraryStruct* next;
    BAMaterial* materialsList;
    BATexture* texturesList;
    char name[256];
} BAMtlLibrary;

MCInline BAMaterial* BAFindMaterial(BAMtlLibrary* lib, const char* name) {
    if (lib && name) {
        BAMaterial* iter = lib->materialsList;
        while (iter) {
            if (MCStringEqual(iter->name, name)) {
                return iter;
            }
            iter = iter->next;
        }
    }
    return null;
}

MCInline void BAAddMaterial(BAMtlLibrary* lib, const char* name) {
    if (lib && name) {
        //insert at head
        BAMaterial* newmtl = BAMaterialNew(name);
        newmtl->next = lib->materialsList;
        lib->materialsList = newmtl;
    }
}

MCInline BATexture* BAFindTextureByFilename(BAMtlLibrary* lib, const char* filename) {
    if (lib && filename) {
        BATexture* iter = lib->texturesList;
        while (iter) {
            if (MCStringEqual(iter->filename, filename)) {
                return iter;
            }
            iter = iter->next;
        }
    }
    return null;
}

MCInline BATexture* BAFindTextureByAttachedGroup(BAMtlLibrary* lib, const char* group) {
    if (lib && group) {
        BATexture* iter = lib->texturesList;
        while (iter) {
            if (MCStringEqual(iter->attachedGroup, group)) {
                return iter;
            }
            iter = iter->next;
        }
    }
    return null;
}

MCInline BATexture* BAFindTextureByAttachedObject(BAMtlLibrary* lib, const char* object) {
    if (lib && object) {
        BATexture* iter = lib->texturesList;
        while (iter) {
            if (MCStringEqual(iter->attachedObject, object)) {
                return iter;
            }
            iter = iter->next;
        }
    }
    return null;
}

MCInline void BAAddTexture(BAMtlLibrary* lib, const char* filename,
                           const char* groupname, const char* objectname) {
    if (lib && filename) {
        //insert at head
        BATexture* newtex = BATextureNew(filename, groupname, objectname);
        newtex->next = lib->texturesList;
        lib->texturesList = newtex;
    }
}

MCInline BAMtlLibrary* BAFindMtlLibrary(BAMtlLibrary* list, const char* name) {
    if (list && name) {
        BAMtlLibrary* iter = list;
        while (iter) {
            if (MCStringEqual(iter->name, name)) {
                return iter;
            }
            iter = iter->next;
        }
    }
    return null;
}

BAMtlLibrary* BAMtlLibraryNew(const char* filename);
void BAMtlLibraryRelease(BAMtlLibrary* lib);

MCInline void BAAddMtlLibrary(BAMtlLibrary** list, const char* name) {
    if (list && name) {
        //insert at head
        BAMtlLibrary* newlib = BAMtlLibraryNew(name);
        if (newlib) {
            newlib->next = *list;
            *list = newlib;
        }
        else{
            error_log("BAMtlParser - can not add mtllib %s\n", name);
        }
    }
}

#endif /* MC3DMtlParser_h */
