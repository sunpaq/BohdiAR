#version 300 es

precision highp float;
precision mediump int;

//vertex attributes
layout (location=0) in vec3 position;

//uniforms
uniform mat4 boxViewMatrix;
uniform mat4 boxProjectionMatrix;

//output
out vec3 TexCoords;

void main()
{
    gl_Position = boxProjectionMatrix * boxViewMatrix * vec4(position, 1.0);
    TexCoords = position;
}
