#ifndef ogl_h
    #define ogl_h

    #include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h>
    #ifdef IOS
	#import <OpenGLES/ES2/gl.h>
	#import <OpenGLES/ES2/glext.h>
    #elif defined OSX
	#include <OpenGL/gl3.h>
	#include <OpenGL/gl3ext.h>
    #elif defined ANDROID
	#include <EGL/egl.h>
	#include <GLES2/gl2.h>
    #elif defined __linux__
	#include <GL/glew.h>

	#include <GL/gl.h>
	#include <GL/glu.h>
    #elif defined WINDOWS
	#define GLEW_STATIC
	#include "GL/glew.h"
    #endif
#elif defined ASMJS
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#endif

void   ogl_errors(const char* place);
GLuint ogl_shader_create_from_file(const char* vertex_source, const char* fragment_source, const char** uniform_structure, const char** attribute_structure, GLint* uniform_locations);
GLuint ogl_shader_create(const char* vertex_source, const char* fragment_source, const char** uniform_structure, const char** attribute_structure, GLint* uniform_locations);
void   ogl_shader_delete(GLuint program);
void   ogl_vertexbuffer(GLuint* buffername);
void   ogl_texture_create_linear(GLuint* texturename, GLsizei width, GLsizei height);
void   ogl_texture_create_nearest(GLuint* texturename, GLsizei width, GLsizei height);
void   ogl_framebuffer_with_texture(GLuint* buffername, GLuint* texturename, GLuint* depthname, GLsizei width, GLsizei height);
void   ogl_test(void);
void   ogl_enableva_f2u1(void);
void   ogl_enableva_f2f2(void);
void   ogl_enableva_f3f2(void);
void   ogl_enableva_f3f3(void);
void   ogl_enableva_f3u4(void);
float  ogl_color_float_from_argbuint32(uint32_t color);
float  ogl_color_float_from_rgbauint32(uint32_t color);
void   ogl_errors(const char* place);

//    uint32_t ogl_color_from_floats_reversed( float r , float g , float b , float a );
//    uint32_t ogl_color_from_floats( float r , float g , float b , float a );
//    float   ogl_color_float_from_floats_reversed( float r , float g , float b , float a );
//    float   ogl_color_float_from_floats( float r , float g , float b , float a );
//    void    ogl_color_float_from_uint( float* r , float* g , float* b , float* a , uint32_t color );
// void    ogl_draw_unit_quad( );
void ogl_texture_update_rgba(GLint texture, GLint width, GLint height, uint8_t* data);
//    GLuint ogl_shader_create_blend( GLint* uniforms );
//    GLuint ogl_shader_create_blur( GLint* uniforms );

#if __INCLUDE_LEVEL__ == 0

    #if defined __linux__
    //#include <GL/gl.h>
    //#include <GL/glu.h>
    #endif

/* print opengl errors */

void ogl_errors(const char* place)
{
    GLenum error = 0;
    do
    {
	GLenum error = glGetError();
	if (error > GL_NO_ERROR) printf("GL Error at %s : %i\n", place, error);
    } while (error > GL_NO_ERROR);
}

/* internal : compile shader */

GLuint ogl_shader_compile(GLenum type, const GLchar* source)
{
    GLint  status, logLength, realLength;
    GLuint shader = 0;

    status = 0;
    shader = glCreateShader(type);

    if (shader > 0)
    {
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

	if (logLength > 0)
	{
	    GLchar log[logLength];
	    glGetShaderInfoLog(shader, logLength, &realLength, log);
	    printf("Shader compile log: %s\n", log);
	}

	// get status

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) return 0;
    }
    else printf("Cannot create shader\n");

    return shader;
}

/* internal : link shaders together in gpu */

int ogl_shader_link(GLuint program)
{
    GLint status, logLength, realLength;

    glLinkProgram(program);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

    if (logLength > 0)
    {
	GLchar log[logLength];
	glGetProgramInfoLog(program, logLength, &realLength, log);
	printf("Program link log : %i %s\n", realLength, log);
    }

    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_TRUE) return 1;
    return 0;
}

