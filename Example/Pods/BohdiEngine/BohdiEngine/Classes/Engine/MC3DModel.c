//
//  MC3DModel.c
//  monkcGame
//
//  Created by Sun YuLi on 16/3/14.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MC3DModel.h"
#include "BAObjParser.h"
#include "BATrianglization.h"
#include "MCMath.h"
#include "MCLinkedList.h"

compute(MC3DFrame, frame)
{
    as(MC3DModel);
    MC3DFrame allframe = (MC3DFrame){0,0,0,0,0,0};
    
    MCLinkedListForEach(sobj->meshes,
        MCMesh* m = (MCMesh*)item;
        if (m != null) {
            MC3DFrame mf = m->Frame;
            //MAX
            MCMath_accumulateMaxd(&allframe.xmax, mf.xmax);
            MCMath_accumulateMaxd(&allframe.ymax, mf.ymax);
            MCMath_accumulateMaxd(&allframe.zmax, mf.zmax);
            //MIN
            MCMath_accumulateMind(&allframe.xmin, mf.xmin);
            MCMath_accumulateMind(&allframe.ymin, mf.ymin);
            MCMath_accumulateMind(&allframe.zmin, mf.zmin);
        }
    )
    
    MCLinkedListForEach(sobj->children,
        MC3DModel* m = (MC3DModel*)item;
        if (m != null) {
            MC3DFrame mf = computed(m, frame);
            //MAX
            MCMath_accumulateMaxd(&allframe.xmax, mf.xmax);
            MCMath_accumulateMaxd(&allframe.ymax, mf.ymax);
            MCMath_accumulateMaxd(&allframe.zmax, mf.zmax);
            //MIN
            MCMath_accumulateMind(&allframe.xmin, mf.xmin);
            MCMath_accumulateMind(&allframe.ymin, mf.ymin);
            MCMath_accumulateMind(&allframe.zmin, mf.zmin);
        }
    )
    
    obj->lastSavedFrame = allframe;
    return allframe;
}

compute(double, maxlength)
{
    as(MC3DModel);
    double max = 0;
    MC3DFrame frame = cpt(frame);
    MCMath_accumulateMaxd(&max, frame.xmax - frame.xmin);
    MCMath_accumulateMaxd(&max, frame.ymax - frame.ymin);
    MCMath_accumulateMaxd(&max, frame.zmax - frame.zmin);
    return max;
}

compute(MCVector3, center)
{
    as(MC3DModel);
    MC3DFrame f = cpt(frame);
    return MCVector3Make((f.xmax-f.xmin)/2.0,
                         (f.ymax-f.ymin)/2.0,
                         (f.zmax-f.zmin)/2.0);
}

oninit(MC3DModel)
{
    if (init(MC3DNode)) {
        obj->tag = -1;
        obj->name[0] = NUL;
        
        obj->defaultColor = (MCColorf){0.9, 0.9, 0.9, 1.0};
        obj->defaultExtension = "obj";
        obj->textureOnOff = false;
        obj->fitted = false;
        
        obj->frame = frame;
        obj->maxlength = maxlength;
        obj->center = center;
        
        obj->lastSavedFrame = (MC3DFrame){0,0,0,0,0,0};
        return obj;
    }else{
        return null;
    }
}

method(MC3DModel, void, bye, voida)
{
    MC3DNode_bye(sobj, 0);
}

