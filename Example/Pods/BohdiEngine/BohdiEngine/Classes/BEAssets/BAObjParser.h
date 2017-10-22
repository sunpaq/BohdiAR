//
//  MC3DFileParser.h
//  monkcGame
//
//  Created by Sun YuLi on 16/3/17.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef MC3DFileParser_h
#define MC3DFileParser_h

#include "monkc.h"
#include "MCMath.h"
#include "MCGeometry.h"
#include "MCLexer.h"
#include "BEAssetsManager.h"
#include "BAMtlParser.h"

typedef struct {
    long*  big;
    long   small[18];
    long*  data;
    size_t vcount;
} BAFace;

MCInline void BAFaceInit(BAFace* face, long* buff, size_t vcount)
{
    face->big = null;
    memset(&face->small[0], 0, sizeof(face->small));
    size_t size = sizeof(long) * vcount;
    if (vcount <= 18) {
        memcpy(face->small, buff, size);
        face->data = face->small;
    }else{
        face->big = (long*)malloc(size);
        memcpy(face->big, buff, size);
        face->data = face->big;
    }
    face->vcount = vcount;
}

typedef union {
    struct {
        double xmax;
        double xmin;
        double ymax;
        double ymin;
        double zmax;
        double zmin;
    };
    double m[6];
} BACubeFrame;

typedef struct {
    size_t vertex_count;
    size_t texcoord_count;
    size_t normal_count;
    size_t face_count;
    size_t mesh_count;
    
    size_t object_count;
    size_t group_count;
    size_t usemtl_count;
    
    size_t object_starts[256];
    size_t group_starts[1024];
    size_t usemtl_starts[1024];
    
    size_t mtllib_count;
} BAObjMeta;

MCInline void BAObjMetaInit(BAObjMeta* meta) {
    meta->vertex_count    = 0;
    meta->texcoord_count  = 0;
    meta->normal_count    = 0;
    meta->face_count      = 0;
    meta->mesh_count      = 0;
    
    meta->object_count    = 0;
    meta->group_count     = 0;
    meta->usemtl_count    = 0;
    
    memset(meta->object_starts, 0, 256);
    memset(meta->group_starts, 0, 1024);
    memset(meta->usemtl_starts, 0, 1024);
    
    meta->mtllib_count    = 0;
}

typedef struct BAMeshStruct {
    size_t prevVertexNum;
    size_t startFaceCount;
    size_t totalFaceCount;
    BAMaterial* usemtl;
    char object[LINE_MAX];
    char group[LINE_MAX];
} BAMesh;

typedef struct BAObjDataStruct {
    BACubeFrame Frame;
    //raw data
    MCVector3* vertexbuff;
    MCVector2* texcoorbuff;
    MCVector4* normalbuff;//use w record times
    size_t normal_count;
    MCBool shouldCalculateNormal;
    //faces
    BAFace* facebuff;
    size_t  facecount;
    //mesh
    BAMesh* meshbuff;
    size_t  meshcount;
    //mtllib list
    BAMtlLibrary* mtllib_list;
    //name
    char name[256];
} BAObjData;

MCInline void BAObjDumpInfo(BAObjData* baobj)
{
    for (int i=0; i<baobj->meshcount; i++) {
        BAMesh* m = &baobj->meshbuff[i];
        if (m) {
            printf("BAObjParser - ");
            if (m->object[0]) {
                printf("object:[%s] ", m->object);
            }
            if (m->group[0]) {
                printf("group:[%s] ", m->group);
            }
            if (baobj->mtllib_list && m->usemtl) {
                printf("usemtl:[%s] ", m->usemtl->name);
            }
            printf("\n");
        }
    }
    printf("total %ld meshes\n", baobj->meshcount);
}

MCInline BAObjData* BAObjAlloc(BAObjMeta* meta)
{
    BAObjData* buff = (BAObjData*)malloc(sizeof(BAObjData));
    if (buff) {
        buff->Frame = (BACubeFrame){0};
        size_t sizevb = sizeof(MCVector3) * (meta->vertex_count);
        size_t sizetb = sizeof(MCVector2) * (meta->texcoord_count);
        size_t sizenb = sizeof(MCVector4) * (meta->normal_count);
        size_t sizefb = sizeof(BAFace) * (meta->face_count);
        size_t sizemb = sizeof(BAMesh) * (meta->mesh_count);
        
        //vertex buffer
        if (sizevb > 0) {
            buff->vertexbuff = (MCVector3*)malloc(sizevb);
            memset(buff->vertexbuff, 0, sizevb);
        } else {
            buff->vertexbuff = null;
        }
        
        //texture coordinate buffer
        if (sizetb > 0) {
            buff->texcoorbuff = (MCVector2*)malloc(sizetb);
            memset(buff->texcoorbuff, 0, sizetb);
        } else {
            buff->texcoorbuff = null;
        }
        
        //normal buffer
        if (sizenb > 0) {
            buff->normalbuff = (MCVector4*)malloc(sizenb);
            memset(buff->normalbuff, 0, sizenb);
        } else {
            buff->normalbuff = null;
        }
        
        //polygon faces buffer
        if (sizefb > 0) {
            buff->facebuff = (BAFace*)malloc(sizefb);
            memset(buff->facebuff, 0, sizefb);
        } else {
            buff->facebuff = null;
        }
        
        //mesh buffer
        if (sizemb > 0) {
            buff->meshbuff = (BAMesh*)malloc(sizemb);
            memset(buff->meshbuff, 0, sizemb);
        } else {
            buff->meshbuff = null;
        }

        buff->normal_count = meta->normal_count;
        buff->facecount   = meta->face_count;
        buff->meshcount   = meta->mesh_count;

        buff->shouldCalculateNormal = false;
        
        buff->mtllib_list = null;
        if (buff->vertexbuff && buff->facebuff) {
            buff->name[0] = NUL;
            return buff;
        }
    }
    error_log("BAObjParser - BAObjAlloc failed. face count is %d\n", meta->face_count);
    return null;
}

BAObjData* BAObjDataNewWithFilepath(const char* filepath, BAObjMeta* meta);
BAObjData* BAObjDataNew(const char* filename, BAObjMeta* meta);

void BAObjRelease(BAObjData* buff);

#endif /* MC3DFileParser_h */
