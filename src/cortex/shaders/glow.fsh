"#version 100\n"
"precision highp float;\n"
"\
varying  vec4 colorv;\
uniform int lightmap;\
void main( )\
{\
    gl_FragColor = colorv;\
    if ( lightmap == 1 ) gl_FragColor = vec4( 0.0 , 0.0 , 0.0 , 1.0 );\
}\
"
