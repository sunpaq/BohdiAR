//
//  BATrianglization.c
//  Sapindus
//
//  Created by Sun YuLi on 2016/10/11.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "BATrianglization.h"

size_t getTrianglesBuffSize(BAFace* faces, size_t facecounts)
{
    size_t trisize = 0;
    for (size_t i=0; i<facecounts; i++) {
        BAFace face = faces[i];
        if (face.vcount > 0) {
            size_t vertexCount = 0;
            vertexCount += face.vcount;
            if (vertexCount < 3) {
                error_log("BATrianglization - vertexCount < 3\n");
            }
            else if (vertexCount == 3) {
                trisize++;
            }
            else if (vertexCount > 3) {
                trisize += vertexCount - 2;
            }
        }else{
            //error_log("BATrianglization - face vertex count is 0\n");
        }
    }
    return trisize;
}

BATriangle* createTrianglesBuffer(BAFace* faces, size_t facecounts)
{
    size_t size = getTrianglesBuffSize(faces, facecounts);
    if (size > 0) {
        BATriangle* triangles = (BATriangle*)malloc(sizeof(BATriangle) * size);
        memset(triangles, 0, sizeof(BATriangle) * size);
        return triangles;
    }
    return null;
}

void releaseTrianglesBuffer(BATriangle* buff)
{
    free(buff);
}

//return face count;
size_t trianglization(BATriangle* triangles, BAFace* faces, size_t facecounts, MCVector3* vertexbuff)
{
    size_t tricounts = 0;
    for (int i=0; i<facecounts; i++) {
        BAFace* face = &faces[i];
        long* data = face->data;
        if (!data) {
            continue;
        }
        if (face->vcount < 3) {
            continue;
        }
        else if (face->vcount == 3) {
            triangles[tricounts] = (BATriangle) {
                data[0], 0, 0,
                data[1], 0, 0,
                data[2], 0, 0
            };
            tricounts++;
        }
        else if (face->vcount == 6) {
            triangles[tricounts] = (BATriangle) {
                data[0], data[1], 0,
                data[2], data[3], 0,
                data[4], data[5], 0
            };
            tricounts++;
        }
        else if (face->vcount == 9) {
            triangles[tricounts] = (BATriangle) {
                data[0], data[1], data[2],
                data[3], data[4], data[5],
                data[6], data[7], data[8]
            };
            tricounts++;
        }
        else if (face->vcount > 9) {
            int count = (int)(face->vcount / 3);
            MCVector3  mcvertexes[LINE_MAX];
            MCTriangle mctriangles[LINE_MAX-2];
            
            for (int i=0; i<count; i++) {
                mcvertexes[i] = vertexbuff[data[i*3]];
            }
            
            MCPolygon Poly = {0};
            MCPolygonInit(&Poly, mcvertexes, count);
            
            if (Poly.isConvex) {
                //debug_log("CONVEX poly: %s\n", linebuff);
                for (int i=0; i< face->vcount-6; i=i+3) {
                    //face
                    triangles[tricounts] = (BATriangle){
                        data[0  ], data[1  ], data[2  ],
                        data[i+3], data[i+4], data[i+5],
                        data[i+6], data[i+7], data[i+8]
                    };
                    tricounts++;
                }
            }else{
                size_t epcount = 0;
                size_t viresult[LINE_MAX];
                size_t mctricount = MCPolygonResolveConcave(&Poly, mctriangles, viresult);
                if (mctricount == 0) {
                    //error_log("poly: %s\n", linebuff);
                    MCPolygonDumpVertexData(&Poly);
                    error_log("error poly %d\n", epcount++);
                }else{
                    //debug_log("poly: %s\n", linebuff);
                }
                
                for (int i=0; i<mctricount; i++) {
                    
                    size_t vi1 = viresult[i*3+0];
                    size_t vi2 = viresult[i*3+1];
                    size_t vi3 = viresult[i*3+2];
                    
                    long v1 = data[vi1*3+0];
                    long t1 = data[vi1*3+1];
                    long n1 = data[vi1*3+2];
                    
                    long v2 = data[vi2*3+0];
                    long t2 = data[vi2*3+1];
                    long n2 = data[vi2*3+2];
                    
                    long v3 = data[vi3*3+0];
                    long t3 = data[vi3*3+1];
                    long n3 = data[vi3*3+2];
                    
                    triangles[tricounts] = (BATriangle){
                        v1, t1, n1,
                        v2, t2, n2,
                        v3, t3, n3
                    };
                    tricounts++;
                }
            }
        }
    }
    return tricounts;
}
