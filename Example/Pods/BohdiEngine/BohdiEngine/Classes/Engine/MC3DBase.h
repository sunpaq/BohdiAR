#ifndef _MC3DBase
#define _MC3DBase

#include <math.h>
#include "MCMath.h"
#include "MCGLBase.h"

#define MCBUFFER_OFFSET(i)         ((char *)NULL + (i))
#define MCARRAY_COUNT(array, type) (sizeof(array)/sizeof(type))

typedef enum {
    MC3DSuccess = 0,
    MC3DErrChildrenFull,
    MC3DErrChildNotFound,
    MC3DErrUniformNotFound = -1,
} MC3DErrCode;

//world is right hand y on top, local is left hand z on top
MCInline MCVector3 MCWorldCoorFromLocal(MCVector3 localvertex, MCVector3 modelposition) {
    return (MCVector3){modelposition.x+localvertex.y,
                      modelposition.y+localvertex.z,
                      modelposition.z+localvertex.x};
}

MCInline MCVector3 MCLocalCoorFromWorld(MCVector3 worldvertex, MCVector3 modelposition) {
    return (MCVector3){worldvertex.z-modelposition.z,
                      worldvertex.x-modelposition.x,
                      worldvertex.y-modelposition.y};
}

//R[0,unlimited) tht[0, M_PI), fai[0, 2M_PI)
MCInline MCVector3 MCVertexFromSpherical(double R, double tht, double fai) {
#if (defined(__APPLE__) || defined(__ANDROID__))
    double sinT = MCSinDegrees(tht);
    double sinF = MCSinDegrees(fai);
    double cosT = MCCosDegrees(tht);
    double cosF = MCCosDegrees(fai);
    double x = R * sinT * cosF;
    double y = R * sinT * sinF;
    double z = R * cosT;
#else
    double x = R * sin(tht) * cos(fai);
    double y = R * sin(tht) * sin(fai);
    double z = R * cos(tht);
#endif
    return (MCVector3){x,y,z};
}

MCInline MCVector3 MCVertexFromSpherical_radians(double R, double tht, double fai) {
#if (defined(__APPLE__) || defined(__ANDROID__))
    double sinT = sin(tht);
    double sinF = sin(fai);
    double cosT = cos(tht);
    double cosF = cos(fai);
    double x = R * sinT * cosF;
    double y = R * sinT * sinF;
    double z = R * cosT;
#else
    double x = R * sin(tht) * cos(fai);
    double y = R * sin(tht) * sin(fai);
    double z = R * cos(tht);
#endif
    return (MCVector3){x,y,z};
}

MCInline MCMatrix4 MCMatrix4MakePerspective(float fovyRadians, float aspect, float nearZ, float farZ)
{
    float cotan = 1.0f / tanf(fovyRadians / 2.0f);
    
    MCMatrix4 m = { cotan / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, cotan, 0.0f, 0.0f,
        0.0f, 0.0f, (farZ + nearZ) / (nearZ - farZ), -1.0f,
        0.0f, 0.0f, (2.0f * farZ * nearZ) / (nearZ - farZ), 0.0f };
    
    return m;
}

MCInline MCMatrix4 MCMatrix4MakeTranslation(float tx, float ty, float tz)
{
    MCMatrix4 m = MCMatrix4Identity;
    m.m[12] = tx;
    m.m[13] = ty;
    m.m[14] = tz;
    return m;
}

MCInline MCMatrix4 MCMatrix4MakeScale(float sx, float sy, float sz)
{
    MCMatrix4 m = MCMatrix4Identity;
    m.m[0] = sx;
    m.m[5] = sy;
    m.m[10] = sz;
    return m;
}

MCInline MCMatrix3 MCMatrix3MakeXAxisRotation(double degree)
{
    double SIN = sin(degree);
    double COS = cos(degree);
    return (MCMatrix3) {
        1,    0,   0,
        0,  COS, SIN,
        0, -SIN, COS
    };
}

MCInline MCMatrix3 MCMatrix3MakeYAxisRotation(double degree)
{
    double SIN = sin(degree);
    double COS = cos(degree);
    return (MCMatrix3) {
         COS, 0, -SIN,
           0, 1, 0,
         SIN, 0, COS
    };
}