function(void, meshLoadFaceElement, MCMesh* mesh, BAObjData* buff, BAFaceElement e, size_t offset, MCColorf color)
{
    MCVector3 v, n;
    MCVector2 t;

    if (e.vi <= 0) {
        error_log("MC3DFileParser: invalide vertex data!\n");
        return;
    }else{
        v = buff->vertexbuff[e.vi-1];
    }
    
    if (buff->shouldCalculateNormal) {
        n = MCNormalOfTriangle(buff->vertexbuff[e.vi], buff->vertexbuff[e.vi+1], buff->vertexbuff[e.vi+2]);
        mesh->calculatedNormal = true;
    }
    
    if (e.ni <= 0) {
        if (!buff->shouldCalculateNormal) {
            n = MCNormalOfTriangle(buff->vertexbuff[e.vi], buff->vertexbuff[e.vi+1], buff->vertexbuff[e.vi+2]);
            mesh->calculatedNormal = true;
        }
    }else{
        n = MCVector3From4(buff->normalbuff[e.ni-1]);
    }
    
    if (e.ti <= 0) {
        //error_log("MC3DFileParser: empty texcoord data, set to 0!");
        t = (MCVector2){0.0,0.0};
    }else{
        t = buff->texcoorbuff[e.ti-1];
    }
    
    //3D frame max
    MCMath_accumulateMaxd(&buff->Frame.xmax, v.x);
    MCMath_accumulateMaxd(&buff->Frame.ymax, v.y);
    MCMath_accumulateMaxd(&buff->Frame.zmax, v.z);
    //3D frame min
    MCMath_accumulateMind(&buff->Frame.xmin, v.x);
    MCMath_accumulateMind(&buff->Frame.ymin, v.y);
    MCMath_accumulateMind(&buff->Frame.zmin, v.z);
    
    MCMesh_setVertex(mesh, (GLuint)offset, &(MCMeshVertexData){
        v.x, v.y, v.z,
        n.x, n.y, n.z,
        color.R.f, color.G.f, color.B.f,
        t.x, t.y
        //0,0
    });
}

function(MCMesh*, createMeshWithBATriangles, BATriangle* triangles, size_t tricount, BAObjData* buff, MCColorf color)
{
    MCMesh* mesh = MCMesh_initWithDefaultVertexAttributes(new(MCMesh), (GLsizei)tricount*3);
    
    for (size_t i=0; i<tricount; i++) {
        size_t offset = i * 33;
        meshLoadFaceElement(null, mesh, buff, triangles[i].e1, offset+0, color);
        meshLoadFaceElement(null, mesh, buff, triangles[i].e2, offset+11, color);
        meshLoadFaceElement(null, mesh, buff, triangles[i].e3, offset+22, color);
    }
    
    //normalize normal
    MCMesh_normalizeNormals(mesh, 0);
    
    //frame
    for (int i=0; i<6; i++) {
        mesh->Frame.m[i] = buff->Frame.m[i];
    }
    
    return mesh;
}

function(void, setDefaultMaterialForNode, MC3DNode* node)
{
    if (node) {
        node->material->ambientLightColor  = MCVector3Make(0.5, 0.5, 0.5);
        node->material->diffuseLightColor  = MCVector3Make(0.5, 0.5, 0.5);
        node->material->specularLightColor = MCVector3Make(0.5, 0.5, 0.5);
        node->material->specularLightPower = 16.0f;
        node->material->dissolve           = 1.0f;
        node->material->hidden             = 0;
        node->material->illum              = 2;
        
        MCStringFill(node->material->tag, "Default");
        node->material->dataChanged = true;
    }
}

function(void, setMaterialForNode, MC3DNode* node, BAMaterial* mtl)
{
    if (mtl && mtl->name[0] != NUL) {
        MCVector3 ambient  = BAMaterialLightColor(mtl, Ambient);
        MCVector3 diffuse  = BAMaterialLightColor(mtl, Diffuse);
        MCVector3 specular = BAMaterialLightColor(mtl, Specular);
        
        node->material->ambientLightColor  = ambient;
        node->material->diffuseLightColor  = diffuse;
        node->material->specularLightColor = specular;
        node->material->specularLightPower = mtl->specularExponent;
        node->material->dissolve           = mtl->dissolveFactor;
        node->material->hidden             = mtl->hidden;
        node->material->illum              = mtl->illumModelNum;
        
        MCStringFill(node->material->tag, mtl->name);
        node->material->dataChanged = true;
    }else{
        setDefaultMaterialForNode(null, node);
    }
}

