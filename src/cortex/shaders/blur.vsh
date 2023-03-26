"#version 100\n"
"precision highp float;\n"
"\
attribute vec3 position;\
attribute vec2 texcoord;\
varying vec2 vUv;\
uniform sampler2D texture_glow;\
void main ( )\
{\
    gl_Position = vec4(position,1.0);\
    vUv = texcoord;\
}\
"
