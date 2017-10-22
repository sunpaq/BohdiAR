//
//  MC3DNode.c
//  monkcGame
//
//  Created by Sun YuLi on 16/2/20.
//  Copyright © 2016年 oreisoft. All rights reserved.
//

#include "MC3DNode.h"
#include "MCGLEngine.h"
#include "MCLinkedList.h"
#include "MCGLRenderer.h"

oninit(MC3DNode)
{
    if (init(MCItem)) {
        var(visible) = true;
        var(center) = MCVector3Make(0, 0, 0);
        
        var(transform) = MCMatrix4Identity;
        var(viewtrans) = MCMatrix4Identity;
        
        var(material) = null;
        var(diffuseTexture) = null;
        var(specularTexture)= null;
        var(zorder) = -1;
        
        var(children) = new(MCLinkedList);
        var(meshes) = new(MCLinkedList);
        
        return obj;
    }else{
        return null;
    }
}

method(MC3DNode, void, bye, voida)
{
    release(var(material));
    release(var(diffuseTexture));
    release(var(specularTexture));
    release(var(children));
    release(var(meshes));
}

method(MC3DNode, MC3DErrCode, addChild, MC3DNode* child)
{
    child->visible = false;
    MCLinkedList_addItem(var(children), (MCItem*)child);
    child->visible = true;
    return MC3DSuccess;
}

method(MC3DNode, MC3DErrCode, addChildAtIndex, MC3DNode* child, int index)
{
    child->visible = false;
    if (index < 0) {
        MCLinkedList_addItem(var(children), (MCItem*)child);
    } else {
        MCLinkedList_addItemAtIndex(var(children), index, (MCItem*)child);
    }
    child->visible = true;
    return MC3DSuccess;
}

method(MC3DNode, MC3DErrCode, removeChild, MC3DNode* child)
{
    child->visible = false;
    MCLinkedList_delItem(var(children), (MCItem*)child);
    return MC3DSuccess;
}

method(MC3DNode, void, copyChildrenFrom, MC3DNode* node)
{
    MCLinkedList_connectList(var(children), node->children);
}

method(MC3DNode, void, cleanUnvisibleChild, voida)
{
    MCLinkedListForEach(var(children),
                        MC3DNode* node = (MC3DNode*)item;
                        if (node != null && node->visible == false) {
                            MCLinkedList_delItem(var(children), (MCItem*)node);
                        })
}

method(MC3DNode, int, childCount, voida)
{
    return computed(obj->children, count);
}

method(MC3DNode, void, setAllVisible, MCBool visible)
{
    if (var(children)) {
        MCLinkedListForEach(var(children),
                            MC3DNode* node = (MC3DNode*)item;
                            if (node != null) {
                                node->visible = visible;
                            })
    }
}

method(MC3DNode, void, changeMatrial, MCMaterial* material)
{
    release(obj->material);
    obj->material = material;
}

method(MC3DNode, void, changeTexture, MCTexture* texture)
{
    release(obj->diffuseTexture);
    obj->diffuseTexture = texture;
}

method(MC3DNode, void, resetTransform, MCMatrix4* transform)
{
    if (transform) {
        obj->transform = *transform;
    } else {
        obj->transform = MCMatrix4Identity;
    }
}

method(MC3DNode, void, translateVec3, MCVector3* position, MCBool incremental)
{
    if (incremental) {
        obj->transform = MCMatrix4Multiply(MCMatrix4MakeTranslation(position->x, position->y, position->z), obj->transform);
    } else {
        obj->transform = MCMatrix4MakeTranslation(position->x, position->y, position->z);
    }
}

method(MC3DNode, void, scaleVec3, MCVector3* factors, MCBool incremental)
{
    if (incremental) {
        obj->transform = MCMatrix4Multiply(MCMatrix4MakeScale(factors->x, factors->y, factors->z), obj->transform);
    } else {
        obj->transform = MCMatrix4MakeScale(factors->x, factors->y, factors->z);
    }
}

method(MC3DNode, void, rotateMat3, float mat3[9], MCBool incremental)
{
    if (mat3) {
        MCMatrix3 m3 = {0};
        for (int i=0; i<9; i++) {
            m3.m[i] = mat3[i];
        }
        if (incremental) {
            obj->transform = MCMatrix4Multiply(MCMatrix4FromMatrix3(m3), obj->transform);
        } else {
            obj->transform = MCMatrix4FromMatrix3(m3);
        }
    }
}

