"\
attribute vec4 position;\
attribute vec4 color;\
varying vec4 colorv;\
uniform int lightmap;\
uniform mat4 projection;\
void main ( )\
{\
    gl_Position = projection * position;\
    colorv = color;\
}\
"
