#ifndef particle_h
#define particle_h

#include "voxel.c"
#include <stdio.h>

typedef struct _particle_t
{

    v3_t    dir;
    char    finished;
    voxel_t model;

} particle_t;

particle_t* particle_create(
    voxel_t voxel,
    v3_t    direction);

void particle_update(
    particle_t* particle,
    float       ratio);

#endif

#if __INCLUDE_LEVEL__ == 0 // if we are compiled as implementation

particle_t* particle_create(
    voxel_t voxel,
    v3_t    direction)
{

    particle_t* particle = mtmem_calloc(
	sizeof(particle_t),
	NULL);

    particle->dir   = direction;
    particle->model = voxel;

    return particle;
}

void particle_update(
    particle_t* particle,
    float       ratio)
{

    particle->model.model.x += particle->dir.x * ratio;
    particle->model.model.y += particle->dir.y * ratio;
    particle->model.model.z += particle->dir.z * ratio;
}

#endif
