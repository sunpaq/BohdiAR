#version 300 es

precision highp sampler3D;
precision highp float;
precision lowp int;
const float Epsilon = 0.0000001;

//input
in vec3 TexCoords;

//uniforms
uniform samplerCube cubeSampler;

//output
out vec4 color;

void main()
{
    color = texture(cubeSampler, TexCoords);
}
