/* Level generator, generates blocks in two switching fixed height segments */

#ifndef generator_h
#define generator_h

#include "excavator.c"
#include "voxel.c"
#include <stdio.h>

#define ROWCOUNT 65
#define COLCOUNT 40
#define BLOCKSIZE 30.0
#define PLAYCUBEY 1000.0
#define SWAPDISTANCE 200.0
#define HALFSCENE BLOCKSIZE* COLCOUNT / 2.0

typedef struct _generator_t generator_t;
struct _generator_t
{
    voxel_t prevlinecubes[COLCOUNT]; // stores generated cubes for the last generated line
    char    prevlinechars[COLCOUNT]; // stores last generated line info ( char string (

    voxel_t bubbles[ROWCOUNT][2]; // stores bubbles

    excavator_t* excavator_a;
    excavator_t* excavator_b;
    excavator_t* excavator_c;
    excavator_t* excavator_d;

    v4_t color;
    v4_t newcolor;
};

void generator_init(
    void);

void generator_free(
    void);

void generator_reset(
    void);

void generator_setup(
    map_t* settings);

void generator_generate_row(
    int   row,
    float topposition);

extern generator_t generator;

#endif /* generator_h */

#if __INCLUDE_LEVEL__ == 0

generator_t generator;

#include "generator.c"
#include "zc_string.c"
#include "zc_vec4.c"
#include <string.h>

void generator_init(
    void)
{
    generator.excavator_a = excavator_alloc(COLCOUNT);
    generator.excavator_b = excavator_alloc(COLCOUNT);
    generator.excavator_c = excavator_alloc(COLCOUNT);
    generator.excavator_d = excavator_alloc(COLCOUNT);
}

void generator_free(
    void)
{
    REL(generator.excavator_a);
    REL(generator.excavator_b);
    REL(generator.excavator_c);
    REL(generator.excavator_d);
}

void generator_reset(
    void)
{
    excavator_reset(generator.excavator_a, COLCOUNT);
    excavator_reset(generator.excavator_b, COLCOUNT);
    excavator_reset(generator.excavator_c, COLCOUNT);
    excavator_reset(generator.excavator_d, COLCOUNT);

    memset(
	generator.prevlinechars,
	0,
	sizeof(generator.prevlinechars));

    memset(
	generator.prevlinecubes,
	0,
	sizeof(generator.prevlinecubes));

    generator.color    = (v4_t){0};
    generator.newcolor = (v4_t){0};
}

/* load values from script */

void generator_setup(
    map_t* settings)
{
    str_t* red = MGET(
	settings,
	"red");

    str_t* blue = MGET(
	settings,
	"blue");

    str_t* green = MGET(
	settings,
	"green");

    str_t* diffA = MGET(
	settings,
	"diffA");

    str_t* diffB = MGET(
	settings,
	"diffB");

    str_t* diffC = MGET(
	settings,
	"diffC");

    str_t* diffD = MGET(
	settings,
	"diffD");

    str_t* sizeA = MGET(
	settings,
	"widthA");

    str_t* sizeB = MGET(
	settings,
	"widthB");

    str_t* sizeC = MGET(
	settings,
	"widthC");

    str_t* sizeD = MGET(
	settings,
	"widthD");

    str_t* modeA = MGET(
	settings,
	"modeA");

    str_t* modeB = MGET(
	settings,
	"modeB");

    str_t* modeC = MGET(
	settings,
	"modeC");

    str_t* modeD = MGET(
	settings,
	"modeD");

    if (diffA != NULL) generator.excavator_a->eff_diff = str_floatvalue(diffA);
    if (diffB != NULL) generator.excavator_b->eff_diff = str_floatvalue(diffB);
    if (diffC != NULL) generator.excavator_c->eff_diff = str_floatvalue(diffC);
    if (diffD != NULL) generator.excavator_d->eff_diff = str_floatvalue(diffD);

    if (sizeA != NULL) generator.excavator_a->new_line_width = str_intvalue(sizeA);
    if (sizeB != NULL) generator.excavator_b->new_line_width = str_intvalue(sizeB);
    if (sizeC != NULL) generator.excavator_c->new_line_width = str_intvalue(sizeC);
    if (sizeD != NULL) generator.excavator_d->new_line_width = str_intvalue(sizeD);

    if (modeA != NULL) excavator_setmode(
	generator.excavator_a,
	modeA);

    if (modeB != NULL) excavator_setmode(
	generator.excavator_b,
	modeB);

    if (modeC != NULL) excavator_setmode(
	generator.excavator_c,
	modeC);

    if (modeD != NULL) excavator_setmode(
	generator.excavator_d,
	modeD);

    if (red != NULL) generator.newcolor.x = str_floatvalue(red);
    if (blue != NULL) generator.newcolor.z = str_floatvalue(blue);
    if (green != NULL) generator.newcolor.y = str_floatvalue(green);
}

