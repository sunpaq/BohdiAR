#version 300 es

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
