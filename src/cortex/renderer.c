/* OpenGL renderer */

#ifndef renderer_h
#define renderer_h

#include "GL/glew.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "defaults.c"
#include "floatbuffer.c"
#include "zc_mat4.c"
#include <stdint.h>

typedef struct _renderdata_t
{
    char           level;
    floatbuffer_t* buffer;

} renderdata_t;

void renderer_init(
    GLuint defaultFrameBuffer,
    GLuint defaultRenderBuffer);

void renderer_free(
    void);

void renderer_render(
    void);

void renderer_reset_buffers(
    void);

#endif /* renderer_h */

#if __INCLUDE_LEVEL__ == 0

#include "bus.c"
#include "defaults.c"
#include "floatbuffer.c"
#include "ogl.c"
#include "voxel.c"
#include <float.h>
#include <string.h>

#if defined(IOS) || defined(ANDROID) || defined(ASMJS)

char* glow_vsh =
    #include "shaders/glow_es.vsh"
    ;
char* glow_fsh =
    #include "shaders/glow_es.fsh"
    ;
char* blur_vsh =
    #include "shaders/blur_es.vsh"
    ;
char* blur_fsh =
    #include "shaders/blur_es.fsh"
    ;
char* blend_vsh =
    #include "shaders/blend_es.vsh"
    ;
char* blend_fsh =
    #include "shaders/blend_es.fsh"
    ;

#else

char* glow_vsh =
    #include "shaders/glow.vsh"
    ;
char* glow_fsh =
    #include "shaders/glow.fsh"
    ;
char* blur_vsh =
    #include "shaders/blur.vsh"
    ;
char* blur_fsh =
    #include "shaders/blur.fsh"
    ;
char* blend_vsh =
    #include "shaders/blend.vsh"
    ;
char* blend_fsh =
    #include "shaders/blend.fsh"
    ;

#endif

const char* uniforms_game[]   = {"2", "projection", "lightmap"};
const char* attributes_game[] = {"2", "position", "color"};

const char* uniforms_blend[]   = {"2", "texture_base", "texture_glow"};
const char* attributes_blend[] = {"2", "position", "texcoord"};

const char* uniforms_blur[]   = {"2", "texture_glow", "orientation"};
const char* attributes_blur[] = {"2", "position", "texcoord"};

void renderer_onmessage(
    const char* name,
    void*       data);

void renderer_update_buffer(
    GLfloat* vertexes,
    uint32_t count,
    m4_t     projection,
    int      level);

struct renderer_t
{

    GLuint shader_glow;
    GLuint shader_blur;
    GLuint shader_blend;

    GLuint fb_game;
    GLuint fb_glow;
    GLuint fb_blur;

    GLuint tex_game;
    GLuint tex_glow;
    GLuint tex_blur;

    GLuint depth_game;
    GLuint depth_glow;
    GLuint depth_blur;

    GLuint vbo_main;

    GLuint defaultFrameBuffer;
    GLuint defaultRenderBuffer;

    GLint gameuniforms[2];
    GLint bluruniforms[2];
    GLint blenduniforms[2];

    float width;
    float height;

    GLuint   vbos[32];
    uint32_t vbocounts[32];
    m4_t     projections[32];

    floatbuffer_t* dynamicbuffer;

} rnd;

void renderer_delete_framebuffers(
    void)
{
}

void renderer_create_framebuffers(
    void)
{
    float fulld = 1024;
    float glowd = 512;
    float blurd = 512;

    if (defaults.effects_level == 1)
    {
	fulld = 512;
	glowd = 256;
	blurd = 256;
    }

    ogl_framebuffer_with_texture(
	&rnd.fb_game,
	&rnd.tex_game,
	&rnd.depth_game,
	fulld,
	fulld);

    ogl_framebuffer_with_texture(
	&rnd.fb_glow,
	&rnd.tex_glow,
	&rnd.depth_glow,
	glowd,
	glowd);

    ogl_framebuffer_with_texture(
	&rnd.fb_blur,
	&rnd.tex_blur,
	&rnd.depth_blur,
	blurd,
	blurd);
}

