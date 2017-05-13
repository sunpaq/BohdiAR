//
//  MC3DMtlParser.c
//  Sapindus
//
//  Created by Sun YuLi on 2016/10/4.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "BEAssetsManager.h"
#include "BAMtlParser.h"
#include "MCIO.h"

/*
 Ka|Kd|Ks|Tf [xyz|spectral] rx gy bz | [file.rfl factor]
 */
//return face count
MCInline void processMtlLine(BAMtlLibrary* lib, const char* linebuff)
{
    //debug_log("processMtlLine:%s\n", linebuff);
    
    //MCToken token;
    MCToken token;
    
    char word[256] = {0};
    const char* remain = linebuff;
    while (!isNewLine(remain) && *remain != NUL) {
        token = tokenize(nextWord(&remain, word));
        
        switch (token.type) {
            case MCTokenIdentifier:
                if (MCStringEqualN(word, "newmtl", 6)) {
                    token = tokenize(nextWord(&remain, word));
                    if (token.type == MCTokenIdentifier || token.type == MCTokenFilename) {
                        if (!BAFindMaterial(lib, token.value.Word)) {
                            BAAddMaterial(lib, token.value.Word);
                        }
                        continue;
                    }
                }
                //texture
                else if (MCStringEqualN(word, "map_Ka", 6)) {
                    return;//next line
                }
                else if (MCStringEqualN(word, "map_Kd", 6)) {
                    char name[256] = {0};
                    BAMaterial* material = lib->materialsList;
                    if (material && MCString_filenameFromPath(remain, &name)) {
                        MCStringFill(material->diffuseMapName, name);
                        
                    } else {
                        error_log("BAMtlParser - can not get filename form path: %s\n", remain);
                    }
                    return;//next line
                }
                else if (MCStringEqualN(word, "map_Ks", 6)) {
                    char name[256] = {0};
                    BAMaterial* material = lib->materialsList;
                    if (material && MCString_filenameFromPath(remain, &name)) {
                        MCStringFill(material->specularMapName, name);
                    } else {
                        error_log("BAMtlParser - can not get filename form path: %s\n", remain);
                    }
                    return;//next line
                }
                else if (MCStringEqualN(word, "map_Ke", 6)) {
                    return;//next line
                }
                //LSLightColor
                else if (MCStringEqualN(word, "illum", 5)) {
                    token = tokenize(nextWord(&remain, word));
                    BAMaterial* material = lib->materialsList;
                    if (material) {
                        if (token.type == MCTokenIdentifier) {
                            if (MCStringEqualN(token.value.Word, "illum_", 6)) {
                                char* num = &token.value.Word[6];
                                int n = atoi(num);
                                if (n >= 0 && n <= 10) {
                                    material->illumModelNum = n;
                                }
                            }
                        }
                        else if (token.type == MCTokenInteger) {
                            int n = (int)token.value.Integer;
                            if (n >= 0 && n <= 10) {
                                material->illumModelNum = n;
                            }
                        }
                    }
                }
                //Ka|Kd|Ks|Tf [xyz|spectral] rx gy bz | [file.rfl factor]
                else if (MCStringEqualN(word, "K", 1) || MCStringEqualN(word, "Tf", 2)) {
                    BALightColor* light = null;
                    if (MCStringEqualN(word, "Tf", 2)) {
                        light = &(lib->materialsList->lightColors[TFilter]);
                    }
                    else if (MCStringEqualN(word, "Ka", 2)) {
                        light = &(lib->materialsList->lightColors[Ambient]);
                    }
                    else if (MCStringEqualN(word, "Kd", 2)) {
                        light = &(lib->materialsList->lightColors[Diffuse]);
                    }
                    else if (MCStringEqualN(word, "Ks", 2)) {
                        light = &(lib->materialsList->lightColors[Specular]);
                    }
                    else if (MCStringEqualN(word, "Ke", 2)) {
                        light = &(lib->materialsList->lightColors[Emissive]);
                    }
                    if (!light) {
                        error_log("BAMtlParser - [%s] not light Ka/Kd/Ks\n", word);
                        return;
                    }
                    light->Ctype = RGB;
                    
                    //option next
                    token = tokenize(peekNext(&remain, word));
                    if (token.type == MCTokenIdentifier) {
                        if (MCStringEqualN(token.value.Word, "xyz", 3)) {
                            light->Ctype = XYZ;
                        }
                        else if (MCStringEqualN(token.value.Word, "spectral", 8)) {
                            light->Ctype = SpectralFile;
                            //filename next
                            skipNext(&remain);//skip
                            token = tokenize(nextWord(&remain, word));
                            if (token.type == MCTokenIdentifier) {
                                MCStringFill(light->data.spectral, token.value.Word);
                                //factor next
                                token = tokenize(nextWord(&remain, word));
                                if (token.type == MCTokenFloat) {
                                    light->factor = token.value.Double;
                                }
                            }
                            //end line
                            return;
                        }
                    }
                    //float value next
                    else if (token.type == MCTokenFloat || token.type == MCTokenInteger) {
                        if (light->Ctype != SpectralFile) {
                            double buff[3] = {0};
                            size_t n = nextNumbersAsFloat(&remain, buff);
                            if (n >= 3) {
                                light->data.rgbxyz[0] = buff[0];
                                light->data.rgbxyz[1] = buff[1];
                                light->data.rgbxyz[2] = buff[2];
                            } else {
                                error_log("MC3DMtlParser [%s] value count less than 3\n", linebuff);
                            }
                        }
                    }
                }
                //LSScalar
                else if (MCStringEqualN(word, "Ns", 2)) {
                    token = tokenize(nextWord(&remain, word));
                    if (token.type == MCTokenInteger) {
                        lib->materialsList->specularExponent = (double)token.value.Integer;
                    }
                    if (token.type == MCTokenFloat) {
                        lib->materialsList->specularExponent = (double)token.value.Double;
                    }
                    continue;
                }
                else if (MCStringEqualN(word, "decal", 5)) {
                    //state = LSScalar;
                }
                else if (MCStringEqualN(word, "disp", 4)) {
                    //state = LSScalar;
                }
                else if (MCStringEqualN(word, "bump", 4)) {
                    //state = LSScalar;
                }
                else if (MCStringEqualN(word, "d", 1)) {
                    token = tokenize(nextWord(&remain, word));
                    if (lib->materialsList) {
                        if (token.type == MCTokenFloat) {
                            lib->materialsList->dissolveFactor = (double)token.value.Double;
                        }
                        if (token.type == MCTokenInteger) {
                            lib->materialsList->dissolveFactor = (double)token.value.Integer;
                        }
                    }
                    continue;
                }
                //BohdiEngine extensions
                else if (MCStringEqualN(word, "ext_hidden", 10)) {
                    token = tokenize(nextWord(&remain, word));
                    if (lib->materialsList) {
                        if (token.type == MCTokenIdentifier && MCStringEqualN(token.value.Word, "off", 3)) {
                            lib->materialsList->hidden = 0;
                        }else{
                            lib->materialsList->hidden = 1;
                        }
                    }
                }
                else if (MCStringEqualN(word, "ext_tex_file", 12)) {
                    token = tokenize(nextWord(&remain, word));
                    if(!BAFindTextureByFilename(lib, token.value.Word)) {
                        BAAddTexture(lib, token.value.Word, null, null);
                    }
                }
                else if (MCStringEqualN(word, "ext_tex_group", 13)) {
                    token = tokenize(nextWord(&remain, word));
                    BATexture* tex = lib->texturesList;
                    MCStringFill(tex->attachedGroup, token.value.Word);
                }
                else if (MCStringEqualN(word, "ext_tex_object", 14)) {
                    token = tokenize(nextWord(&remain, word));
                    BATexture* tex = lib->texturesList;
                    MCStringFill(tex->attachedObject, token.value.Word);
                    
                }
                else {
                    
                }
                break;
            case MCTokenComment:
            case MCTokenUnknown:
            default:
                nextWord(&remain, word);
                continue;
        }
    }

    return;
}

