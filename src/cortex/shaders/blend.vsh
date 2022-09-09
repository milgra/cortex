"\
attribute vec3 position;\
attribute vec2 texcoord;\
\
uniform sampler2D texture_base;\
uniform sampler2D texture_glow;\
\
varying vec2 vUv;\
\
void main ( )\
{\
    gl_Position = vec4(position,1.0);\
    vUv = texcoord;\
}\
"
