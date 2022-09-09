#ifndef physics2_h
#define physics2_h

#include "math2.c"
#include "mtvec.c"
#include <math.h>
#include <stdio.h>

typedef struct _mass2_t mass2_t;
struct _mass2_t
{
    v2_t trans;
    v2_t basis;

    float weight;
    float radius;
    float elasticity;

    mtvec_t* segmentgroups;
};

mass2_t* mass2_alloc(v2_t position, float radius, float weight, float elasticity);

typedef struct _dguard2_t dguard2_t;
struct _dguard2_t
{
    mass2_t* mass_a;
    mass2_t* mass_b;

    float distance;
    float elasticity;

    float ratio_a;
    float ratio_b;
};

dguard2_t* dguard2_alloc(mass2_t* mass_a, mass2_t* mass_b, float distance, float elasticity);
void       dguard2_resetdistance(dguard2_t* dguard);
void       dguard2_new(dguard2_t* dguard);

typedef struct _aguard2_t aguard2_t;
struct _aguard2_t
{
    mass2_t* mass_a;
    mass2_t* mass_b;
    mass2_t* mass_c;

    float angle_min;
    float angle_max;

    float ratio_a;
    float ratio_c;
};

aguard2_t* aguard2_alloc(mass2_t* mass_a, mass2_t* mass_b, mass2_t* mass_c, float angle_min, float angle_max);
void       aguard2_new(aguard2_t* guard);

typedef struct _surfaces_t surfaces_t;
struct _surfaces_t
{
    mtvec_t** strips;
    mtvec_t*  groups;

    uint32_t size;
    float    stripwidth;
};

surfaces_t* surfaces_alloc(uint32_t size, float stripwidth);
void        surfaces_reset(surfaces_t* surfaces);
void        surfaces_addsegment(surfaces_t* surfaces, segment2_t* segment);
void        surfaces_segments_for_mtvec_and_threshold(surfaces_t* surfaces, v2_t trans, v2_t basis, float threshold);

typedef struct _physics2_collision_t physics2_collision_t;
struct _physics2_collision_t
{
    int        count;
    v2_t       isps[10];
    segment2_t segments[10];
};

void physics2_set_gravity(mtvec_t* masses, v2_t gravity);
void physics2_set_distances(mtvec_t* dguards);
void physics2_set_angles(mtvec_t* aguards);
void physics2_set_positions(mtvec_t* masses, surfaces_t* surfaces);
void physics2_new_mass_position(mass2_t* mass, surfaces_t* surfaces);
void physics2_collect_intersecting_surfaces(surfaces_t* surfaces, v2_t trans, v2_t basis, float radius, physics2_collision_t* collision);
void physics2_collect_intersecting_and_nearby_surfaces(surfaces_t* surfaces, v2_t trans, v2_t basis, float radius, physics2_collision_t* collision);

#endif

#if __INCLUDE_LEVEL__ == 0

#include "mtmem.c"
#include <float.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
    #define M_PI_2 3.14159265358979323846 * 2
#endif

void mass2_dealloc(void* pointer);
void surfaces_dealloc(void* pointer);

/* creates mass point */

mass2_t* mass2_alloc(v2_t position, float radius, float weight, float elasticity)
{
    mass2_t* result = mtmem_calloc(sizeof(mass2_t), mass2_dealloc);

    result->trans = position;
    result->basis = v2_init(0.0, 0.0);

    result->radius     = radius;
    result->weight     = weight;
    result->elasticity = elasticity;

    result->segmentgroups = mtvec_alloc();

    return result;
}

/* dealloc mass point */

void mass2_dealloc(void* pointer)
{
    mass2_t* mass = pointer;
    mtmem_release(mass->segmentgroups);
}

/* creates distance guard */

dguard2_t* dguard2_alloc(mass2_t* mass_a, mass2_t* mass_b, float distance, float elasticity)
{
    float sumweight = mass_a->weight + mass_b->weight;

    dguard2_t* dguard = mtmem_calloc(sizeof(dguard2_t), NULL);

    dguard->mass_a     = mass_a;
    dguard->mass_b     = mass_b;
    dguard->distance   = distance;
    dguard->elasticity = elasticity;
    dguard->ratio_a    = mass_b->weight / sumweight;
    dguard->ratio_b    = mass_a->weight / sumweight;

    return dguard;
}

/* resets distance based on actual mass positions */

void dguard2_resetdistance(dguard2_t* dguard)
{
    v2_t vector      = v2_sub(dguard->mass_b->trans, dguard->mass_a->trans);
    dguard->distance = v2_length(vector);
}

/* adds forces to masses in dguard to keep up distance */

