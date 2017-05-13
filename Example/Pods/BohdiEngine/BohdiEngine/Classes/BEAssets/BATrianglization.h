//
//  BATrianglization.h
//  Sapindus
//
//  Created by Sun YuLi on 2016/10/11.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#ifndef BATrianglization_h
#define BATrianglization_h

#include "BAObjParser.h"

typedef struct {
    long vi;
    long ti;
    long ni;
} BAFaceElement;

typedef union {
    struct {
        BAFaceElement e1;
        BAFaceElement e2;
        BAFaceElement e3;
    };
    struct {
        long v1;
        long t1;
        long n1;
        
        long v2;
        long t2;
        long n2;
        
        long v3;
        long t3;
        long n3;
    };
    long data[9];
} BATriangle;

size_t       getTrianglesBuffSize(BAFace* faces, size_t facecounts);
BATriangle* createTrianglesBuffer(BAFace* faces, size_t facecounts);
void       releaseTrianglesBuffer(BATriangle* buff);

size_t trianglization(BATriangle* triangles, BAFace* faces, size_t facecounts, MCVector3* vertexbuff);

#endif /* BATrianglization_h */