static BAMtlLibrary* BAMtlLibraryAlloc() {
    BAMtlLibrary* lib = (BAMtlLibrary*)malloc(sizeof(BAMtlLibrary));
    if (lib) {
        lib->next = null;
        lib->materialsList = null;
        lib->texturesList = null;
        lib->name[0] = NUL;
        return lib;
    }
    return null;
}

BAMtlLibrary* BAMtlLibraryNew(const char* filename)
{
    char path[LINE_MAX] = {0};
    if(MCFileGetPath(filename, path)) {
        return null;
    }
    
    const char* assetbuff = MCFileCopyContentWithPath(path);
    
    if (assetbuff != null) {
        BAMtlLibrary* lib = BAMtlLibraryAlloc();
        if (lib == null) {
            error_log("MC3DObjParser - MC3DMtlLibraryAlloc failed.\n");
            return null;
        }
        
        char line[LINE_MAX];
        char* c = (char*)assetbuff;
        while (*c!=NUL) {
            //skip '\n' '\r\n' '\t' ' '
            if (*c==MCNewLineN || *c==MCNewLineR || *c==MCTab || *c==MCWhiteSpace) {
                c++; continue;
            }
            for (int i=0; !isNewLine(c); c++) {
                line[i++] = *c;
                line[i] = NUL;
            }
            processMtlLine(lib, line);
        }
        free((void*)assetbuff);
        MCStringFill(lib->name, filename);
        return lib;
    }else{
        error_log("MC3DObjParser - AAssetManager_open %s failed\n", filename);
        return null;
    }
}

static void recursiveFreeBAMaterial(BAMaterial* ptr)
{
    if (!ptr)
        return;
    if (ptr->next)
        recursiveFreeBAMaterial(ptr->next);
    free(ptr);
}

static void recursiveFreeBATexture(BATexture* ptr)
{
    if (!ptr)
        return;
    if (ptr->next)
        recursiveFreeBATexture(ptr->next);
    free(ptr);
}

void BAMtlLibraryRelease(BAMtlLibrary* lib)
{
    if (lib->materialsList) {
        recursiveFreeBAMaterial(lib->materialsList);
    }
    if (lib->texturesList) {
        recursiveFreeBATexture(lib->texturesList);
    }
    free(lib);
}

