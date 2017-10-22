//
//  MCGLShader.c
//  monkcGame
//
//  Created by SunYuLi on 15/11/22.
//  Copyright © 2015年 oreisoft. All rights reserved.
//

#include "MCGLRenderer.h"
#include "MCGLEngine.h"
#include "MC3DBase.h"
#include "MCIO.h"

static const char* VCODE = S(
//version is specified in MCGLContext
precision highp float;
precision mediump int;

//vertex attributes
layout (location=0) in vec4 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 color;
layout (location=3) in vec2 texcoord;

//Android GLSL doesn't support struct
uniform mat4 view_view;
uniform mat4 view_projection;
uniform vec3 view_position;

uniform mat4 model_model;
uniform mat3 model_normal;

//varying variables use to pass value between vertex & fragment shader
out vec3 vertexcolor;
out vec2 texturecoord;
out vec3 calculatedNormal;
out vec3 modelPosition;
out vec3 viewPosition;

void main()
{
    //Vertex Color
    vertexcolor = color;
    
    //Texture Coordinate
    texturecoord = texcoord;
    
    //Normal fix the non-uniform scale issue
    calculatedNormal = normalize(mat3(transpose(inverse(model_model))) * normal);
    //calculatedNormal = normalize(model.normal * normal);
    //calculatedNormal = normalize(normal);
    
    //Eye normal
    //eyeNormal = normalize(model.normal * normal);
    
    //Specular Light
    modelPosition = vec3(model_model * position);
    viewPosition  = view_position;
    
    //Position
    gl_Position = view_projection * view_view * model_model * position;
}
);//VCODE END