void ogl_loadfile(const char* filename, char* buffer)
{
    FILE* file  = fopen(filename, "rb");
    int   index = 0;
    if (file)
    {
	while (1)
	{
	    int next = fgetc(file);

	    if (next == EOF) break;

	    buffer[index++] = next;
	}
	// fread( buffer , 1 , 10000 , file );
	fclose(file);
    }
}

GLuint ogl_shader_create_from_file(const char* vertex_source, const char* fragment_source, const char** uniform_structure, const char** attribute_structure, GLint* uniform_locations)
{
    char vertex[10000] = {0};

    ogl_loadfile(vertex_source, vertex);

    char fragment[10000] = {0};

    ogl_loadfile(fragment_source, fragment);

    #ifdef DEBUG
	// printf( "compiling %s and %s\n" , vertex_source , fragment_source );
    #endif

    return ogl_shader_create(vertex, fragment, uniform_structure, attribute_structure, uniform_locations);
}

/* create shader */

GLuint ogl_shader_create(const char* vertex_source, const char* fragment_source, const char** uniform_structure, const char** attribute_structure, GLint* uniform_locations)
{
    #ifdef DEBUG
    ogl_errors("before ogl_shader_create\n");
    #endif

    int uniform_locations_length   = atoi(uniform_structure[0]);
    int attribute_locations_length = atoi(attribute_structure[0]);

    GLuint program = glCreateProgram();

    GLuint vertex_shader = ogl_shader_compile(GL_VERTEX_SHADER, vertex_source);
    if (vertex_shader == 0) printf("Failed to compile vertex shader : %s\n", vertex_source);

    GLuint fragment_shader = ogl_shader_compile(GL_FRAGMENT_SHADER, fragment_source);
    if (fragment_shader == 0) printf("Failed to compile fragment shader : %s\n", fragment_source);

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    for (int index = 0; index < attribute_locations_length; index++)
    {
	const GLchar* name = attribute_structure[index + 1];
	glBindAttribLocation(program, index, name);
    }

    int success = ogl_shader_link(program);

    if (success == 1)
    {
	for (int index = 0; index < uniform_locations_length; index++)
	{
	    const GLchar* name       = uniform_structure[index + 1];
	    GLint         location   = glGetUniformLocation(program, name);
	    uniform_locations[index] = location;
	}
    }
    else printf("Failed to link shader program\n");

    if (vertex_shader > 0)
    {
	glDetachShader(program, vertex_shader);
	glDeleteShader(vertex_shader);
    }

    if (fragment_shader > 0)
    {
	glDetachShader(program, fragment_shader);
	glDeleteShader(fragment_shader);
    }

    #ifdef DEBUG
    ogl_errors("after ogl_shader_create\n");
    #endif

    return program;
}

/* delete shader */

void ogl_shader_delete(GLuint program)
{
    glDeleteProgram(program);
}

/* creates vertex buffer */

void ogl_vertexbuffer(GLuint* buffername)
{
    #ifdef DEBUG
    ogl_errors("before ogl_vertexbuffer");
    #endif
    glGenBuffers(1, buffername);
    #ifdef DEBUG
    ogl_errors("after ogl_vertexbuffer");
    #endif
}

/* creates texture with linear filtering */

void ogl_texture_create_linear(GLuint* texturename, GLsizei width, GLsizei height)
{
    #ifdef DEBUG
    ogl_errors("before ogl_texture_create_linear");
    #endif

    glGenTextures(1, texturename);
    glBindTexture(GL_TEXTURE_2D, *texturename);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    #ifdef DEBUG
    ogl_errors("after ogl_texture_create_linear");
    #endif
}

/* creates texture with nearest filtering */

void ogl_texture_create_nearest(GLuint* texturename, GLsizei width, GLsizei height)
{
    #ifdef DEBUG
    ogl_errors("before ogl_texture_create_nearest");
    #endif

    glGenTextures(1, texturename);
    glBindTexture(GL_TEXTURE_2D, *texturename);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    #ifdef DEBUG
    ogl_errors("after ogl_texture_create_nearest");
    #endif
}

/* creates framebuffer backed by on given texture */