function(void, setTextureForNode, MC3DNode* node, BAObjData* buff, BAMesh* mesh)
{
    //object texture
    if (mesh->object[0]) {
        BATexture* tex = BAFindTextureByAttachedObject(buff->mtllib_list, mesh->object);
        if (tex && tex->filename[0]) {
            node->diffuseTexture = MCTexture_initWithFileName(new(MCTexture), tex->filename);
            return;
        }
    }
    
    //group texture
    if (mesh->group[0]) {
        BATexture* tex = BAFindTextureByAttachedGroup(buff->mtllib_list, mesh->group);
        if (tex && tex->filename[0]) {
            node->diffuseTexture = MCTexture_initWithFileName(new(MCTexture), tex->filename);
            return;
        }
    }
    
    //material texture
    BAMaterial* mtl = mesh->usemtl;
    if (mtl) {
        if (mtl->diffuseMapName[0]) {
            node->diffuseTexture = MCTexture_initWithFileName(new(MCTexture), mtl->diffuseMapName);
        }
        if (mtl->specularMapName[0]) {
            node->specularTexture = MCTexture_initWithFileName(new(MCTexture), mtl->specularMapName);
        }
    }

}

//size_t fcursor, BAMaterial* mtl, size_t facecount,
function(MC3DModel*, initModel, BAObjData* buff, BAMesh* bamesh, MCColorf color)
{
    MC3DModel* model = (MC3DModel*)any;
    if (model && bamesh) {
        BAFace* faces = &buff->facebuff[bamesh->startFaceCount];
        BAMaterial* mtl = bamesh->usemtl;
        
        BATriangle* triangles = createTrianglesBuffer(faces, bamesh->totalFaceCount);
        size_t tricount = trianglization(triangles, faces, bamesh->totalFaceCount, buff->vertexbuff);
        MCMesh* mesh = createMeshWithBATriangles(null, triangles, tricount, buff, color);
        
        model->Super.material = new(MCMaterial);
        model->Super.diffuseTexture  = null;
        model->Super.specularTexture = null;
        MCLinkedList_addItem(model->Super.meshes, (MCItem*)mesh);
        
        //set mtl
        if (mtl) {
            setMaterialForNode(null, &model->Super, mtl);


        }else{
            setDefaultMaterialForNode(null, &model->Super);
        }
        
        //set texture
        setTextureForNode(null, &model->Super, buff, bamesh);
        
        //set name
        MCStringFill(model->name, buff->name);
        releaseTrianglesBuffer(triangles);
        return model;
    }else{
        return null;
    }
}

method(MC3DModel, MC3DModel*, initWithFilePathColor, const char* path, MCColorf color)
{
    debug_log("MC3DModel - initWithFilePathColor: %s\n", path);
    
    BAObjMeta Meta;
    BAObjData* buff = BAObjDataNewWithFilepath(path, &Meta);
    if (!buff) {
        error_log("MC3DModel initWithFilePathColor BAObjNew() failed exit\n");
        exit(-1);
    }
    debug_log("MC3DModel - BAObjNew success: %s\n", path);
    
    BAObjDumpInfo(buff);

    //separate model by mesh
    if (Meta.mesh_count <= 1) {
        BAMesh* m = &buff->meshbuff[0];
        if (m) {
            MC3DModel* model = new(MC3DModel);
            initModel(model, buff, m, color);
            MCLinkedList_addItem(obj->Super.children, (MCItem*)model);
        }
    } else {
        for (size_t i=0; i<Meta.mesh_count; i++) {
            BAMesh* m = &buff->meshbuff[i];
            if (m) {
                MC3DModel* model = new(MC3DModel);
                initModel(model, buff, m, color);
                MCLinkedList_addItem(obj->Super.children, (MCItem*)model);
            }
        }
    }
    
    BAObjRelease(buff);
    cpt(frame);
    
    debug_log("MC3DModel - return obj: %s\n", path);
    return obj;
}

