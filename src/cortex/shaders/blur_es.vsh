"\
attribute highp vec3 position;\
attribute highp vec2 texcoord;\
varying highp vec2 vUv;\
uniform sampler2D texture_glow;\
void main ( )\
{\
    gl_Position = vec4(position,1.0);\
    vUv = texcoord;\
}\
"