void ogl_framebuffer_with_texture(GLuint* buffername, GLuint* texturename, GLuint* depthname, GLsizei width, GLsizei height)
{
    #ifdef DEBUG
    ogl_errors("before ogl_framebuffer_with_texture");
    #endif

    glGenTextures(1, texturename);
    glBindTexture(GL_TEXTURE_2D, *texturename);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, depthname);
    glBindRenderbuffer(GL_RENDERBUFFER, *depthname);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, buffername);
    glBindFramebuffer(GL_FRAMEBUFFER, *buffername);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texturename, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *depthname);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) printf("Failed to create framebuffer at ogl_framebuffer_with_texture ");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    #ifdef DEBUG
    ogl_errors("before ogl_framebuffer_with_texture");
    #endif
}

/* draws a simple full cube for debug purposes */

void ogl_test()
{
    #ifdef DEBUG
    ogl_errors("before ogl_drawcube");
    #endif

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    const char* vertex_flat =
	"attribute vec3 position;\n"
	"void main ( )\n"
	"{\n"
	"gl_Position = vec4(position,1.0);\n"
	"}\n";

    const char* fragment_flat =
	"void main( )\n"
	"{\n"
	"	gl_FragColor = vec4(1.0,1.0,1.0,1.0);\n"
	"}\n";

    const char* uniforms[]   = {"0"};
    const char* attributes[] = {"1", "position"};
    GLint       shader       = ogl_shader_create(vertex_flat, fragment_flat, attributes, uniforms, NULL);

    glUseProgram(shader);

    GLuint vbo;
    ogl_vertexbuffer(&vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    ogl_enableva_f3f2();

    GLfloat vertexes[] = {
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 5, vertexes, GL_STATIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    #ifdef DEBUG
    ogl_errors("after ogl_drawcube");
    #endif
}

/* enables vertex arrays with 2 float 1 unsigned byte */

void ogl_enableva_f2u1()
{
    #ifdef DEBUG
    ogl_errors("before ogl_enableva_f2u1");
    #endif
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 12, 0);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 12, (const GLvoid*) 8);
    #ifdef DEBUG
    ogl_errors("after ogl_enableva_f3u4");
    #endif
}

/* enables vertex arrays with 2 float 2 float */

void ogl_enableva_f2f2()
{
    #ifdef DEBUG
    ogl_errors("before ogl_enableva_f2f2");
    #endif
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (const GLvoid*) 8);
    #ifdef DEBUG
    ogl_errors("after ogl_enableva_f2f2");
    #endif
}

/* enables vertex arrays with 3 float 2 float */

void ogl_enableva_f3f2()
{
    #ifdef DEBUG
    ogl_errors("before ogl_enableva_f3f2");
    #endif
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 20, 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 20, (const GLvoid*) 12);
    #ifdef DEBUG
    ogl_errors("after ogl_enableva_f3f2");
    #endif
}

/* enables vertex arrays with 3 float 3 float */

void ogl_enableva_f3f3()
{
    #ifdef DEBUG
    ogl_errors("before ogl_enableva_f3f3");
    #endif
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, (const GLvoid*) 12);
    #ifdef DEBUG
    ogl_errors("after ogl_enableva_f3f3");
    #endif
}

/* enables vertex arrays with 3 float 4 unsigned */

void ogl_enableva_f3u4()
{
    #ifdef DEBUG
    ogl_errors("before ogl_enableva_f3u4");
    #endif
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 16, 0);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 16, (const GLvoid*) 12);
    #ifdef DEBUG
    ogl_errors("after ogl_enableva_f3u4");
    #endif
}

union colorfloat
{
    float    floatvalue;
    uint8_t  bytes[4];
    uint32_t color;
};

/* puts an rgba uint32 into one float for a float vertex buffer */

float ogl_color_float_from_rgbauint32(uint32_t color)
{
    union colorfloat cf;
    cf.bytes[0] = (color >> 24) & 0xFF;
    cf.bytes[1] = (color >> 16) & 0xFF;
    cf.bytes[2] = (color >> 8) & 0xFF;
    cf.bytes[3] = (color) &0xFF;

    return cf.floatvalue;
}

/* puts an argb uint32 into one float for a float vertex buffer */

float ogl_color_float_from_argbuint32(uint32_t color)
{
    union colorfloat cf;
    cf.color = color;
    return cf.floatvalue;
}