method(MC3DNode, void, rotateMat4, float mat4[16], MCBool incremental)
{
    if (mat4) {
        MCMatrix4 m4 = {0};
        for (int i=0; i<16; i++) {
            m4.m[i] = mat4[i];
        }
        if (incremental) {
            obj->transform = MCMatrix4Multiply(m4, obj->transform);
        } else {
            obj->transform = m4;
        }
    }
}

method(MC3DNode, void, update, MCGLContext* ctx)
{
    //update children
    MCLinkedListForEach(var(children),
                        MC3DNode* node = (MC3DNode*)item;
                        if (node != null && node->visible != false) {
                            ff(node, update, ctx);
                            //ff(node, update, ctx);
                        })
}

method(MC3DNode, void, draw, MCGLContext* ctx)
{
    //draw self
    //if (obj->visible) {
        MCGLContext_activateShaderProgram(ctx, 0);
        MCGLUniform f;
        
        //scale translate
        MCMatrix4 viewModel = MCMatrix4Multiply(var(viewtrans), var(transform));

        if (!MCMatrix4Equal(&MCMatrix4Identity, &viewModel)) {
            f.data.mat4 = viewModel;
            MCGLContext_updateUniform(ctx, model_model, f.data);
        }
    
        MCMatrix3 nor = MCMatrix3InvertAndTranspose(MCMatrix4GetMatrix3(var(transform)), NULL);
        f.data.mat3 = nor;
        MCGLContext_updateUniform(ctx, model_normal, f.data);
        
        //material
        if (obj->material != null) {
            if (obj->material->hidden == 1) {
                return;
            }
            obj->material->dataChanged = true;
            MCMaterial_prepareMatrial(obj->material, ctx);
        }
        
        //draw self texture
        if (obj->diffuseTexture != null) {
            glUniform1i(glGetUniformLocation(ctx->pid, "usetexture"), true);
        } else {
            glUniform1i(glGetUniformLocation(ctx->pid, "usetexture"), false);
        }
        
        //batch setup
        MCGLContext_setUniforms(ctx, 0);
        
        //draw self meshes
        MCLinkedListForEach(var(meshes),
                            MCMesh* mesh = (MCMesh*)item;
                            if (mesh != null) {
                                mesh->diffuseTextureRef  = obj->diffuseTexture;
                                mesh->specularTextureRef = obj->specularTexture;
                                MCMesh_prepareMesh(mesh, ctx);
                                MCMesh_drawMesh(mesh, ctx);
                            })
    //}
    
    //draw children
    MCLinkedListForEach(var(children),
                        MC3DNode* node = (MC3DNode*)item;
                        if (node != null && node->visible != false) {
                            ff(node, draw, ctx);
                        })
    
    //ff(ctx, printUniforms, 0);
}

method(MC3DNode, void, hide, voida)
{
    var(visible) = false;
}

method(MC3DNode, void, show, voida)
{
    var(visible) = true;
}

onload(MC3DNode)
{
    if (load(MCItem)) {
        binding(MC3DNode, void, bye, voida);
        binding(MC3DNode, void, addChild, MC3DNode* child);
        binding(MC3DNode, MC3DErrCode, addChildAtIndex, MC3DNode* child, int index);
        binding(MC3DNode, void, removeChild, MC3DNode* child);
        binding(MC3DNode, void, copyChildrenFrom, MC3DNode* node);
        binding(MC3DNode, void, cleanUnvisibleChild, voida);
        binding(MC3DNode, int, childCount, voida);
        binding(MC3DNode, void, setAllVisible, MCBool visible);
        binding(MC3DNode, void, changeMatrial, MCMaterial* material);
        binding(MC3DNode, void, changeTexture, MCTexture* texture);
        binding(MC3DNode, void, resetTransform, MCMatrix4* transform);
        binding(MC3DNode, void, translateVec3, MCVector3* position, MCBool incremental);
        binding(MC3DNode, void, rotateMat3, float mat3[9], MCBool incremental);
        binding(MC3DNode, void, rotateMat4, float mat4[16], MCBool incremental);
        binding(MC3DNode, void, scaleVec3, MCVector3* factors, MCBool incremental);
        binding(MC3DNode, void, update, voida);
        binding(MC3DNode, void, draw, voida);
        binding(MC3DNode, void, hide, voida);
        binding(MC3DNode, void, show, voida);
        return cla;
    }else{
        return null;
    }
}
