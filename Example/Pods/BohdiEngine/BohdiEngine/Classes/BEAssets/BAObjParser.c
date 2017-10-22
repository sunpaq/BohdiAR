#include "BAObjParser.h"
#include "BEAssetsManager.h"
#include "MCGeometry.h"
#include "MCIO.h"

void parseObjMeta(BAObjMeta* meta, const char* buff)
{
    BAObjMetaInit(meta);
    if (meta && buff) {
        MCBool meshDetected = false;
        
        char line[LINE_MAX]; char* c = (char*)buff;
        while (*c!=NUL) {
            //skip '\n' '\r\n' '\t' ' '
            if (*c==MCNewLineN || *c==MCNewLineR || *c==MCTab || *c==MCWhiteSpace) {
                c++; continue;
            }
            for (int i=0; !isNewLine(c); c++) {
                line[i++] = *c;
                line[i] = NUL;
            }
            //process line
            const char* remain = line; char word[256];
            MCToken token = tokenize(nextWord(&remain, word));
            if (token.type == MCTokenIdentifier) {
                if (MCStringEqualN(word, "vt", 2)) {
                    meta->texcoord_count++;
                }
                else if (MCStringEqualN(word, "vn", 2)) {
                    meta->normal_count++;
                }
                else if (MCStringEqualN(word, "v", 1)) {
                    meta->vertex_count++;
                }
                else if (MCStringEqualN(word, "f", 1)) {
                    if (meshDetected == false) {
                        meshDetected = true;
                        meta->mesh_count++;
                    }
                    meta->face_count++;
                    continue;
                }
                else if (MCStringEqualN(word, "o", 1)) {
                    meta->object_starts[meta->object_count] = meta->face_count;
                    meta->object_count++;
                }
                else if (MCStringEqualN(word, "g", 1)) {
                    meta->group_starts[meta->group_count] = meta->face_count;
                    meta->group_count++;
                }
                else if (MCStringEqualN(word, "s", 1)) {
                    //smooth group don't break a mesh
                    continue;
                }
                else if (MCStringEqualN(word, "mtllib", 6)) {
                    meta->mtllib_count++;
                }
                else if (MCStringEqualN(word, "usemtl", 6)) {
                    MCToken token = tokenize(nextWordsInThisLine(&remain, word));
                    if (token.type == MCTokenIdentifier || token.type == MCTokenFilename) {
                        meta->usemtl_starts[meta->usemtl_count] = meta->face_count;
                        meta->usemtl_count++;
                        debug_log("usemtl %s\n", token.value.Word);
                    }
                }
            }
            meshDetected = false;
        }//end while
    }
}