void dguard2_new(dguard2_t* dguard)
{
    v2_t transa = v2_add(dguard->mass_a->trans, dguard->mass_a->basis);
    v2_t transb = v2_add(dguard->mass_b->trans, dguard->mass_b->basis);

    v2_t connector = v2_sub(transa, transb);

    float delta = v2_length(connector) - dguard->distance;

    if (dguard->elasticity > 0.0) delta /= dguard->elasticity;

    assert(!isinf(delta));

    if (fabs(delta) > 0.01)
    {
	v2_t a                = v2_resize(connector, -delta * dguard->ratio_a);
	v2_t b                = v2_resize(connector, delta * dguard->ratio_b);
	dguard->mass_a->basis = v2_add(dguard->mass_a->basis, a);
	dguard->mass_b->basis = v2_add(dguard->mass_b->basis, b);
    }
}

/* creates angle guard */

aguard2_t* aguard2_alloc(mass2_t* mass_a, mass2_t* mass_b, mass2_t* mass_c, float angle_min, float angle_max)
{
    float sumweight = mass_a->weight + mass_c->weight;

    aguard2_t* guard = mtmem_calloc(sizeof(aguard2_t), NULL);

    guard->mass_a = mass_a;
    guard->mass_b = mass_b;
    guard->mass_c = mass_c;

    guard->angle_min = angle_min;
    guard->angle_max = angle_max;

    guard->ratio_a = mass_c->weight / sumweight;
    guard->ratio_c = mass_a->weight / sumweight;

    return guard;
}

/* adds forces to masses in aguard to keep up minimal and maximal angles */

void aguard2_new(aguard2_t* guard)
{
    // calculate incoming positions

    v2_t transa = v2_add(guard->mass_a->trans, guard->mass_a->basis);
    v2_t transb = v2_add(guard->mass_b->trans, guard->mass_b->basis);
    v2_t transc = v2_add(guard->mass_c->trans, guard->mass_c->basis);

    // calculate actual angles

    v2_t ba = v2_sub(transa, transb);
    v2_t bc = v2_sub(transc, transb);

    float angleba = v2_angle_x(ba);
    float anglebc = v2_angle_x(bc);

    // get difference angle calculated counterclockwise ( base circle )

    float anglere = anglebc - angleba;
    if (anglere < 0.0) anglere += 2 * M_PI;

    // check borders

    if (anglere < guard->angle_min || anglere > guard->angle_max)
    {
	float diffmin = guard->angle_min - anglere; // CCW delta
	float diffmax = anglere - guard->angle_max; // CCW delta

	// convert to sub-180 pies if needed

	if (diffmin < 0.0) diffmin += 2 * M_PI;
	if (diffmax < 0.0) diffmax += 2 * M_PI;

	// modify angles

	if (diffmin < diffmax)
	{
	    angleba -= diffmin * guard->ratio_a;
	    anglebc += diffmin * guard->ratio_c;
	}
	else
	{
	    angleba += diffmax * guard->ratio_a;
	    anglebc -= diffmax * guard->ratio_c;
	}

	// calculate rotated ba and bc endpoints

	float balength = v2_length(ba);
	float bclength = v2_length(bc);

	ba.x = transb.x + cosf(angleba) * balength;
	ba.y = transb.y + sinf(angleba) * balength;

	bc.x = transb.x + cosf(anglebc) * bclength;
	bc.y = transb.y + sinf(anglebc) * bclength;

	// calculate forces, b will move backwards because we rotate ba and bc around their centers

	v2_t force_a = v2_scale(v2_sub(ba, transa), 0.5);
	v2_t force_c = v2_scale(v2_sub(bc, transc), 0.5);
	v2_t force_b = v2_scale(v2_add(force_a, force_c), -0.5);

	// update basises

	guard->mass_a->basis = v2_add(guard->mass_a->basis, force_a);
	guard->mass_b->basis = v2_add(guard->mass_b->basis, force_b);
	guard->mass_c->basis = v2_add(guard->mass_c->basis, force_c);
    }
}

/* creates default surfaces */

surfaces_t* surfaces_alloc(uint32_t size, float stripwidth)
{
    surfaces_t* result = mtmem_calloc(sizeof(surfaces_t), surfaces_dealloc);

    result->size       = size;
    result->stripwidth = stripwidth;
    result->strips     = mtmem_calloc(sizeof(mtvec_t) * size, NULL);
    result->groups     = mtvec_alloc();

    for (int index = 0; index < size; index++) result->strips[index] = mtvec_alloc();

    return result;
}

/* dealloc surfaces */

