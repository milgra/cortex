#ifndef voxel_h
#define voxel_h

#include "floatbuffer.c"

#include "zc_util2.c"
#include "zc_vec2.c"

#include "math3.c"
#include "math4.c"
#include "ogl.c"

typedef struct _voxel_t voxel_t;
struct _voxel_t
{
    vox_t    model;
    v3_t     points[8];
    uint32_t colors[3];
};

void voxel_set_color(
    voxel_t* voxel,
    v4_t     topcolor,
    v4_t     midcolor,
    v4_t     btmcolor);

void voxel_update(
    voxel_t* voxel);

v3_t voxel_intersect(
    voxel_t* voxel,
    v3_t     line_a,
    v3_t     line_b,
    char*    side);

void voxel_collect_points(
    voxel_t*       voxel,
    floatbuffer_t* buffer);

#endif

#if __INCLUDE_LEVEL__ == 0

uint32_t voxel_color_from_floats(float r, float g, float b, float a)
{
    if (r > 1.0) r = 1.0;
    if (g > 1.0) g = 1.0;
    if (b > 1.0) b = 1.0;
    if (a > 1.0) a = 1.0;
    if (r < 0.0) r = 0.0;
    if (g < 0.0) g = 0.0;
    if (b < 0.0) b = 0.0;
    if (a < 0.0) a = 0.0;

    return (uint8_t) (r * 255.0) << 24 |
	   (uint8_t) (g * 255.0) << 16 |
	   (uint8_t) (b * 255.0) << 8 |
	   (uint8_t) (a * 255.0);
}

void voxel_set_color(
    voxel_t* voxel,
    v4_t     topcolor,
    v4_t     midcolor,
    v4_t     btmcolor)
{

    voxel->colors[0] = voxel_color_from_floats(topcolor.x, topcolor.y, topcolor.z, topcolor.w);
    voxel->colors[1] = voxel_color_from_floats(midcolor.x, midcolor.y, midcolor.z, midcolor.w);
    voxel->colors[2] = voxel_color_from_floats(btmcolor.x, btmcolor.y, btmcolor.z, btmcolor.w);
}

//    y
//    ^
//    |
//      4---5   z
//     /|  /|  ^
//    0---1 | /
//    | | | |
//    | 6-|-7
//    |/  |/
//    2---3  -> x
//
// triangle strip 2-3-0-1-7-5-6-4-2-0

void voxel_update(
    voxel_t* voxel)
{
    v3_t a = {voxel->model.x, voxel->model.y, voxel->model.z};
    v3_t b = {voxel->model.x + voxel->model.w, voxel->model.y, voxel->model.z};
    v3_t c = {voxel->model.x, voxel->model.y - voxel->model.h, voxel->model.z};
    v3_t d = {voxel->model.x + voxel->model.w, voxel->model.y - voxel->model.h, voxel->model.z};

    v3_t e = {voxel->model.x, voxel->model.y, voxel->model.z - voxel->model.d};
    v3_t f = {voxel->model.x + voxel->model.w, voxel->model.y, voxel->model.z - voxel->model.d};
    v3_t g = {voxel->model.x, voxel->model.y - voxel->model.h, voxel->model.z - voxel->model.d};
    v3_t h = {voxel->model.x + voxel->model.w, voxel->model.y - voxel->model.h, voxel->model.z - voxel->model.d};

    voxel->points[0] = a;
    voxel->points[1] = b;
    voxel->points[2] = c;
    voxel->points[3] = d;

    voxel->points[4] = e;
    voxel->points[5] = f;
    voxel->points[6] = g;
    voxel->points[7] = h;
}

v3_t voxel_intersect(
    voxel_t* voxel,
    v3_t     line_a,
    v3_t     line_b,
    char*    side)
{

    voxel_update(voxel);

    v3_t result = {0};

    if (strcmp(side, "03") == 0)
    {

	result = v4_quadlineintersection(
	    *((v4_t*) &voxel->points[0]),
	    *((v4_t*) &voxel->points[1]),
	    *((v4_t*) &voxel->points[2]),
	    line_a,
	    line_b);
    }
    else if (strcmp(side, "27") == 0)
    {

	result = v4_quadlineintersection(
	    *((v4_t*) &voxel->points[2]),
	    *((v4_t*) &voxel->points[3]),
	    *((v4_t*) &voxel->points[6]),
	    line_a,
	    line_b);
    }

    return result;
}

//    y
//    ^
//    |
//      4---5   z
//     /|  /|  ^
//    0---1 | /
//    | | | |
//    | 6-|-7
//    |/  |/
//    2---3  -> x
//
// triangle strip 0-0-0-2-1-3-3-3  5-1-7-3  6-2-4-0-5-1-1-1  6-6-6-4-7-5-5-5

void voxel_collect_points(
    voxel_t*       voxel,
    floatbuffer_t* buffer)
{

    // TODO REMOVE!!!
    voxel_update(voxel);

    // degenerate first

    floatbuffer_addvector3(buffer, voxel->points[0]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[0]));

    floatbuffer_addvector3(buffer, voxel->points[0]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[0]));

    floatbuffer_addvector3(buffer, voxel->points[0]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[0]));

    floatbuffer_addvector3(buffer, voxel->points[2]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[0]));

    floatbuffer_addvector3(buffer, voxel->points[1]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[0]));

    // degenerate

    floatbuffer_addvector3(buffer, voxel->points[3]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[0]));

    floatbuffer_addvector3(buffer, voxel->points[3]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[0]));

    floatbuffer_addvector3(buffer, voxel->points[3]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[1]));

    floatbuffer_addvector3(buffer, voxel->points[5]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[2]));

    floatbuffer_addvector3(buffer, voxel->points[1]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[1]));

    floatbuffer_addvector3(buffer, voxel->points[7]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[2]));

    floatbuffer_addvector3(buffer, voxel->points[3]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[1]));

    floatbuffer_addvector3(buffer, voxel->points[6]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[2]));

    floatbuffer_addvector3(buffer, voxel->points[2]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[1]));

    floatbuffer_addvector3(buffer, voxel->points[4]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[2]));

    floatbuffer_addvector3(buffer, voxel->points[0]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[1]));

    floatbuffer_addvector3(buffer, voxel->points[5]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[2]));

    // degenerate last

    floatbuffer_addvector3(buffer, voxel->points[1]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[1]));

    floatbuffer_addvector3(buffer, voxel->points[1]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[1]));

    //        floatbuffer_addvector3( buffer , voxel->points[1] );
    //        floatbuffer_add( buffer , ogl_color_float_from_rgbauint32( voxel->colors[1] ) );

    // degenerate first

    //        floatbuffer_addvector3( buffer , voxel->points[6] );
    //        floatbuffer_add( buffer , ogl_color_float_from_rgbauint32( voxel->colors[6] ) );

    floatbuffer_addvector3(buffer, voxel->points[6]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[2]));

    floatbuffer_addvector3(buffer, voxel->points[6]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[2]));

    floatbuffer_addvector3(buffer, voxel->points[4]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[2]));

    floatbuffer_addvector3(buffer, voxel->points[7]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[2]));

    // degenerate last

    floatbuffer_addvector3(buffer, voxel->points[5]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[2]));

    floatbuffer_addvector3(buffer, voxel->points[5]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[2]));

    floatbuffer_addvector3(buffer, voxel->points[5]);
    floatbuffer_add(buffer, ogl_color_float_from_rgbauint32(voxel->colors[2]));
}

#endif
