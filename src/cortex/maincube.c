#ifndef maincube_h
#define maincube_h

#include "math3.c"
#include "voxel.c"

typedef struct _maincube_t maincube_t;
struct _maincube_t
{

    v3_t    dir;
    char    visible;
    float   angle;
    voxel_t voxel;
};

void maincube_reset(
    float position,
    float size);

void maincube_set_direction(
    int   direction,
    float speed);

void maincube_update(
    float step,
    float position);

extern maincube_t maincube;

#endif

#if __INCLUDE_LEVEL__ == 0

maincube_t maincube;

void maincube_reset(
    float position,
    float size)
{

    voxel_t voxel =
	{
	    .model =
		{
		    0.0,
		    position,
		    -580.0,
		    size,
		    size,
		    size},

	    .colors =
		{
		    0xFFFFFFFF,
		    0xAAAAAAFF,
		    0x555555FF}};

    maincube.dir     = (v3_t){0};
    maincube.voxel   = voxel;
    maincube.angle   = 0.0;
    maincube.visible = 0;
}

void maincube_set_direction(
    int   direction,
    float speed)
{

    maincube.dir.x = (float) direction * speed;
}

void maincube_update(
    float step,
    float position)
{

    maincube.angle += .05 * step;
    if (maincube.angle > 2 * M_PI) maincube.angle -= 2 * M_PI;

    maincube.voxel.model.x = maincube.voxel.model.x + (float) maincube.dir.x * step;
    maincube.voxel.model.y = position;
    maincube.voxel.model.z = -600.0 + sinf(maincube.angle) * 20.0;
}

#endif