method(MC3DModel, MC3DModel*, initWithFilePath, const char* path)
{
    return MC3DModel_initWithFilePathColor(obj, path, obj->defaultColor);
}

method(MC3DModel, MC3DModel*, initWithFileNameColor, const char* name, MCColorf color)
{
    if (obj) {
        MCStringFill(obj->name, name);
        char path[PATH_MAX] = {0};
        if (MCFileGetPath(name, path)) {
            return null;
        }
        debug_log("MC3DModel - find path: %s\n", path);
        return MC3DModel_initWithFilePathColor(obj, path, color);
    }else{
        return null;
    }
}

method(MC3DModel, MC3DModel*, initWithFileName, const char* name)
{
    return MC3DModel_initWithFileNameColor(obj, name, obj->defaultColor);
}

method(MC3DModel, void, translateToOrigin, voida)
{
    MCVector3 center = cpt(center);
    MCVector3 rcenter = MCVector3Reverse(center);
    MC3DNode_translateVec3(sobj, &rcenter, false);
}

method(MC3DModel, void, rotateAroundSelfAxisX, double ccwRadian)
{
    MCMatrix4 RX = MCMatrix4FromMatrix3(MCMatrix3MakeXAxisRotation(ccwRadian));
    sobj->transform = MCMatrix4Multiply(sobj->transform, RX);
}

method(MC3DModel, void, rotateAroundSelfAxisY, double ccwRadian)
{
    MCMatrix4 RY = MCMatrix4FromMatrix3(MCMatrix3MakeYAxisRotation(ccwRadian));
    sobj->transform = MCMatrix4Multiply(sobj->transform, RY);
}

method(MC3DModel, void, rotateAroundSelfAxisZ, double ccwRadian)
{
    MCMatrix4 RZ = MCMatrix4FromMatrix3(MCMatrix3MakeZAxisRotation(ccwRadian));
    sobj->transform = MCMatrix4Multiply(sobj->transform, RZ);
}

method(MC3DModel, void, resizeToFit, double maxsize)
{
    if (var(fitted) == false) {
        double maxl  = cpt(maxlength);
        double scale = maxsize / maxl;
        MCVector3 scaleVec = MCVector3Make(scale, scale, scale);
        MC3DNode_scaleVec3(sobj, &scaleVec, false);
        var(fitted) = true;
        debug_log("MC3DModel - model maxlength=%lf scale=%lf\n", maxl, scale);
    }
}

//override
method(MC3DModel, void, update, MCGLContext* ctx)
{
    MC3DNode_update(sobj, ctx);
}

method(MC3DModel, void, draw, MCGLContext* ctx)
{
    MC3DNode_draw(sobj, ctx);
}

onload(MC3DModel)
{
    if (load(MC3DNode)) {
        binding(MC3DModel, void, bye, voida);
        binding(MC3DModel, MC3DModel*, initWithFilePath, const char* path);
        binding(MC3DModel, MC3DModel*, initWithFileName, const char* name);
        binding(MC3DModel, MC3DModel*, initWithFilePathColor, const char* path, MCColorf color);
        binding(MC3DModel, MC3DModel*, initWithFileNameColor, const char* name, MCColorf color);
        binding(MC3DModel, void, rotateAroundSelfAxisX, double ccwRadian);
        binding(MC3DModel, void, rotateAroundSelfAxisY, double ccwRadian);
        binding(MC3DModel, void, rotateAroundSelfAxisZ, double ccwRadian);
        binding(MC3DModel, void, resizeToFit, double maxsize);
        //override
        binding(MC3DModel, void, update, MCGLContext* ctx);
        binding(MC3DModel, void, draw, MCGLContext* ctx);
        binding(MC3DModel, void, translateToOrigin, voida);
        return cla;
    }else{
        return null;
    }
}