void renderer_init(
    GLuint defFrameBuffer,
    GLuint defRenderBuffer)
{
    bus_subscribe("CTL", renderer_onmessage);
    bus_subscribe("SCN", renderer_onmessage);
    bus_subscribe("MNU", renderer_onmessage);

    memset(rnd.vbos, 0, sizeof(GLuint) * 32);

    rnd.dynamicbuffer = floatbuffer_alloc();

    rnd.defaultFrameBuffer  = defFrameBuffer;
    rnd.defaultRenderBuffer = defRenderBuffer;

    rnd.shader_glow = ogl_shader_create(
	glow_vsh,
	glow_fsh,
	uniforms_game,
	attributes_game,
	rnd.gameuniforms);

    rnd.shader_blur = ogl_shader_create(
	blur_vsh,
	blur_fsh,
	uniforms_blur,
	attributes_blur,
	rnd.bluruniforms);

    rnd.shader_blend = ogl_shader_create(
	blend_vsh,
	blend_fsh,
	uniforms_blend,
	attributes_blend,
	rnd.blenduniforms);

    ogl_vertexbuffer(&rnd.vbo_main);

    renderer_create_framebuffers();

    glBindRenderbuffer(
	GL_RENDERBUFFER,
	rnd.defaultRenderBuffer);
}

void renderer_free(
    void)
{
    if (rnd.dynamicbuffer) REL(rnd.dynamicbuffer);
    rnd.dynamicbuffer = NULL;
}

void renderer_reset_buffers(
    void)
{
    for (int index = 0;
	 index < 32;
	 index++)
    {

	rnd.vbocounts[index] = 0;
    }
}

void renderer_update_buffer(
    GLfloat* vertexes,
    uint32_t count,
    m4_t     projection,
    int      level)
{
    if (rnd.vbos[level] == 0)
    {
	GLuint name;
	glGenBuffers(1, &name);

	rnd.vbos[level]      = name;
	rnd.vbocounts[level] = 0;
    }

    rnd.vbocounts[level]   = count;
    rnd.projections[level] = projection;

    if (count > 0)
    {
	glBindBuffer(GL_ARRAY_BUFFER, rnd.vbos[level]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * count, vertexes, GL_DYNAMIC_DRAW);
    }
}

void renderer_update_matrix(
    m4_t projection,
    int  level)
{
    rnd.projections[level] = projection;
}

void renderer_draw_unit_quad(
    void)
{
    GLfloat vertexes[] =
	{

	    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f

	};

    glBindBuffer(
	GL_ARRAY_BUFFER,
	rnd.vbo_main);

    ogl_enableva_f3f2();

    glBufferData(
	GL_ARRAY_BUFFER,
	sizeof(GLfloat) * 6 * 5,
	vertexes,
	GL_DYNAMIC_DRAW);

    glDrawArrays(
	GL_TRIANGLES,
	0,
	6);
}

void renderer_render_scene(
    int* levelmap,
    int* drawmap)
{
    for (int index = 0;
	 index < levelmap[0];
	 index++)
    {

	int level = levelmap[1 + index];
	int draw  = drawmap[1 + index];

	glUniform1i(rnd.gameuniforms[1], 1 - draw); // clear non-drawable shapes from glow buffer

	// if ( level == 3 ) glClear( GL_DEPTH_BUFFER_BIT );	// reset depth buffer when drawing menu

	matrix4array_t projection;
	projection.matrix = rnd.projections[level];

	glUniformMatrix4fv(
	    rnd.gameuniforms[0],
	    1,
	    0,
	    projection.array);

	if (rnd.vbocounts[level] > 0)
	{
	    glBindBuffer(
		GL_ARRAY_BUFFER,
		rnd.vbos[level]);

	    ogl_enableva_f3u4();

	    glDrawArrays(
		GL_TRIANGLE_STRIP,
		0,
		rnd.vbocounts[level] / 4);
	}
    }
}

