"\
uniform sampler2D texture_base;\
uniform sampler2D texture_glow;\
\
varying vec2 vUv;\
\
void main( )\
{\
	vec4 dst = texture2D(texture_base, vUv);\
	vec4 src = texture2D(texture_glow, vUv);\
	int BlendMode = 0;\
\
	if ( BlendMode == 0 )\
	{\
		gl_FragColor = min(src + dst, 1.0);\
	}\
	else if ( BlendMode == 1 )\
	{\
		gl_FragColor = clamp((src + dst) - (src * dst), 0.0, 1.0);\
		gl_FragColor.w = 1.0;\
	}\
	else if ( BlendMode == 2 )\
	{\
		src = (src * 0.5) + 0.5;\
		gl_FragColor.xyz = vec3((src.x <= 0.5) ? (dst.x - (1.0 - 2.0 * src.x) * dst.x * (1.0 - dst.x)) : (((src.x > 0.5) && (dst.x <= 0.25)) ? (dst.x + (2.0 * src.x - 1.0) * (4.0 * dst.x * (4.0 * dst.x + 1.0) * (dst.x - 1.0) + 7.0 * dst.x)) : (dst.x + (2.0 * src.x - 1.0) * (sqrt(dst.x) - dst.x))),\
					(src.y <= 0.5) ? (dst.y - (1.0 - 2.0 * src.y) * dst.y * (1.0 - dst.y)) : (((src.y > 0.5) && (dst.y <= 0.25)) ? (dst.y + (2.0 * src.y - 1.0) * (4.0 * dst.y * (4.0 * dst.y + 1.0) * (dst.y - 1.0) + 7.0 * dst.y)) : (dst.y + (2.0 * src.y - 1.0) * (sqrt(dst.y) - dst.y))),\
					(src.z <= 0.5) ? (dst.z - (1.0 - 2.0 * src.z) * dst.z * (1.0 - dst.z)) : (((src.z > 0.5) && (dst.z <= 0.25)) ? (dst.z + (2.0 * src.z - 1.0) * (4.0 * dst.z * (4.0 * dst.z + 1.0) * (dst.z - 1.0) + 7.0 * dst.z)) : (dst.z + (2.0 * src.z - 1.0) * (sqrt(dst.z) - dst.z))));\
		gl_FragColor.w = 1.0;\
	}\
	else\
	{\
		gl_FragColor = src;\
	}\
}\
"
