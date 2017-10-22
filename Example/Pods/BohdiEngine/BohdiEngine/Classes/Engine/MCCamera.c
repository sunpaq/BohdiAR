#include "MCCamera.h"
#include "MCGLRenderer.h"

compute(double, Radius);
compute(MCMatrix3, normal);
compute(MCMatrix4, viewMatrix);
compute(MCMatrix4, projectionMatrix);
compute(MCVector3, currentPosition);
compute(MCMatrix3, rotationMat3);

oninit(MCCamera)
{
    if (init(MC3DNode)) {
        var(ratio) = MCRatioHDTV16x9;
        var(field_of_view) = MCLensIphoneVideoViewAngle;
        var(depth_of_field) = 100;

        //local spherical coordinate
        var(R_value) = 100;
        var(R_percent) = 1.0;
        var(tht) = 90;
        var(fai) = 0;

        var(lookat) = MCVector3Make(0,0,0);
        var(eye) = MCVector3Make(0.0,obj->R_value,0.0);
        var(up)  = MCVector3Make(0.0,0.0,-1.0);
        
        var(Radius) = Radius;
        var(normal) = normal;
        var(viewMatrix) = viewMatrix;
        var(projectionMatrix) = projectionMatrix;
        var(currentPosition) = currentPosition;
        //transform
        var(rotationMat3) = rotationMat3;
        
        var(isReverseMovement) = false;
        var(isLockRotation) = false;
        
        var(rotateMode)= MCCameraRotateAroundModelManual;
        
        return obj;
    }else{
        return null;
    }
}

method(MCCamera, void, bye, voida)
{
    MC3DNode_bye(sobj, 0);
}

method(MCCamera, void, printDebugInfo, voida)
{
    debug_log("MCCamera: lookat=%.2f/%.2f/%.2f R_value=%.2f R_percent=%.3f\n",
              obj->lookat.x, obj->lookat.y, obj->lookat.z, obj->R_value, obj->R_percent);
}

compute(double, Radius)
{
    as(MCCamera);
    return (obj->R_value * obj->R_percent);
}

compute(MCMatrix3, normal)
{
    as(MCCamera);
    MCMatrix3 nor = MCMatrix3InvertAndTranspose((MCMatrix3)MCMatrix4GetMatrix3(cpt(viewMatrix)), NULL);
    return nor;
}

/*
 Initialize Status:
 
 view space coordinate orientation equal to device attitude (iphone on the table, x-right z-up)
 world space coordinate orientation equal monitor attitude (LCD on the desk, x-right -z forward)
 */
compute(MCMatrix4, viewMatrix)
{
    as(MCCamera);
    double r = cpt(Radius);
    if (obj->rotateMode == MCCameraRotateAroundModelManual) {
        MCCamera_transformSelfByEularAngle(obj, obj->lookat, cpt(Radius), obj->fai, obj->tht);
    }
    else if (obj->rotateMode == MCCameraRotateAroundModelByGyroscope) {
        sobj->transform = MCMatrix4MakeLookAt(0, 0, r, 0, 0, 0, 0, 1, 0);
    }
    else if (obj->rotateMode == MCCameraRotateAroundModelByGyroscopeReverse) {
        sobj->transform = MCMatrix4MakeLookAt(0, 0, r, 0, 0, 0, 0, 1, 0);
        sobj->viewtrans = MCMatrix4Invert(sobj->viewtrans, null);
    }
    else if (obj->rotateMode == MCCameraFixedAtOrigin) {
        sobj->transform = MCMatrix4Identity;
        sobj->viewtrans = MCMatrix4Identity;
    }
    //default
    else {
        //do nothing
    }
    MCMatrix4 view = MCMatrix4Multiply(sobj->transform, sobj->viewtrans);
    return view;
}

compute(MCMatrix4, projectionMatrix)
{
    as(MCCamera);
    double far  = cpt(Radius) + var(depth_of_field);
    double near = cpt(Radius) - var(depth_of_field);
    if (near <= 0) {
        near = MCLensIphone29mm;
    }
    return MCMatrix4MakePerspective(MCDegreesToRadians(obj->field_of_view),
                                    var(ratio),
                                    near,
                                    far);
}