static const char* FCODE = S(
//version is specified in MCGLContext
precision highp sampler3D;
precision highp float;
precision lowp int;
const float Epsilon = 0.0000001;

//varying variables use to pass value between vertex & fragment shader
in vec3 vertexcolor;
in vec2 texturecoord;
in vec3 calculatedNormal;
in vec3 modelPosition;
in vec3 viewPosition;

out vec4 FragColor;

//Android GLSL doesn't support struct
uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;
uniform vec3 light_color;
uniform vec3 light_position;

uniform vec3  material_ambient;
uniform vec3  material_diffuse;
uniform vec3  material_specular;
uniform float material_dissolve;
uniform float material_shininess;

//texture sampling must in fragment shader
uniform sampler3D diffuse_sampler3d;
uniform sampler2D diffuse_sampler;
uniform sampler2D specular_sampler;

//uniforms not in context
uniform bool usetexture;
uniform bool usetexture3d;

uniform int illum;

bool floatEqual(float A, float B)
{
    if (abs(A-B) < Epsilon) {
        return true;
    }
    return false;
}
                             
vec3 halfwayVector(vec3 L, vec3 V)
{
    return (L+V)/abs(L+V);
}
                             
vec3 reflectVector(vec3 L, vec3 N)
{
    //    float dotNL = max(0.0,dot(N,L));
    //    return 2.0*dotNL*N - L;
    return reflect(-L,N);
}

//doen't contains Ka*Ia
vec3 lambertianShading(vec3 Kd, vec3 Id, vec3 L, vec3 N)
{
    float dotNL = max(0.0,dot(N,L));
    return Kd*Id*dotNL;
}

//doen't contains Ka*Ia
vec3 phongShading(vec3 Kd, vec3 Ks, vec3 Id, vec3 Is, float Es, vec3 L, vec3 N, vec3 V, bool simplified)
{
    //Lambertian shading and Blinn's interpretation of Phong's specular illumination model (BLIN77)
    float dotNL = max(0.0,dot(N,L));
    if (simplified == true) {
        vec3 H = halfwayVector(L,V);
        float dotNH = max(0.0,dot(N,H));
        if (dotNH > 0.0 && Es > 0.0) {
            return Kd*Id*dotNL + Ks*Is*pow(dotNH,Es);
        } else {
            return Kd*Id*dotNL;
        }
    }
    else {
        vec3 R = reflectVector(L,N);
        float dotVR = max(0.0,dot(V,R));
        if (dotVR > 0.0 && Es > 0.0) {
            return Kd*Id*dotNL + Ks*Is*pow(dotVR,Es);
        } else {
            return Kd*Id*dotNL;
        }
    }
}
                             
//doen't contains Ka*Ia
vec3 phongShading_withIr(vec3 Kd, vec3 Ks, vec3 Id, vec3 Is, float Es, vec3 L, vec3 N, vec3 V, bool simplified, vec3 Ir)
{
    //Lambertian shading and Blinn's interpretation of Phong's specular illumination model (BLIN77)
    float dotNL = max(0.0,dot(N,L));
    if (simplified == true) {
        vec3 H = halfwayVector(L,V);
        float dotNH = max(0.0,dot(N,H));
        if (dotNH > 0.0 && Es > 0.0) {
            return Kd*Id*dotNL + Ks*(Is*pow(dotNH,Es) + Ir);
        } else {
            return Kd*Id*dotNL;
        }
    }
    else {
        vec3 R = reflectVector(L,N);
        float dotVR = max(0.0,dot(V,R));
        if (dotVR > 0.0 && Es > 0.0) {
            return Kd*Id*dotNL + Ks*(Is*pow(dotVR,Es) + Ir);
        } else {
            return Kd*Id*dotNL;
        }
    }
}
                             
void main()
{
    //Color Output
    vec3 color = material_diffuse;
    float alpha = 1.0;
    if (usetexture == true) {
        vec4 texcolor = texture(diffuse_sampler, vec2(texturecoord.x, 1.0-texturecoord.y));
        //vec4 texcolor = texture(diffuse_sampler, texturecoord);
        alpha = texcolor.a;
        color = texcolor.rgb;
    }
    
    //Illuminate Mode
    vec3 Ka = material_ambient;
    vec3 Ia = light_ambient * light_color;
    
    vec3 Kd = color;
    vec3 Ks = material_specular;
    vec3 Id = light_diffuse * light_color;
    vec3 Is = light_specular * light_color;
    
    float Es = material_shininess;
    
    vec3 L  = normalize(light_position - modelPosition);
    vec3 N  = calculatedNormal;
    vec3 V  = normalize(viewPosition - modelPosition);
    
    if (illum == 0) {
        //color = Kd
        FragColor = vec4(Kd, alpha);
        return;
    }
    else if (illum == 1) {
        //Lambertian shading
        FragColor = vec4(Ka*Ia+lambertianShading(Kd,Id,L,N), alpha);
        return;
    }
    else if (illum == 2) {
        //Lambertian shading and Blinn's interpretation of Phong's specular illumination model (BLIN77)
        FragColor = vec4(Ka*Ia+phongShading(Kd,Ks,Id,Is,Es,L,N,V,false), alpha);
        return;
    }
    else if (illum == 3) {
        //Ir = (intensity of reflection map) + (ray trace)
        vec3 Ir = texture(specular_sampler, vec2(texturecoord.x, 1.0-texturecoord.y)).rgb;
        FragColor = vec4(Ka*Ia+phongShading_withIr(Kd,Ks,Id,Is,Es,L,N,V,false, Ir), alpha);
        return;
    }
    else if (illum == 4) {
        //glass
        //Ir = (intensity of reflection map) + (ray trace)
        vec3 Ir = vec3(0.0,0.0,0.0);
        FragColor = vec4(Ka*Ia+phongShading_withIr(Kd,Ks,Id,Is,Es,L,N,V,true, Ir), 0.1);
        return;
    }
    else if (illum == 5) {
        
    }
    else if (illum == 6) {
        
    }
    else if (illum == 7) {
        
    }
    else if (illum == 8) {//3 without ray trace
        //Ir = (intensity of reflection map)
        vec3 Ir = vec3(0.0,0.0,0.0);
        FragColor = vec4(Ka*Ia+phongShading_withIr(Kd,Ks,Id,Is,Es,L,N,V,true, Ir), alpha);
        return;
    }
    else if (illum == 9) {//4 without ray trace
        //glass
        //Ir = (intensity of reflection map)
        vec3 Ir = texture(specular_sampler, vec2(texturecoord.x, 1.0-texturecoord.y)).rgb;
        FragColor = vec4(Ka*Ia+phongShading_withIr(Kd,Ks,Id,Is,Es,L,N,V,true, Ir), 0.1);
        return;
    }
    else if (illum == 10) {
        //color = Pixel color. The pixel color of a shadowmatte material is always black.
        FragColor = vec4(0.0,0.0,0.0,1.0);
        return;
    }
    else {
        //Lambertian shading
        FragColor = vec4(Ka*Ia+lambertianShading(Kd,Id,L,N), alpha);
        return;
    }
}
);//FCODE END

static MCHash _draw;
static MCHash _update;