void surfaces_dealloc(void* pointer)
{
    surfaces_t* surfaces = pointer;

    for (int index = 0; index < surfaces->size; index++) mtmem_release(surfaces->strips[index]);

    mtmem_release(surfaces->strips);
    mtmem_release(surfaces->groups);
}

/* reset surfaces */

void surfaces_reset(surfaces_t* surfaces)
{
    for (int index = 0; index < surfaces->size; index++) mtvec_reset(surfaces->strips[index]);

    mtvec_reset(surfaces->groups);
}

/* adds segment to surfaces */

void surfaces_addsegment(surfaces_t* surfaces, segment2_t* segment)
{
    float minx, maxx;

    if (segment->basis.x < 0.0)
    {
	minx = segment->trans.x + segment->basis.x;
	maxx = segment->trans.x;
    }
    else
    {
	minx = segment->trans.x;
	maxx = segment->trans.x + segment->basis.x;
    }

    int16_t mincol = floorf(minx / surfaces->stripwidth);
    int16_t maxcol = floorf(maxx / surfaces->stripwidth);

    assert(mincol >= 0 && maxcol < 100);

    for (int colindex = mincol; colindex <= maxcol; colindex++)
    {
	mtvec_t* group = surfaces->strips[colindex];
	mtvec_add(group, segment);
    }
}

/* returns surrounding segments for vector */

void surfaces_segments_for_mtvec_and_threshold(surfaces_t* surfaces, v2_t trans, v2_t basis, float threshold)
{
    if (surfaces->size == 0) return;
    float minx, maxx;

    if (basis.x < 0.0)
    {
	minx = trans.x + basis.x;
	maxx = trans.x;
    }
    else
    {
	minx = trans.x;
	maxx = trans.x + basis.x;
    }

    minx -= threshold;
    maxx += threshold;

    int16_t mincol = floorf(minx / surfaces->stripwidth);
    int16_t maxcol = floorf(maxx / surfaces->stripwidth);

    if (mincol >= 0 && maxcol < 100)
    {
	for (int index = mincol; index <= maxcol; index++)
	{
	    mtvec_add(surfaces->groups, surfaces->strips[index]);
	}
    }
}

/* collects colliding segments for given inerta's trans and basis TODO unify these two */

void physics2_collect_intersecting_surfaces(surfaces_t* surfaces, v2_t trans, v2_t basis, float radius, physics2_collision_t* collision)
{
    /* zero vectors fuck up everything */

    if (fabs(basis.x) > 0.0 || fabs(basis.y) > 0.0)
    {
	surfaces_segments_for_mtvec_and_threshold(surfaces, trans, basis, radius);

	for (int gindex = 0; gindex < surfaces->groups->length; gindex++)
	{
	    mtvec_t* group = surfaces->groups->data[gindex];

	    for (int sindex = 0; sindex < group->length; sindex++)
	    {
		segment2_t* segment = group->data[sindex];

		if (v2_box_intersect(basis, trans, segment->basis, segment->trans, radius) == 1)
		{
		    v2_t isp = v2_intersect_with_proximity(trans, basis, segment->trans, segment->basis, radius);

		    if (isp.x != FLT_MAX)
		    {
			collision->isps[collision->count]     = isp;
			collision->segments[collision->count] = *segment;
			collision->count++;
		    }
		}
	    }
	}

	mtvec_reset(surfaces->groups);
    }
}

/* collects colliding segments for given inerta's trans and basis */

void physics2_collect_intersecting_and_nearby_surfaces(surfaces_t* surfaces, v2_t trans, v2_t basis, float radius, physics2_collision_t* collision)
{
    /* zero vectors fuck up everything */

    if (fabs(basis.x) > 0.0 || fabs(basis.y) > 0.0)
    {
	surfaces_segments_for_mtvec_and_threshold(surfaces, trans, basis, radius);

	for (int gindex = 0; gindex < surfaces->groups->length; gindex++)
	{
	    mtvec_t* group = surfaces->groups->data[gindex];

	    for (int sindex = 0; sindex < group->length; sindex++)
	    {
		segment2_t* segment = group->data[sindex];

		if (v2_box_intersect(basis, trans, segment->basis, segment->trans, radius) == 1)
		{
		    v2_t isp = v2_intersect_with_nearby(trans, basis, segment->trans, segment->basis, radius);

		    if (isp.x != FLT_MAX)
		    {
			collision->isps[collision->count]     = isp;
			collision->segments[collision->count] = *segment;
			collision->count++;
		    }
		}
	    }
	}

	mtvec_reset(surfaces->groups);
    }
}

/* pushes colliding segment towards mass center with radius and adds them to the collision struct */