MCInline MCMatrix3 MCMatrix3MakeZAxisRotation(double degree)
{
    double SIN = sin(degree);
    double COS = cos(degree);
    return (MCMatrix3) {
         COS, SIN, 0,
        -SIN, COS, 0,
           0,   0, 1
    };
}

MCInline MCMatrix3 MCMatrix4GetMatrix3(MCMatrix4 mat4)
{
    MCMatrix3 m = {
        mat4.m[0], mat4.m[1], mat4.m[2],
        mat4.m[4], mat4.m[5], mat4.m[6],
        mat4.m[8], mat4.m[9], mat4.m[10] };
    return m;
}

MCInline MCMatrix4 MCMatrix4FromMatrix3(MCMatrix3 mat3)
{
    MCMatrix4 m = {
        mat3.m[0], mat3.m[1], mat3.m[2], 0,
        mat3.m[3], mat3.m[4], mat3.m[5], 0,
        mat3.m[6], mat3.m[7], mat3.m[8], 0,
        0, 0, 0, 1
    };
    return m;
}

//MCInline MCMatrix3 MCMatrix3Rotate(MCMatrix3 matrix, float x, float y, float z)
//{
//    
//}

MCInline MCMatrix3 MCMatrix3Scale(MCMatrix3 matrix, float sx, float sy, float sz)
{
    MCMatrix3 m = {
        matrix.m[0] * sx, matrix.m[1] * sx, matrix.m[2] * sx,
        matrix.m[3] * sy, matrix.m[4] * sy, matrix.m[5] * sy,
        matrix.m[6] * sz, matrix.m[7] * sz, matrix.m[8] * sz };
    return m;
}

MCInline MCMatrix3 MCMatrix3Transpose(MCMatrix3 matrix)
{
    MCMatrix3 m = {
        matrix.m[0], matrix.m[3], matrix.m[6],
        matrix.m[1], matrix.m[4], matrix.m[7],
        matrix.m[2], matrix.m[5], matrix.m[8] };
    return m;
}

MCInline MCMatrix4 MCMatrix4Transpose(MCMatrix4 matrix)
{
    MCMatrix4 m = {
        matrix.m[0], matrix.m[4], matrix.m[8], matrix.m[12],
        matrix.m[1], matrix.m[5], matrix.m[9], matrix.m[13],
        matrix.m[2], matrix.m[6], matrix.m[10],matrix.m[14],
        matrix.m[3], matrix.m[7], matrix.m[11],matrix.m[15] };
    return m;
}

MCInline MCMatrix3 MCMatrix3Invert(MCMatrix3 matrix, MCBool* isInvertible) {
    float determinant = (matrix.m00 * (matrix.m11 * matrix.m22 - matrix.m12 * matrix.m21)) + (matrix.m01 * (matrix.m12 * matrix.m20 - matrix.m22 * matrix.m10)) + (matrix.m02 * (matrix.m10 * matrix.m21 - matrix.m11 *matrix.m20));
    
    if (isInvertible) {
        *isInvertible = (determinant != 0.0f)? true : false;
    }
    
    if (!(determinant != 0.0f)) {
        return MCMatrix3Identity;
    }
    
    return MCMatrix3Scale(MCMatrix3Transpose(matrix), determinant, determinant, determinant);
}

MCInline MCMatrix4 MCMatrix4Invert(MCMatrix4 matrix, MCBool* isInvertible) {
    MCMatrix3 m3 = MCMatrix4GetMatrix3(matrix);
    MCMatrix3 i3 = MCMatrix3Invert(m3, isInvertible);
    return MCMatrix4FromMatrix3(i3);
}

MCInline MCMatrix3 MCMatrix3InvertAndTranspose(MCMatrix3 matrix, void* isInvertible) {
    return MCMatrix3Transpose(MCMatrix3Invert(matrix, isInvertible));
}

