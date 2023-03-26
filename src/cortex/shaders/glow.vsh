"#version 100\n"
"precision highp float;\n"
"\
attribute vec4 position;\
attribute vec4 color;\
varying vec4 colorv;\
uniform mat4 projection;\
void main ( )\
{\
    gl_Position = projection * position;\
    colorv = color;\
}\
"