static void prehash()
{
    _draw = hash("draw");
    _update = hash("update");
}

oninit(MCGLRenderer)
{
    if(init(MCObject)){
        MCGLEngine_featureSwith(MCGLDepthTest, true);
        MCGLEngine_featureSwith(MCGLStencilTest, true);
        MCGLEngine_featureSwith(MCGLCullFace, true);

        MCGLEngine_cullFace(MCGLBack);
        MCGLEngine_setFrontCounterClockWise(true);//CCW

        //glDepthFunc(GL_LESS);
        
        // Enable blending
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        obj->context = new(MCGLContext);

        return obj;
    }else{
        return null;
    }
}

method(MCGLRenderer, void, bye, voida)
{
    release(obj->context);
    superbye(MCObject);
}

method(MCGLRenderer, MCGLRenderer*, initWithShaderCodeString, const char* vcode, const char* fcode)
{
    MCGLContext_initWithShaderCode(obj->context, vcode, fcode,
        (const char* []){
            "position",
            "normal",
            "color",
            "texcoord"
        }, 4,
        (MCGLUniformType []){
            MCGLUniformMat4,
            MCGLUniformMat4,
            MCGLUniformVec3,
            
            MCGLUniformMat4,
            MCGLUniformMat3,
            
            MCGLUniformVec3,
            MCGLUniformVec3,
            MCGLUniformVec3,
            MCGLUniformVec3,
            MCGLUniformVec3,

            MCGLUniformVec3,
            MCGLUniformVec3,
            MCGLUniformVec3,
            MCGLUniformVec1,
            MCGLUniformVec1,
            
            MCGLUniformScalar,
            MCGLUniformScalar
        },
        (const char* []){
            view_view,
            view_projection,
            view_position,
            
            model_model,
            model_normal,
            
            light_ambient,
            light_diffuse,
            light_specular,
            light_color,
            light_position,
            
            material_ambient,
            material_diffuse,
            material_specular,
            material_dissolve,
            material_shininess,
            
            diffuse_sampler,
            specular_sampler
        }, 17);
    return obj;
}

method(MCGLRenderer, MCGLRenderer*, initWithShaderFileName, const char* vshader, const char* fshader)
{
    char path[LINE_MAX];
    MCFileGetPath(vshader, path);
    const char* vcode = MCFileCopyContentWithPath(path);
    
    MCFileGetPath(fshader, path);
    const char* fcode = MCFileCopyContentWithPath(path);
    
    MCGLRenderer_initWithShaderCodeString(obj, vcode, fcode);
    
    free((void*)vcode);
    free((void*)fcode);
    return obj;
}

method(MCGLRenderer, MCGLRenderer*, initWithDefaultShader, voida)
{
    return MCGLRenderer_initWithShaderCodeString(obj, VCODE, FCODE);
}

method(MCGLRenderer, void, updateNodes, MC3DNode* rootnode)
{
    //update nodes
    if (rootnode != null) {
        ff(rootnode, update, obj->context);
    }
}

method(MCGLRenderer, void, drawNodes, MC3DNode* rootnode)
{
    if (rootnode != null) {
        ff(rootnode, draw, obj->context);
        //make FPS stable motion more smooth
        //MCGLEngine_flushCommandBlock(0);
        //MCGLEngine_flushCommandAsync(0);
    }
}

method(MCGLRenderer, void, drawMesh, MCMesh* mesh)
{

}

method(MCGLRenderer, void, drawMaterial, MCMaterial* material)
{
    
}

method(MCGLRenderer, void, drawTexture, MCTexture* texture)
{
    
}

onload(MCGLRenderer)
{
    if (load(MCObject)) {
        prehash();
        binding(MCGLRenderer, void, bye, voida);
        binding(MCGLRenderer, MCGLRenderer*, initWithShaderCodeString, const char* vcode, const char* fcode);
        binding(MCGLRenderer, MCGLRenderer*, initWithShaderFileName, const char* vshader, const char* fshader);
        binding(MCGLRenderer, void, updateNodes, MC3DNode* rootnode);
        binding(MCGLRenderer, void, drawNodes, MC3DNode* rootnode);
        binding(MCGLRenderer, void, drawMesh, MCMesh* mesh);
        binding(MCGLRenderer, void, drawMaterial, MCMaterial* material);
        binding(MCGLRenderer, void, drawTexture, MCTexture* texture);
        return cla;
    }else{
        return null;
    }
}