/* generates z value based on surrounding z values */

float generator_generatez(
    char  edge,
    char  inside,
    float currentz,
    float prevlinecubez,
    float prevcubez)
{
    float z = -3000.0;

    if (edge)
    {
	z = -600.0;
    }
    else if (inside == 1)
    {
	// fall down from previous line
	if (prevlinecubez < 0.0) z = prevlinecubez - (float) (rand() % 80);

	if (z > -3000.0)
	{
	    // if fallen prev line, fall with an average
	    z = (z + prevcubez - (float) (rand() % 80)) / 2.0;
	}
	else
	{
	    // if not, fall down from previous cube
	    z = prevcubez - (float) (rand() % 80);
	}

	if (z < 0.0 && z > -2000.0 && z > currentz)
	{
	    // if already set, do nothing
	}
	else z = -3000.0;
    }

    return z;
}

/* generate next segment */

void generator_generate_row(
    int   row,
    float topposition)
{
    /* transform to wanted color */

    v4_t sub = v4_sub(
	generator.newcolor,
	generator.color);

    v4_t mul = v4_scale(
	sub,
	0.01);

    generator.color = v4_add(
	generator.color,
	mul);

    generator.color.w = 1.0;

    /* excavate line */

    char linechars[COLCOUNT] = {0};

    excavator_update(
	generator.excavator_a,
	linechars);

    excavator_update(
	generator.excavator_b,
	linechars);

    excavator_update(
	generator.excavator_c,
	linechars);

    excavator_update(
	generator.excavator_d,
	linechars);

    /* build up voxels */

    voxel_t linecubes[COLCOUNT] = {0};

    float leftedge  = 0.0;
    float rightedge = 0.0;

    /* from left to right */

    for (int col = 0;
	 col < COLCOUNT;
	 col++)
    {

	/* edge detection */

	char edge = 0;

	if (linechars[col] == 0)
	{

	    if (col > 0 && linechars[col - 1] == 1) edge = 1;
	    if (col < COLCOUNT - 1 && linechars[col + 1] == 1) edge = 1;
	    if (row > 0 && generator.prevlinechars[col] == 1) edge = 1;
	}

	if (linechars[col] == 1 && row > 0)
	{

	    if (generator.prevlinechars[col] == 0) edge = 1;
	    if (col > 0 && generator.prevlinechars[col - 1] == 1) edge = 0;
	    if (col < COLCOUNT - 1 && generator.prevlinechars[col + 1] == 1) edge = 0;
	}

	// cube generation

	float x = -600 + col * BLOCKSIZE;
	float y = topposition + row * -BLOCKSIZE;
	float z = -3000.0;

	linecubes[col].model = (vox_t){

	    x,
	    y,
	    z,
	    BLOCKSIZE,
	    BLOCKSIZE,
	    1000.0

	};

	// needed for bubble generation

	if (edge == 1 && leftedge == 0.0) leftedge = x;
	if (edge == 1) rightedge = x;

	// depth generation

	voxel_t prevlinecube = generator.prevlinecubes[col];
	voxel_t prevcube     = {0};

	if (col > 0) prevcube = linecubes[col - 1];

	z = generator_generatez(
	    edge,
	    linechars[col],
	    linecubes[col].model.z,
	    prevlinecube.model.z,
	    prevcube.model.z);

	if (z > -1200.0)
	{

	    float delta = -z;
	    float ratio = 1.0 - (delta / 1200.0);

	    v4_t midcolor = v4_scale(
		generator.color,
		ratio);

	    v4_t topcolor = v4_scale(
		midcolor,
		1.5);

	    midcolor.w = 1.0;
	    topcolor.w = 1.0;

	    v4_t btmcolor = (v4_t){0.0, 0.0, 0.0, 1.0};

	    if (z == -600.0) topcolor = (v4_t){1.0, 1.0, 1.0, 1.0};

	    voxel_set_color(
		&linecubes[col],
		topcolor,
		midcolor,
		btmcolor);

	    linecubes[col].model.x = x;
	    linecubes[col].model.y = y;
	    linecubes[col].model.z = z;
	}
    }

    /* from right to left */

    for (int col = COLCOUNT - 1;
	 col > -1;
	 col--)
    {

	float x = -600 + col * BLOCKSIZE;
	float y = topposition + row * -BLOCKSIZE;
	float z = -3000.0;

	voxel_t prevlinecube = generator.prevlinecubes[col];
	voxel_t prevcube     = {0};
	if (col < COLCOUNT - 1) prevcube = linecubes[col + 1];

	z = generator_generatez(
	    0,
	    linechars[col],
	    linecubes[col].model.z,
	    prevlinecube.model.z,
	    prevcube.model.z);

	if (z > -1200.0)
	{

	    float delta = -z;
	    float ratio = 1.0 - (delta / 1200.0);

	    v4_t midcolor = v4_scale(
		generator.color,
		ratio);

	    v4_t topcolor = v4_scale(
		midcolor,
		1.5);

	    midcolor.w = 1.0;
	    topcolor.w = 1.0;

	    v4_t btmcolor = (v4_t){0.0, 0.0, 0.0, 1.0};

	    if (z == -600.0) topcolor = (v4_t){1.0, 1.0, 1.0, 1.0};

	    voxel_set_color(
		&linecubes[col],
		topcolor,
		midcolor,
		btmcolor);

	    linecubes[col].model.x = x;
	    linecubes[col].model.y = y;
	    linecubes[col].model.z = z;
	}
    }

    memcpy(
	&generator.prevlinecubes,
	&linecubes,
	sizeof(linecubes));

    memcpy(
	&generator.prevlinechars,
	&linechars,
	sizeof(linechars));

    /* random floating points around path */

    voxel_t cube;

    float x = -1400 + (float) (rand() % ((int) (leftedge + 1300)));
    float y = topposition + row * -BLOCKSIZE;
    float z = -700 - (float) (rand() % 800);

    cube.model = (vox_t){
	x,
	y,
	z,
	BLOCKSIZE,
	BLOCKSIZE,
	BLOCKSIZE};

    float delta = -cube.model.x;
    float ratio = 1.0 - (delta / 1500.0);

    v4_t midcolor = v4_scale(
	generator.color,
	ratio);

    v4_t topcolor = v4_scale(
	midcolor,
	1.2);

    midcolor.w = 1.0;
    topcolor.w = 1.0;

    v4_t btmcolor = (v4_t){0.0, 0.0, 0.0, 1.0};

    voxel_set_color(
	&cube,
	topcolor,
	midcolor,
	btmcolor);

    generator.bubbles[row][0] = cube;

    x = rightedge + 100 + (float) (rand() % ((int) (1500 - rightedge)));
    y = topposition + row * -BLOCKSIZE;
    z = -700 - (float) (rand() % 800);

    cube.model = (vox_t){
	x,
	y,
	z,
	BLOCKSIZE,
	BLOCKSIZE,
	BLOCKSIZE};

    delta = -cube.model.z;
    ratio = 1.0 - (delta / 1500);

    // TODO move to function this twp

    midcolor = v4_scale(
	generator.color,
	ratio);

    topcolor = v4_scale(
	midcolor,
	1.2);

    midcolor.w = 1.0;
    topcolor.w = 1.0;

    btmcolor = (v4_t){0.0, 0.0, 0.0, 1.0};

    voxel_set_color(
	&cube,
	topcolor,
	midcolor,
	btmcolor);

    generator.bubbles[row][1] = cube;
}

#endif
