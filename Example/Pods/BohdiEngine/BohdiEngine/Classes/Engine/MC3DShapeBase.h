#ifndef __MC3DShapeBase__
#define __MC3DShapeBase__
#include "MC3DBase.h"
#include "MCUIBase.h"

//algorithm copy from http://slabode.exofire.net/circle_draw.shtml
MCInline void MakeCircleData(float cx, float cy, float cz, float r, int num_segments, double vertexs[])
{
    float theta = 2 * 3.1415926 / (float)(num_segments);
    float c = cosf(theta);//precalculate the sine and cosine
    float s = sinf(theta);
    float t;
    
    float x = r;//we start at angle = 0
    float y = 0;
    
    for(int i = 0; i < num_segments; i++)
    {
        vertexs[i*3]   = x + cx;
        vertexs[i*3+1] = y + cy;
        vertexs[i*3+2] = cz;
        //apply the rotation matrix
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;
        
        
    }
}

MCInline MCUInt MCDrawLinePrepare(MCVector3 p1, MCVector3 p2, MCColor color)
{
    float data[18] = {
        p1.x, p1.y, p1.z, 0,0,0, color.R/255, color.G/255, color.B/255,
        p2.x, p2.y, p2.z, 0,0,0, color.R/255, color.G/255, color.B/255
    };
    MCUInt bufferid;//GLuint
    glGenBuffers(1, &bufferid);
    glBindBuffer(GL_ARRAY_BUFFER, bufferid);
    glBufferData(GL_ARRAY_BUFFER, 18*4, data, GL_STATIC_DRAW);//GL_STREAM_DRAW, GL_DYNAMIC_DRAW
    glEnableVertexAttribArray(MCVertexAttribPosition);
    glEnableVertexAttribArray(MCVertexAttribColor);
    glVertexAttribPointer(MCVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 9*4, MCBUFFER_OFFSET(0));
    glVertexAttribPointer(MCVertexAttribNormal,   3, GL_FLOAT, GL_FALSE, 9*4, MCBUFFER_OFFSET(3*4));
    glVertexAttribPointer(MCVertexAttribColor,    3, GL_FLOAT, GL_FALSE, 9*4, MCBUFFER_OFFSET(6*4));
    return bufferid;
}

MCInline void MCDrawLine(MCUInt bufferid)
{
    const MCUInt count = 2;
    glBindVertexArray(bufferid);
    glDrawArrays(MCLines, 0, count);
}

//nr: row nc: column of sphere texture
MCInline GLuint MCGenerateSkysphere(int nr, int nc, GLfloat R, GLfloat* vertices, GLuint* indices)
{
    const GLfloat dr = M_PI / (nr-1);
    const GLfloat dc = (2.0*M_PI) / (nc-1);
    //vertex
    int cur = 0;
    for (int r=0; r<nr; r++) {
        GLfloat tht = r * dr;
        GLfloat z = R * cosf(tht);
        
        for (int c=0; c<nc; c++) {
            GLfloat fai = 2.0 * M_PI - c * dc;
            //GLfloat fai = c * dc;
            GLfloat x = R * sinf(tht) * cosf(fai);
            GLfloat y = R * sinf(tht) * sinf(fai);
            //uv
            GLfloat u = ((double)c) / ((double)(nc-1));
            GLfloat v = ((double)r) / ((double)(nr-1));
            //vertex
            vertices[cur++] = x;
            vertices[cur++] = y;
            vertices[cur++] = z;
            vertices[cur++] = u;
            vertices[cur++] = v;
        }
    }
    //vertices
    GLuint ic = 0;
    for (int r=1; r<nr; r++) {
        int bot = r*nc;
        int top = (r - 1)*nc;
        for (int c=0; c<nc; c++) {
            indices[ic++] = top+c;
            indices[ic++] = bot+c;
        }
    }
    return ic;
}

#endif