segment2_t physics2_move_segment_towards_point(v2_t point, segment2_t segment, float radius)
{
    v2_t isp = v2_intersect_lines(point, v2_rotate_90_left(segment.basis), segment.trans, segment.basis);
    v2_t vec = v2_sub(point, isp);

    vec = v2_resize(vec, radius);

    segment.trans = v2_add(segment.trans, vec);

    return segment;
}

/* moves mass to new position defined by inertia basis and checks surface collision */

void physics2_new_mass_position(mass2_t* mass, surfaces_t* surfaces)
{
    // we have to store actual inertia with every collosion fragment
    segment2_t inertia = {.trans = mass->trans, .basis = mass->basis};

    // we allow only 4 collision iterations, after it we reset actual inertia
    int iterations = 0;

    while (1)
    {
	physics2_collision_t collision = {0};
	physics2_collect_intersecting_and_nearby_surfaces(surfaces, inertia.trans, inertia.basis, mass->radius, &collision);

	// check ending conditions
	if (collision.count == 0)
	{
	    break;
	}
	else if (collision.count == 1)
	{
	    collision.segments[0] = physics2_move_segment_towards_point(inertia.trans, collision.segments[0], mass->radius);

	    // mirror original basis and reduce with elasticity
	    mass->basis = v2_scale(v2_mirror(collision.segments[0].basis, mass->basis), mass->elasticity);

	    // calculate new fragment of base
	    inertia = v2_collide_and_fragment(inertia.trans, inertia.basis, collision.segments[0].trans, collision.segments[0].basis);

	    // stop if segment is not too steep
	    if (fabs(v2_angle_x(collision.segments[0].basis)) < 0.23)
	    {
		inertia.basis = v2_init(0.0, 0.0);
		break;
	    }
	}
	else if (collision.count > 1)
	{
	    collision.segments[0] = physics2_move_segment_towards_point(inertia.trans, collision.segments[0], mass->radius);
	    collision.segments[1] = physics2_move_segment_towards_point(inertia.trans, collision.segments[1], mass->radius);

	    // sum basises and reduce with elasticity
	    v2_t new_basis_a = v2_scale(v2_mirror(collision.segments[0].basis, inertia.basis), mass->elasticity);
	    v2_t new_basis_b = v2_scale(v2_mirror(collision.segments[1].basis, inertia.basis), mass->elasticity);

	    mass->basis = v2_scale(v2_add(new_basis_a, new_basis_b), 0.5);

	    // new trans is intersection point of the colliding segments
	    inertia.basis = mass->basis;
	    v2_t newtrans = v2_intersect_lines(collision.segments[0].trans, collision.segments[0].basis, collision.segments[1].trans, collision.segments[1].basis);

	    if (newtrans.x != FLT_MAX) inertia.trans = newtrans;
	    else break;

	    // stop if basis is too small
	    if (v2_length(mass->basis) <= mass->radius)
	    {
		inertia.basis = v2_init(0.0, 0.0);
		break;
	    }
	}

	iterations++;

	// too many iterations without result ( narrow bottom? ), exiting with a reset basis
	if (iterations == 4 && collision.count > 0)
	{
	    inertia.trans = mass->trans;
	    inertia.basis = v2_init(0.0, 0.0);
	    break;
	}
    }
    // end of while true

    // add remaining part of inertia/full inertia
    mass->trans = v2_add(inertia.trans, inertia.basis);
}

/* adds gravity to masses */

void physics2_set_gravity(mtvec_t* masses, v2_t gravity)
{
    for (int massindex = 0;
	 massindex < masses->length;
	 massindex++)
    {
	mass2_t* mass = masses->data[massindex];
	mass->basis   = v2_add(mass->basis, gravity);
    }
}

/* adds distance keeping forces */

void physics2_set_distances(mtvec_t* dguards)
{
    for (int dguardindex = 0;
	 dguardindex < dguards->length;
	 dguardindex++)
    {
	dguard2_t* dguard = dguards->data[dguardindex];
	dguard2_new(dguard);
    }
}

/* adds angle keeping forces */

void physics2_set_angles(mtvec_t* aguards)
{
    for (int index = 0;
	 index < aguards->length;
	 index++)
    {
	aguard2_t* guard = aguards->data[index];
	aguard2_new(guard);
    }
}

/* sets final position of masses */

void physics2_set_positions(mtvec_t* masses, surfaces_t* surfaces)
{
    for (int index = 0; index < masses->length; index++)
    {
	mass2_t* mass = masses->data[index];
	physics2_new_mass_position(mass, surfaces);
    }
}

#endif