//	float ogl_color_float_from_floats( float r , float g , float b , float a )
//	{
//		if ( r < 0.0 ) r = 0.0; else if ( r > 1.0 ) r = 1.0;
//		if ( g < 0.0 ) g = 0.0; else if ( g > 1.0 ) g = 1.0;
//		if ( b < 0.0 ) b = 0.0; else if ( b > 1.0 ) b = 1.0;
//		if ( a < 0.0 ) a = 0.0; else if ( a > 1.0 ) a = 1.0;
//
//		union colorfloat cf;
//		cf.bytes[0] = (uint8_t)(r * 254.0); // using 254 because of emscripten
//		cf.bytes[1] = (uint8_t)(g * 254.0);
//		cf.bytes[2] = (uint8_t)(b * 254.0);
//		cf.bytes[3] = (uint8_t)(a * 254.0);
//
//		return cf.floatvalue;
//	}
//
//	uint32_t ogl_color_from_floats_reversed( float r , float g , float b , float a )
//	{
//		return (uint8_t)(r * 255.0) | (uint8_t)(g * 255.0) << 8 | (uint8_t)(b * 255.0) << 16 | (uint8_t)(a * 255.0) << 24;
//	}
//
//	uint32_t ogl_color_from_floats( float r , float g , float b , float a )
//	{
//		return (uint8_t)(r * 255.0) << 24 | (uint8_t)(g * 255.0) << 16 | (uint8_t)(b * 255.0) << 8 | (uint8_t)(a * 255.0);
//	}
//
//	float ogl_color_float_from_floats_reversed( float r , float g , float b , float a )
//	{
//		if ( r < 0.0 ) r = 0.0; else if ( r > 1.0 ) r = 1.0;
//		if ( g < 0.0 ) g = 0.0; else if ( g > 1.0 ) g = 1.0;
//		if ( b < 0.0 ) b = 0.0; else if ( b > 1.0 ) b = 1.0;
//		if ( a < 0.0 ) a = 0.0; else if ( a > 1.0 ) a = 1.0;
//
//		union colorfloat cf;
//		cf.bytes[0] = (uint8_t)(a * 254.0);
//		cf.bytes[1] = (uint8_t)(b * 254.0);
//		cf.bytes[2] = (uint8_t)(g * 254.0);
//		cf.bytes[3] = (uint8_t)(r * 254.0);
//
//		return cf.floatvalue;
//	}
//
//	void ogl_color_float_from_uint( float* r , float* g , float* b , float* a , uint32_t color )
//	{
//		*r = (float)(( color >> 24 ) & 0xFF ) / 255.0;
//		*g = (float)(( color >> 16 ) & 0xFF ) / 255.0;
//		*b = (float)(( color >> 8  ) & 0xFF ) / 255.0;
//		*a = (float)(( color       ) & 0xFF ) / 255.0;
//	}
//
//	void ogl_draw_unit_quad( )
//	{
//		GLfloat vertexes[] = {
//			-1.0f, -1.0f, 0.0f, 0.0f , 0.0f ,
//			 1.0f, -1.0f, 0.0f, 1.0f , 0.0f ,
//			-1.0f,  1.0f, 0.0f, 0.0f , 1.0f ,
//			-1.0f,  1.0f, 0.0f, 0.0f , 1.0f ,
//			 1.0f, -1.0f, 0.0f, 1.0f , 0.0f ,
//			 1.0f,  1.0f, 0.0f, 1.0f , 1.0f
//		};
//
//		ogl_enableva_f3f2( );
//
//        glBufferData( GL_ARRAY_BUFFER , sizeof(GLfloat) * 6 * 5 , vertexes , GL_DYNAMIC_DRAW );
//		glDrawArrays( GL_TRIANGLES , 0, 6);
//	}
//
void ogl_texture_update_rgba(GLint texture, GLint width, GLint height, uint8_t* data)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}
//
//	const char* vertex_blur;
//	const char* fragment_blur;
//
//	const char* vertex_blend;
//	const char* fragment_blend;
//
//	GLuint ogl_shader_create_blend( GLint* uniforms )
//	{
//        char* blenduniforms[ ]   = { "2" , "texture_base" , "texture_glow" };
//        char* blendattributes[ ] = { "2" , "position" , "texcoord"};
//
//		GLuint shader = ogl_shader_create( vertex_blend , fragment_blend , blenduniforms , blendattributes , uniforms );
//		return shader;
//	}
//
//	GLuint ogl_shader_create_blur( GLint* uniforms )
//	{
//        char* bluruniforms[ ]   = { "2" , "texture_glow" , "orientation" };
//        char* blurattributes[ ] = { "2" , "position" , "texcoord" };
//
//		GLuint shader = ogl_shader_create( vertex_blur , fragment_blur , bluruniforms , blurattributes , uniforms );
//		return shader;
//	}
//
//	#if defined(IOS) || defined(ANDROID) || defined(ASMJS)
//
//	/* blend shader pair */
//
//	const char* vertex_blend =
//		"attribute highp vec3 position;\n"
//		"attribute highp vec2 texcoord;\n"
//
//		"uniform sampler2D texture_base;\n"
//		"uniform sampler2D texture_glow;\n"
//
//		"varying highp vec2 vUv;\n"
//
//		"void main ( )\n"
//		"{\n"
//			"gl_Position = vec4(position,1.0);\n"
//			"vUv = texcoord;\n"
//		"}\n";
//
//	const char* fragment_blend =
//		"uniform sampler2D texture_base;\n"
//		"uniform sampler2D texture_glow;\n"
//
//		"varying highp vec2 vUv;\n"
//
//		//"uniform int BlendMode;\n"
//		"void main( )\n"
//		"{\n"
//		"	highp vec4 dst = texture2D(texture_base, vUv); // rendered scene\n"
//		"	highp vec4 src = texture2D(texture_glow, vUv); // glowmap\n"
//		"	int BlendMode = 0;\n"
//
//		"	if ( BlendMode == 0 )\n"
//		"	{\n"
//		"		// Additive blending (strong result, high overexposure)\n"
//		"		gl_FragColor = min(src + dst, 1.0);\n"
//		"	}\n"
//		"	else if ( BlendMode == 1 )\n"
//		"	{\n"
//		"		// Screen blending (mild result, medium overexposure)\n"
//		"		gl_FragColor = clamp((src + dst) - (src * dst), 0.0, 1.0);\n"
//		"		gl_FragColor.w = 1.0;\n"
//		"	}\n"
//		"	else if ( BlendMode == 2 )\n"
//		"	{\n"
//		"		// Softlight blending (light result, no overexposure)\n"
//		"		// Due to the nature of soft lighting, we need to bump the black region of the glowmap\n"
//		"		// to 0.5, otherwise the blended result will be dark (black soft lighting will darken\n"
//		"		// the image).\n"
//		"		src = (src * 0.5) + 0.5;\n"
//		"\n"
//		"		gl_FragColor.xyz = vec3((src.x <= 0.5) ? (dst.x - (1.0 - 2.0 * src.x) * dst.x * (1.0 - dst.x)) : (((src.x > 0.5) && (dst.x <= 0.25)) ? (dst.x + (2.0 * src.x - 1.0) * (4.0 * dst.x * (4.0 * dst.x + 1.0) * (dst.x - 1.0) + 7.0 * dst.x)) : (dst.x + (2.0 * src.x - 1.0) * (sqrt(dst.x) - dst.x))),\n"
//		"					(src.y <= 0.5) ? (dst.y - (1.0 - 2.0 * src.y) * dst.y * (1.0 - dst.y)) : (((src.y > 0.5) && (dst.y <= 0.25)) ? (dst.y + (2.0 * src.y - 1.0) * (4.0 * dst.y * (4.0 * dst.y + 1.0) * (dst.y - 1.0) + 7.0 * dst.y)) : (dst.y + (2.0 * src.y - 1.0) * (sqrt(dst.y) - dst.y))),\n"
//		"					(src.z <= 0.5) ? (dst.z - (1.0 - 2.0 * src.z) * dst.z * (1.0 - dst.z)) : (((src.z > 0.5) && (dst.z <= 0.25)) ? (dst.z + (2.0 * src.z - 1.0) * (4.0 * dst.z * (4.0 * dst.z + 1.0) * (dst.z - 1.0) + 7.0 * dst.z)) : (dst.z + (2.0 * src.z - 1.0) * (sqrt(dst.z) - dst.z))));\n"
//		"		gl_FragColor.w = 1.0;\n"
//		"	}\n"
//		"	else\n"
//		"	{\n"
//		"		// Show just the glow map\n"
//		"		gl_FragColor = src;\n"
//		"	}\n"
//		"}\n";
//
//	/* blur shader pair */
//
//	const char* vertex_blur =
//		"attribute highp vec3 position;\n"
//		"attribute highp vec2 texcoord;\n"
//		"varying highp vec2 vUv;\n"
//		"uniform sampler2D texture_glow;\n"
//		"void main ( )\n"
//		"{\n"
//			"gl_Position = vec4(position,1.0);\n"
//			"vUv = texcoord;\n"
//		"}\n";
//
//	const char* fragment_blur =
//		"varying highp vec2 vUv;\n"
//		"uniform sampler2D texture_glow;\n"
////		"uniform vec2 TexelSize;\n"
//		"uniform int orientation;\n"
////		"uniform int BlurAmount;\n"
////		"uniform float BlurScale;\n"
////		"uniform float BlurStrength;\n"
//		"highp float Gaussian (highp float x, highp float deviation)\n"
//		"{\n"
//		"	return (1.0 / sqrt(2.0 * 3.141592 * deviation)) * exp(-((x * x) / (2.0 * deviation)));\n"
//		"}\n"
//		"void main( )\n"
//		"{\n"
//			"highp float BlurStrength = 0.05;\n"
//			"int BlurAmount = 10;\n"
////			"int Orientation = 0;\n"
//			"highp float BlurScale = 0.1;\n"
//			"highp float TexelSize = 0.1;\n"
//
//			"highp float halfBlur = float(BlurAmount) * 0.5;\n"
//			"highp vec4 colour = vec4(0.0);\n"
//			"highp vec4 texColour = vec4(0.0);\n"
//			"highp float deviation = halfBlur * 0.35;\n"
//			"deviation *= deviation;\n"
//			"highp float strength = 1.0 - BlurStrength;\n"
//			"if ( orientation == 0 )\n"
//			"{\n"
//			"	for (int i = 0; i < 10; ++i)\n"
//			"	{\n"
//			"		if ( i >= 10 ) break;\n"
//			"			highp float offset = float(i) - halfBlur;\n"
//			"			texColour = texture2D(texture_glow, vUv + vec2(offset * TexelSize * BlurScale, 0.0)) * Gaussian(offset * strength, deviation);\n"
//			"			colour += texColour;\n"
//			"	}\n"
//			"}\n"
//			"else\n"
//			"{\n"
//			"	for (int i = 0; i < 20; ++i)\n"
//			"	{\n"
//			"		if ( i >= 20 ) break;\n"
//			"			highp float offset = float(i) - halfBlur;\n"
//			"			texColour = texture2D(texture_glow, vUv + vec2(0.0,offset * TexelSize * BlurScale)) * Gaussian(offset * strength, deviation);\n"
//			"			colour += texColour;\n"
//			"	}\n"
//			"}\n"
//			"gl_FragColor = clamp(colour, 0.0, 1.0);\n"
//			"gl_FragColor.w = 1.0;\n"
//		"}\n";
//
//	#else
//
//	/* blend shader pair */
//
//	const char* vertex_blend =
//		"attribute vec3 position;\n"
//		"attribute vec2 texcoord;\n"
//
//		"uniform sampler2D texture_base;\n"
//		"uniform sampler2D texture_glow;\n"
//
//		"varying vec2 vUv;\n"
//
//		"void main ( )\n"
//		"{\n"
//			"gl_Position = vec4(position,1.0);\n"
//			"vUv = texcoord;\n"
//		"}\n";
//
//	const char* fragment_blend =
//		"uniform sampler2D texture_base;\n"
//		"uniform sampler2D texture_glow;\n"
//
//		"varying vec2 vUv;\n"
//
//		//"uniform int BlendMode;\n"
//		"void main( )\n"
//		"{\n"
//		"	vec4 dst = texture2D(texture_base, vUv); // rendered scene\n"
//		"	vec4 src = texture2D(texture_glow, vUv); // glowmap\n"
//		"	int BlendMode = 0;\n"
//
//		"	if ( BlendMode == 0 )\n"
//		"	{\n"
//		"		// Additive blending (strong result, high overexposure)\n"
//		"		gl_FragColor = min(src + dst, 1.0);\n"
//		"	}\n"
//		"	else if ( BlendMode == 1 )\n"
//		"	{\n"
//		"		// Screen blending (mild result, medium overexposure)\n"
//		"		gl_FragColor = clamp((src + dst) - (src * dst), 0.0, 1.0);\n"
//		"		gl_FragColor.w = 1.0;\n"
//		"	}\n"
//		"	else if ( BlendMode == 2 )\n"
//		"	{\n"
//		"		// Softlight blending (light result, no overexposure)\n"
//		"		// Due to the nature of soft lighting, we need to bump the black region of the glowmap\n"
//		"		// to 0.5, otherwise the blended result will be dark (black soft lighting will darken\n"
//		"		// the image).\n"
//		"		src = (src * 0.5) + 0.5;\n"
//		"\n"
//		"		gl_FragColor.xyz = vec3((src.x <= 0.5) ? (dst.x - (1.0 - 2.0 * src.x) * dst.x * (1.0 - dst.x)) : (((src.x > 0.5) && (dst.x <= 0.25)) ? (dst.x + (2.0 * src.x - 1.0) * (4.0 * dst.x * (4.0 * dst.x + 1.0) * (dst.x - 1.0) + 7.0 * dst.x)) : (dst.x + (2.0 * src.x - 1.0) * (sqrt(dst.x) - dst.x))),\n"
//		"					(src.y <= 0.5) ? (dst.y - (1.0 - 2.0 * src.y) * dst.y * (1.0 - dst.y)) : (((src.y > 0.5) && (dst.y <= 0.25)) ? (dst.y + (2.0 * src.y - 1.0) * (4.0 * dst.y * (4.0 * dst.y + 1.0) * (dst.y - 1.0) + 7.0 * dst.y)) : (dst.y + (2.0 * src.y - 1.0) * (sqrt(dst.y) - dst.y))),\n"
//		"					(src.z <= 0.5) ? (dst.z - (1.0 - 2.0 * src.z) * dst.z * (1.0 - dst.z)) : (((src.z > 0.5) && (dst.z <= 0.25)) ? (dst.z + (2.0 * src.z - 1.0) * (4.0 * dst.z * (4.0 * dst.z + 1.0) * (dst.z - 1.0) + 7.0 * dst.z)) : (dst.z + (2.0 * src.z - 1.0) * (sqrt(dst.z) - dst.z))));\n"
//		"		gl_FragColor.w = 1.0;\n"
//		"	}\n"
//		"	else\n"
//		"	{\n"
//		"		// Show just the glow map\n"
//		"		gl_FragColor = src;\n"
//		"	}\n"
//		"}\n";
//
//	/* blur shader pair */
//
//	const char* vertex_blur =
//		"attribute vec3 position;\n"
//		"attribute vec2 texcoord;\n"
//		"varying vec2 vUv;\n"
//		"uniform sampler2D texture_glow;\n"
//		"void main ( )\n"
//		"{\n"
//			"gl_Position = vec4(position,1.0);\n"
//			"vUv = texcoord;\n"
//		"}\n";
//
//	const char* fragment_blur =
//		"varying vec2 vUv;\n"
//		"uniform sampler2D texture_glow;\n"
////		"uniform vec2 TexelSize;\n"
//		"uniform int orientation;\n"
////		"uniform int BlurAmount;\n"
////		"uniform float BlurScale;\n"
////		"uniform float BlurStrength;\n"
//		"float Gaussian (float x, float deviation)\n"
//		"{\n"
//		"	return (1.0 / sqrt(2.0 * 3.141592 * deviation)) * exp(-((x * x) / (2.0 * deviation)));\n"
//		"}\n"
//		"void main( )\n"
//		"{\n"
//			"float BlurStrength = 0.05;\n"
//			"int BlurAmount = 10;\n"
////			"int Orientation = 0;\n"
//			"float BlurScale = 0.1;\n"
//			"float TexelSize = 0.1;\n"
//
//			"float halfBlur = float(BlurAmount) * 0.5;\n"
//			"vec4 colour = vec4(0.0);\n"
//			"vec4 texColour = vec4(0.0);\n"
//			"float deviation = halfBlur * 0.35;\n"
//			"deviation *= deviation;\n"
//			"float strength = 1.0 - BlurStrength;\n"
//			"if ( orientation == 0 )\n"
//			"{\n"
//			"	for (int i = 0; i < 10; ++i)\n"
//			"	{\n"
//			"		if ( i >= 10 ) break;\n"
//			"			float offset = float(i) - halfBlur;\n"
//			"			texColour = texture2D(texture_glow, vUv + vec2(offset * TexelSize * BlurScale, 0.0)) * Gaussian(offset * strength, deviation);\n"
//			"			colour += texColour;\n"
//			"	}\n"
//			"}\n"
//			"else\n"
//			"{\n"
//			"	for (int i = 0; i < 20; ++i)\n"
//			"	{\n"
//			"		if ( i >= 20 ) break;\n"
//			"			float offset = float(i) - halfBlur;\n"
//			"			texColour = texture2D(texture_glow, vUv + vec2(0.0,offset * TexelSize * BlurScale)) * Gaussian(offset * strength, deviation);\n"
//			"			colour += texColour;\n"
//			"	}\n"
//			"}\n"
//			"gl_FragColor = clamp(colour, 0.0, 1.0);\n"
//			"gl_FragColor.w = 1.0;\n"
//		"}\n";
//
//	#endif

