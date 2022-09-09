"\
attribute vec4 position;\
attribute vec4 color;\
varying highp vec4 colorv;\
uniform mat4 projection;\
void main ( )\
{\
    gl_Position = projection * position;\
    colorv = color;\
}\
"