void renderer_render(
    void)
{
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    int levelmap[]      = {6, 0, 1, 2, 3, 4, 5};
    int drawmap[]       = {6, 1, 1, 1, 1, 1, 1};
    int levelmap_glow[] = {4, 2, 3, 4, 5};
    int drawmap_glow[]  = {4, 1, 1, 1, 1};

    float fulld = 1024;
    float glowd = 512;
    float blurd = 512;

    if (defaults.effects_level == 1)
    {
	fulld = 512;
	glowd = 256;
	blurd = 256;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glBlendFunc(
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA);

    // render scene

    if (defaults.effects_level > 0)
    {

	glBindFramebuffer(
	    GL_FRAMEBUFFER,
	    rnd.fb_game);

	glViewport(
	    0,
	    0,
	    fulld,
	    fulld);
    }
    else
    {

	glBindFramebuffer(
	    GL_FRAMEBUFFER,
	    rnd.defaultFrameBuffer);

	glBindRenderbuffer(
	    GL_RENDERBUFFER,
	    rnd.defaultRenderBuffer);

	glViewport(
	    0.0,
	    0.0,
	    rnd.width,
	    rnd.height);
    }

    glClearColor(
	defaults.backcolor.x,
	defaults.backcolor.y,
	defaults.backcolor.z,
	1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(rnd.shader_glow);

    renderer_render_scene(
	levelmap,
	drawmap);

    if (defaults.effects_level > 0)
    {

	// render scene with full color if layer is glowing, with blank if not

	glBindFramebuffer(
	    GL_FRAMEBUFFER,
	    rnd.fb_glow);

	glClearColor(
	    0.0,
	    0.0,
	    0.0,
	    1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(
	    0.0,
	    0.0,
	    glowd,
	    glowd);

	renderer_render_scene(
	    levelmap_glow,
	    drawmap_glow);

	// render contents of glow framebuffer with horizontal blur shader into blur framebuffer

	glBindFramebuffer(
	    GL_FRAMEBUFFER,
	    rnd.fb_blur);

	glClearColor(
	    0.0,
	    0.0,
	    0.0,
	    1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(rnd.shader_blur);

	glViewport(
	    0.0,
	    0.0,
	    blurd,
	    blurd);

	glActiveTexture(GL_TEXTURE0 + 0);
	glUniform1i(rnd.bluruniforms[0], 0);
	glBindTexture(GL_TEXTURE_2D, rnd.tex_glow);

	glUniform1i(rnd.bluruniforms[1], 0); // draw horizontal direction

	renderer_draw_unit_quad();

	// render contents of blur framebuffer with vertical blur shader into glow framebuffer

	glBindFramebuffer(
	    GL_FRAMEBUFFER,
	    rnd.fb_glow);

	glClearColor(
	    0.0,
	    0.0,
	    0.0,
	    1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(
	    0.0,
	    0.0,
	    blurd,
	    blurd);

	glActiveTexture(GL_TEXTURE0 + 0);
	glUniform1i(rnd.bluruniforms[0], 0);
	glBindTexture(GL_TEXTURE_2D, rnd.tex_blur);

	glUniform1i(
	    rnd.bluruniforms[1],
	    1); // draw vertical direction

	renderer_draw_unit_quad();

	// blend game framebuffer and glow framebuffer together into default framebuffer with blend shader

	glBindFramebuffer(
	    GL_FRAMEBUFFER,
	    rnd.defaultFrameBuffer);

	glBindRenderbuffer(
	    GL_RENDERBUFFER,
	    rnd.defaultRenderBuffer);

	glClearColor(
	    0.0,
	    0.0,
	    0.0,
	    1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(rnd.shader_blend);

	glViewport(
	    0.0,
	    0.0,
	    rnd.width,
	    rnd.height);

	glUniform1i(rnd.blenduniforms[0], 0); // set textures
	glUniform1i(rnd.blenduniforms[1], 1); // set textures

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, rnd.tex_game);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, rnd.tex_glow);

	renderer_draw_unit_quad();
    }

    ogl_errors("renderer_init");
}

void renderer_resize(
    void* data)
{
    v2_t* newdimensions = data;

    rnd.width  = newdimensions->x;
    rnd.height = newdimensions->y;
}

void renderer_set_detail(
    void)
{
    renderer_delete_framebuffers();
    renderer_create_framebuffers();
}

void renderer_onmessage(
    const char* name,
    void*       data)
{
    if (strcmp(name, "UPDBUFF") == 0)
    {

	renderdata_t* rdata = (renderdata_t*) data;
	renderer_update_buffer(
	    rdata->buffer->data,
	    rdata->buffer->length,
	    rdata->buffer->projection,
	    rdata->level);
    }
    else if (strcmp(name, "UPDPRJ") == 0)
    {
	renderdata_t* rdata = (renderdata_t*) data;
	renderer_update_matrix(
	    rdata->buffer->projection, rdata->level);
    }
    else if (strcmp(name, "RENDER") == 0) renderer_render();
    else if (strcmp(name, "RESIZE") == 0) renderer_resize(data);
    else if (strcmp(name, "EFFECTS") == 0) renderer_set_detail();
}

#endif
