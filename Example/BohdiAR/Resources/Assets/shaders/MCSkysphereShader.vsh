#version 300 es

precision highp float;
precision mediump int;

//vertex attributes
layout (location=0) in vec3 position;
layout (location=1) in vec2 texcoord;

//uniforms
uniform mat4 sphViewMatrix;
uniform mat4 sphProjectionMatrix;

//output
out vec2 TexCoords;

void main()
{
    gl_Position = sphProjectionMatrix * sphViewMatrix * vec4(position, 1.0);
    TexCoords = texcoord;
}
