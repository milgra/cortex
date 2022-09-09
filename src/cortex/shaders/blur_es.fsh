"\
varying highp vec2 vUv;\
uniform sampler2D texture_glow;\
uniform int orientation;\
highp float Gaussian (highp float x, highp float deviation)\
{\
	return (1.0 / sqrt(2.0 * 3.141592 * deviation)) * exp(-((x * x) / (2.0 * deviation)));\
}\
void main( )\
{\
highp float BlurStrength = 0.05;\
int BlurAmount = 10;\
highp float BlurScale = 0.1;\
highp float TexelSize = 0.1;\
\
highp float halfBlur = float(BlurAmount) * 0.5;\
highp vec4 colour = vec4(0.0);\
highp vec4 texColour = vec4(0.0);\
highp float deviation = halfBlur * 0.35;\
deviation *= deviation;\
highp float strength = 1.0 - BlurStrength;\
if ( orientation == 0 )\
{\
	for (int i = 0; i < 10; ++i)\
	{\
		if ( i >= 10 ) break;\
			highp float offset = float(i) - halfBlur;\
			texColour = texture2D(texture_glow, vUv + vec2(offset * TexelSize * BlurScale, 0.0)) * Gaussian(offset * strength, deviation);\
			colour += texColour;\
	}\
}\
else\
{\
	for (int i = 0; i < 20; ++i)\
	{\
		if ( i >= 20 ) break;\
			highp float offset = float(i) - halfBlur;\
			texColour = texture2D(texture_glow, vUv + vec2(0.0,offset * TexelSize * BlurScale)) * Gaussian(offset * strength, deviation);\
			colour += texColour;\
	}\
}\
gl_FragColor = clamp(colour, 0.0, 1.0);\
gl_FragColor.w = 1.0;\
}\
"