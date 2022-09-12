
#ifndef floatbuffer_h
#define floatbuffer_h

#ifdef IOS
    #import <OpenGLES/ES2/gl.h>
    #import <OpenGLES/ES2/glext.h>
#elif defined OSX
    #include <OpenGL/gl3.h>
    #include <OpenGL/gl3ext.h>
#elif defined ANDROID
    #include <EGL/egl.h>
    #include <GLES/gl.h>
#elif defined __linux__

    #include <GL/glew.h>

    #include <GL/gl.h>
    #include <GL/glu.h>
#elif defined ASMJS
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#elif defined __MINGW32__
    #include <gl/gl.h>
    #include <gl/glu.h>
#endif

#include "math2.c"
#include "math3.c"
#include "math4.c"
#include "zc_memory.c"

typedef struct floatbuffer_t floatbuffer_t;
struct floatbuffer_t
{
    GLfloat* data;
    uint32_t length;
    uint32_t length_real;

    uint8_t changed;
    m4_t    projection;
};

floatbuffer_t* floatbuffer_alloc(void);
void           floatbuffer_dealloc(void* floatbuffer);
void           floatbuffer_reset(floatbuffer_t* floatbuffer);
void           floatbuffer_add(floatbuffer_t* floatbuffer, GLfloat data);
void           floatbuffer_add4(floatbuffer_t* floatbuffer, GLfloat dataa, GLfloat datab, GLfloat datac, GLfloat datad);
void           floatbuffer_addvector2(floatbuffer_t* floatbuffer, v2_t vector);
void           floatbuffer_addvector22(floatbuffer_t* floatbuffer, v2_t vectora, v2_t vectorb);
void           floatbuffer_addvector3(floatbuffer_t* floatbuffer, v3_t vector);
void           floatbuffer_setprojection(floatbuffer_t* floatbuffer, m4_t projection);

#endif

#if __INCLUDE_LEVEL__ == 0

/* alloc floatbuffer */

floatbuffer_t* floatbuffer_alloc()
{
    floatbuffer_t* floatbuffer = CAL(sizeof(floatbuffer_t), floatbuffer_dealloc, NULL);

    floatbuffer->data        = CAL(sizeof(GLfloat) * 10, NULL, NULL);
    floatbuffer->length      = 0;
    floatbuffer->length_real = 10;

    return floatbuffer;
}

/* dealloc floatbuffer */

void floatbuffer_dealloc(void* pointer)
{
    floatbuffer_t* floatbuffer = pointer;
    REL(floatbuffer->data);
}

/* resets floatbuffer */

void floatbuffer_reset(floatbuffer_t* floatbuffer)
{
    floatbuffer->length  = 0;
    floatbuffer->changed = 1;
}

/* internal use, expands internal storage size */

void floatbuffer_expand(floatbuffer_t* floatbuffer)
{
    assert(floatbuffer->length_real < UINT32_MAX / 2);
    floatbuffer->length_real *= 2;
    floatbuffer->data = mem_realloc(floatbuffer->data, sizeof(void*) * floatbuffer->length_real);
}

/* adds single float to buffer */

void floatbuffer_add(floatbuffer_t* floatbuffer, GLfloat value)
{
    if (floatbuffer->length == floatbuffer->length_real) floatbuffer_expand(floatbuffer);
    floatbuffer->data[floatbuffer->length] = value;
    floatbuffer->length += 1;
    floatbuffer->changed = 1;
}

/* adds four float to buffer */

void floatbuffer_add4(floatbuffer_t* floatbuffer, GLfloat dataa, GLfloat datab, GLfloat datac, GLfloat datad)
{
    if (floatbuffer->length + 4 >= floatbuffer->length_real) floatbuffer_expand(floatbuffer);
    floatbuffer->data[floatbuffer->length]     = dataa;
    floatbuffer->data[floatbuffer->length + 1] = datab;
    floatbuffer->data[floatbuffer->length + 2] = datac;
    floatbuffer->data[floatbuffer->length + 3] = datad;
    floatbuffer->length += 4;
    floatbuffer->changed = 1;
}

/* adds v2_t to floatbuffer */

void floatbuffer_addvector2(floatbuffer_t* floatbuffer, v2_t vector)
{
    if (floatbuffer->length + 2 >= floatbuffer->length_real) floatbuffer_expand(floatbuffer);
    floatbuffer->data[floatbuffer->length]     = vector.x;
    floatbuffer->data[floatbuffer->length + 1] = vector.y;
    floatbuffer->length += 2;
    floatbuffer->changed = 1;
}

/* adds 2 v2_t to floatbuffer */

void floatbuffer_addvector22(floatbuffer_t* floatbuffer, v2_t vectora, v2_t vectorb)
{
    if (floatbuffer->length + 4 >= floatbuffer->length_real) floatbuffer_expand(floatbuffer);
    floatbuffer->data[floatbuffer->length]     = vectora.x;
    floatbuffer->data[floatbuffer->length + 1] = vectora.y;
    floatbuffer->data[floatbuffer->length + 2] = vectorb.x;
    floatbuffer->data[floatbuffer->length + 3] = vectorb.y;
    floatbuffer->length += 4;
    floatbuffer->changed = 1;
}

/* adds v3_t to floatbuffer */

void floatbuffer_addvector3(floatbuffer_t* floatbuffer, v3_t vector)
{
    if (floatbuffer->length + 3 >= floatbuffer->length_real) floatbuffer_expand(floatbuffer);
    floatbuffer->data[floatbuffer->length]     = vector.x;
    floatbuffer->data[floatbuffer->length + 1] = vector.y;
    floatbuffer->data[floatbuffer->length + 2] = vector.z;
    floatbuffer->length += 3;
    floatbuffer->changed = 1;
}

void floatbuffer_setprojection(floatbuffer_t* floatbuffer, m4_t projection)
{
    floatbuffer->projection = projection;
}

#endif