void parseObj(BAObjData* object, const char* file)
{
    if (object && file) {
        size_t vcursor = 0;
        size_t tcursor = 0;
        size_t ncursor = 0;
        size_t fcursor = 0;
        size_t mcursor = 0;
        
        BAMesh* current_mesh = null;

        char usemtl_name[LINE_MAX] = {0};
        char object_name[LINE_MAX] = {0};
        char group_name[LINE_MAX]  = {0};
        
        char line[LINE_MAX]; char* c = (char*)file;
        while (*c != NUL && *c != EOF) {
            //skip '\n' '\r\n' '\t' ' '
            if (*c==MCNewLineN || *c==MCNewLineR || *c==MCTab || *c==MCWhiteSpace) {
                c++; continue;
            }
            for (int i=0; !isNewLine(c); c++) {
                line[i++] = *c;
                line[i] = NUL;
            }
            //process line
            //debug_log("process line: %s\n", line);
            char word[256]; const char* remain = line;
            MCToken token = tokenize(nextWord(&remain, word));
            switch (token.type) {
                case MCTokenIdentifier:
                    //don't change the order!
                    if (MCStringEqualN(word, "vt", 2)) {
                        double fbuff[LINE_MAX];
                        size_t n = nextFloats(&remain, fbuff);
                        for (int i=0; i<n; i++)
                            object->texcoorbuff[tcursor].v[i] = (float)fbuff[i];
                        tcursor++;
                    }
                    else if (MCStringEqualN(word, "vn", 2)) {
                        double fbuff[LINE_MAX];
                        size_t n = nextFloats(&remain, fbuff);
                        for (int i=0; i<n; i++)
                            object->normalbuff[ncursor].v[i] = (float)fbuff[i];
                        ncursor++;
                    }
                    else if (MCStringEqualN(word, "vp", 2)) {
                        //parameter
                    }
                    else if (MCStringEqualN(word, "v", 1)) {
                        double fbuff[LINE_MAX];
                        size_t n = nextFloats(&remain, fbuff);
                        for (int i=0; i<n; i++)
                            object->vertexbuff[vcursor].v[i] = fbuff[i];
                        vcursor++;
                    }
                    else if (MCStringEqualN(word, "f", 1)) {
                        //peek next value
                        token = tokenize(peekNext(&remain, word));
                        if (token.type == MCTokenDate || token.type == MCTokenInteger) {
                            //common
                            if (!current_mesh) {
                                current_mesh = &object->meshbuff[mcursor++];
                                current_mesh->prevVertexNum  = vcursor-1;
                                current_mesh->startFaceCount = fcursor;
                                current_mesh->totalFaceCount = 0;
                                current_mesh->usemtl = null;
                                if (group_name[0]) {
                                    MCStringFill(current_mesh->group, group_name);
                                }
                                if (object_name[0]) {
                                    MCStringFill(current_mesh->object, object_name);
                                }
                                if (usemtl_name[0]) {
                                    current_mesh->usemtl = BAFindMaterial(object->mtllib_list, usemtl_name);
                                }
                            }
                            //special
                            if (token.type == MCTokenDate) {
                                long lbuff[LINE_MAX];
                                BAFace* f = &object->facebuff[fcursor];
                                f->vcount = nextDates(&remain, lbuff);
                                if (f->vcount < 6) {
                                    error_log("[%s] -> detect a face have less then 3 vertex, ignore it\n", line);
                                } else {
                                    BAFaceInit(f, lbuff, f->vcount);
                                    current_mesh->totalFaceCount++;
                                    fcursor++;
                                }
                            }
                            if (token.type == MCTokenInteger) {
                                long lbuff[LINE_MAX];
                                BAFace* f = &object->facebuff[fcursor];
                                f->vcount = nextIntegers(&remain, lbuff);
                                if (f->vcount < 3) {
                                    error_log("[%s] -> detect a face have less then 3 vertex, ignore it\n", line);
                                } else {
                                    BAFaceInit(f, lbuff, f->vcount);
                                    current_mesh->totalFaceCount++;
                                    fcursor++;
                                }
                            }
                            //common final
                            continue;
                        }
                    }
                    else if (MCStringEqualN(word, "g", 1)) {
                        token = tokenize(nextWordsInThisLine(&remain, word));
                        if (token.type == MCTokenIdentifier || token.type == MCTokenFilename) {
                            MCStringFill(group_name, token.value.Word);
                        }
                    }
                    else if (MCStringEqualN(word, "s", 1)) {
                        MCToken token = tokenize(nextWord(&remain, word));
                        if (token.type == MCTokenInteger && token.value.Integer != 0) {
                            object->shouldCalculateNormal = true;
                        }
                        if (token.type == MCTokenIdentifier && !MCStringEqualN("off", token.value.Word, 3)) {
                            object->shouldCalculateNormal = true;
                        }
                        continue;
                    }
                    else if (MCStringEqualN(word, "o", 1)) {
                        token = tokenize(nextWordsInThisLine(&remain, word));
                        if (token.type == MCTokenIdentifier || token.type == MCTokenFilename) {
                            MCStringFill(object_name, token.value.Word);
                        }
                    }
                    else if (MCStringEqualN(word, "mtllib", 6)) {
                        token = tokenize(nextWordsInThisLine(&remain, word));
                        if (token.type == MCTokenIdentifier || token.type == MCTokenFilename) {
                            if (!BAFindMtlLibrary(object->mtllib_list, token.value.Word)) {
                                BAAddMtlLibrary(&object->mtllib_list, token.value.Word);
                            }
                        }
                    }
                    else if (MCStringEqualN(word, "usemtl", 6)) {
                        token = tokenize(nextWordsInThisLine(&remain, word));
                        if (token.type == MCTokenIdentifier || token.type == MCTokenFilename) {
                            MCStringFill(usemtl_name, token.value.Word);
                        }
                    }
                    else {
                        
                    }
                    break;
                case MCTokenComment:
                case MCTokenUnknown:
                default:
                    break;
            }
            current_mesh = null;
        }//end while
    }
}

BAObjData* BAObjDataNewWithFilepath(const char* filepath, BAObjMeta* meta)
{
    const char* assetbuff;
    assetbuff = MCFileCopyContentWithPath(filepath);

    if (assetbuff) {
        parseObjMeta(meta, assetbuff);
        if (meta->face_count <= 0 || meta->vertex_count <= 0) {
            error_log("MC3DObjParser - object face count is ZERO\n");
            return null;
        }
        if (meta->normal_count == 0 || meta->normal_count < meta->vertex_count) {
            error_log("MC3DObjParser modle need calculate normal\n");
        }
        
        BAObjData* buff = BAObjAlloc(meta);
        if (!buff) {
            return null;
        }
        
        debug_log("MC3DObjParser - before parse\n");
        parseObj(buff, assetbuff);
        debug_log("MC3DObjParser - after parse\n");
        
        free((void*)assetbuff);
        return buff;
    }else{
        error_log("MC3DObjParser - AAssetManager_open %s failed\n", filepath);
        return null;
    }
}

BAObjData* BAObjDataNew(const char* filename, BAObjMeta* meta)
{
    char path[PATH_MAX] = {0};
    if(MCFileGetPath(filename, path)) {
        //error
        return null;
    }
    
    return BAObjDataNewWithFilepath(path, meta);
}

static void recursiveFreeBAMtlLibrary(BAMtlLibrary* lib)
{
    if (!lib)
        return;
    if (lib->next)
        recursiveFreeBAMtlLibrary(lib->next);
    BAMtlLibraryRelease(lib);
}

void BAObjRelease(BAObjData* buff)
{
    //recursively
    if (buff) {
        //clean up self
        for (int i=0; i<buff->facecount; i++) {
            BAFace* f = &buff->facebuff[i];
            if (f->big) {
                free(f->big);
            }
        }
        
        recursiveFreeBAMtlLibrary(buff->mtllib_list);
        
        free(buff->facebuff);
        free(buff->meshbuff);
        free(buff->vertexbuff);
        free(buff->texcoorbuff);
        free(buff->normalbuff);
        free(buff);
    }
}