compute(MCVector3, currentPosition)
{
    as(MCCamera);
    if (obj->rotateMode != MCCameraRotateAroundModelManual) {
        double r = cpt(Radius);
        obj->eye = MCGetEyeFromRotationMat4(sobj->viewtrans, r);
        obj->R_value = MCVector3Length(MCGetTranslateFromCombinedMat4(sobj->transform));
        obj->R_percent = 1.0;
    }
    return obj->eye;
}

compute(MCMatrix3, rotationMat3)
{
    as(MCCamera);
    return MCMatrix4GetMatrix3(sobj->transform);
}

method(MCCamera, MCCamera*, initWithWidthHeight, unsigned width, unsigned height)
{
    //setting camera
    obj->ratio = MCRatioMake(width, height);
    obj->R_value = 5;
    return obj;
}

method(MCCamera, void, reset, voida)
{
    init(MCCamera);
}

method(MCCamera, void, transformWorld, MCMatrix4* mat4)
{
    sobj->viewtrans = *mat4;
}

method(MCCamera, void, transformSelf, MCMatrix4* mat4)
{
    sobj->transform = *mat4;
}

method(MCCamera, void, transformSelfByEularAngle, MCVector3 lookat, double R, double fai, double tht)
{
    sobj->transform = MCMatrix4MakeLookAtByEulerAngle_EyeUp(obj->lookat, cpt(Radius),
                                                            obj->fai, obj->tht,
                                                            &obj->eye, &obj->up);
}

//override
method(MCCamera, void, update, MCGLContext* ctx)
{
    MCGLUniformData data;
    
    data.mat4 = cpt(viewMatrix);
    MCGLContext_updateUniform(ctx, view_view, data);
    
    data.mat4 = cpt(projectionMatrix);
    MCGLContext_updateUniform(ctx, view_projection, data);
}

method(MCCamera, void, move, MCFloat deltaFai, MCFloat deltaTht)
{
    if (var(isLockRotation) == true) {
        return;
    }
    //angle with x+
    if (var(isReverseMovement)) {
        obj->fai += deltaFai.f;   //Left
        obj->tht += deltaTht.f;   //Up
    }else{
        obj->fai -= deltaFai.f;   //Left
        obj->tht -= deltaTht.f;   //Up
    }
}

method(MCCamera, void, fucus, MCFloat deltaX, MCFloat deltaY)
{
    //angle with x+
    if (var(isReverseMovement)) {
        obj->lookat.x += deltaX.f;
        obj->lookat.y += deltaY.f;
    }else{
        obj->lookat.x -= deltaX.f;
        obj->lookat.y -= deltaY.f;
    }
}

method(MCCamera, void, pull, MCFloat deltaR)
{
    obj->R_value += deltaR.f;
}

method(MCCamera, void, distanceScale, MCFloat scale)
{
    obj->R_percent = scale.f;
}

method(MCCamera, void, setRotationMat3, float mat3[9])
{
    MCMatrix4 m4 = (MCMatrix4) {
        mat3[0], mat3[1], mat3[2], 0,
        mat3[3], mat3[4], mat3[5], 0,
        mat3[6], mat3[7], mat3[8], 0,
        0, 0, 0, 1
    };
    sobj->viewtrans = m4;
}

onload(MCCamera)
{
    if (load(MC3DNode)) {
        binding(MCCamera, void, bye, voida);
        binding(MCCamera, MCCamera*, initWithWidthHeight, unsigned width, unsigned height);
        binding(MCCamera, void, transformWorld, MCMatrix4* mat4);
        binding(MCCamera, void, transformSelf, MCMatrix4* mat4);
        binding(MCCamera, void, transformSelfByEularAngle, MCVector3 lookat, double R, double fai, double tht);
        binding(MCCamera, void, move, MCFloat deltaFai, MCFloat deltaTht);
        binding(MCCamera, void, fucus, MCFloat deltaX, MCFloat deltaY);
        binding(MCCamera, void, pull, MCFloat deltaR);
        binding(MCCamera, void, reset, MCBool updateOrNot);
        binding(MCCamera, void, update);
        binding(MCCamera, void, distanceScale, MCFloat scale);
        binding(MCCamera, void, setRotationMat3, float mat3[9]);
        binding(MCCamera, void, printDebugInfo, voida);
        return cla;
    }else{
        return null;
    }
}