MCInline MCMatrix4 MCMatrix4MakeLookAt(float eyeX, float eyeY, float eyeZ,
                                           float centerX, float centerY, float centerZ,
                                           float upX, float upY, float upZ)
{
    MCVector3 eye = { eyeX, eyeY, eyeZ };
    MCVector3 lka = { centerX, centerY, centerZ };
    MCVector3 upv = { upX, upY, upZ };
    
    //uvn is unit vector of xyz in view space
    MCVector3 v = MCVector3Normalize(upv);
    MCVector3 n = MCVector3Normalize(MCVector3Sub(eye, lka));
    MCVector3 u = MCVector3Normalize(MCVector3Cross(v, n));
    
    //-dot(u, eye) -dot(v, eye) -dot(z, eye)
    double dup = - eye.x * u.x - eye.y * u.y - eye.z * u.z;
    double dvp = - eye.x * v.x - eye.y * v.y - eye.z * v.z;
    double dnp = - eye.x * n.x - eye.y * n.y - eye.z * n.z;

    //column major
    MCMatrix4 m = {
        u.x, v.x, n.x, 0.0f,
        u.y, v.y, n.y, 0.0f,
        u.z, v.z, n.z, 0.0f,
        dup, dvp, dnp, 1.0f
    };
    
    return m;
}

MCInline MCMatrix4 MCMatrix4MakeLookAtByEulerAngle_EyeUp(MCVector3 lookat, double R, double fai, double tht,
                                                         MCVector3* eyeResult, MCVector3* upResult)
{
    //rotate around y-axis first then x-axis
    MCQuaternion qy = MCQuaternionFromAxisAngle(MCVector3Make(0, 1, 0), fai);
    MCQuaternion qx = MCQuaternionFromAxisAngle(MCVector3Make(1, 0, 0), tht);
    MCQuaternion qc = MCQuaternionGProduct(qy, qx);
    
    MCVector3 lat = lookat;
    MCVector3 eye = MCVector3RotateByQuaternion(MCVector3Make(0.0, R, 0.0), qc);
    MCVector3 up  = MCVector3RotateByQuaternion(MCVector3Make(0.0, 0.0, -1.0), qc);
    
    if (eyeResult) {
        *eyeResult = eye;
    }
    
    if (upResult) {
        *upResult  = up;
    }
    
//    return MCMatrix4MakeLookAt(eye.x, eye.y, eye.z,
//                               lat.x, lat.y, lat.z,
//                               up.x,  up.y,  up.z);
    
    MCMatrix4 rotation = MCMatrix4MakeLookAt(eye.x, eye.y, eye.z,
                                             0, 0, 0,
                                             up.x,  up.y,  up.z);
    
    //first translate then rotate
    return MCMatrix4Multiply(rotation, MCMatrix4MakeTranslation(lat.x, lat.y, lat.z));
}

//column major
MCInline MCMatrix4 MCMakeRotationMatrix4ByUVN(MCVector3 u, MCVector3 v, MCVector3 n)
{
    return (MCMatrix4) {
        u.x, v.x, n.x, 0,
        u.y, v.y, n.y, 0,
        u.z, v.z, n.z, 0,
        0, 0, 0, 1
    };
}

MCInline MCMatrix4 MCMatrix4MakeLookAtByEulerAngle(MCVector3 lookat, double R, double fai, double tht)
{
    return MCMatrix4MakeLookAtByEulerAngle_EyeUp(lookat, R, fai, tht, null, null);
}

MCInline MCVector3 MCGetEyeFromRotationMat4(MCMatrix4 mat4, double R)
{
    return (MCVector3) { mat4.m[2]*R, mat4.m[6]*R, mat4.m[10]*R };
}

MCInline MCVector3 MCGetTranslateFromCombinedMat4(MCMatrix4 mat4)
{
    return (MCVector3) { mat4.m[12], mat4.m[13], mat4.m[14] };
}

MCInline MCMatrix3 MCGetRotateFromCombinedMat4(MCMatrix4 mat4)
{
    return (MCMatrix3) {
        mat4.m[0], mat4.m[1], mat4.m[2],
        mat4.m[4], mat4.m[5], mat4.m[6],
        mat4.m[8], mat4.m[9], mat4.m[10]
    };
}

MCInline MCMatrix4 MCMatrix4CombineRT(MCMatrix3 R, MCVector3 T)
{
    return (MCMatrix4) {
        R.m[0], R.m[1], R.m[2], 0,
        R.m[3], R.m[4], R.m[5], 0,
        R.m[6], R.m[7], R.m[8], 0,
        T.v[0], T.v[1], T.v[2], 1,
    };
}

#endif