//	opengl es3/4.0 versions of shaders

//        const char* vertex_source =
//        #ifdef IOS
//        "#version 300 es\n"
//        "precision highp float;\n"
//
//        "in vec4   position;\n"
//        "in vec2   texcoord;\n"
//		  "in float  alpha;\n"
//
//        "out vec2  texcoordv;\n"
//		  "out float alphav;\n"
//
//        "uniform mat4 projection;\n"
//        "void main ( )\n"
//        "{\n"
//			"gl_Position = projection * position;\n"
//          "texcoordv = texcoord;\n"
//          "alphav = alpha;\n"
//        "}\n";
//        #else
//        "#version 410\n"
//
//        "in vec4   position;\n"
//        "in vec2   texcoord;\n"
//		  "in float  alpha;\n"
//
//        "out vec2  texcoordv;\n"
//        "out vec3  positionv;\n"
//		  "out float alphav;\n"
//
//        "uniform mat4 projection;\n"
//
//        "void main ( )\n"
//        "{\n"
//			"gl_Position = projection * position;\n"
//			"positionv = vec3(position);\n"
//          "texcoordv  = texcoord;\n"
//          "alphav = alpha;\n"
//        "}\n";
//        #endif

//        const char* fragment_source =
//        #ifdef IOS
//            "#version 300 es\n"
//            "precision highp float;\n"
//
//            "in vec2  texcoordv;\n"
//			"in float alphav;\n"
//
//            "out vec4 fragColor;\n"
//
//            "uniform sampler2D maintexture;\n"
//            "uniform sampler2D frametexture;\n"
//
//            "void main ( )\n"
//            "{\n"
//            "    fragColor = texture(maintexture, texcoordv, 0.0);\n"
//            "}\n";
//        #else
//            "#version 410\n"
//
//            "in vec2  texcoordv;\n"
//			"in vec3  positionv;\n"
//			"in float alphav;\n"
//
//            "out vec4 fragColor;\n"
//
//            "uniform sampler2D maintexture;\n"
//            "uniform sampler2D frametexture;\n"
//            "void main ( )\n"
//            "{\n"
//            "    fragColor = texture(maintexture, texcoordv, 0.0);\n"
//			"	 fragColor *= ( 100.0 + positionv.z ) / 100.0;\n"
//			"	 fragColor.w *= alphav;\n"
//            "}\n";
//        #endif
//

#endif
