/* Scene generator */

#ifndef scene_h
#define scene_h

#include "buffers.c"
#include "floatbuffer.c"
#include "generator.c"
#include "voxel.c"
#include "zc_vector.c"
#include <stdio.h>

#define SCENE_WIDTH 960.0
#define SCENE_HEIGHT 640.0

#define STATE_DEAD 0
#define STATE_ALIVE 1
#define STATE_FINISHED 2

typedef struct _scene_loadmsg_t scene_loadmsg_t;
struct _scene_loadmsg_t
{
    float speed;
    int   stage;
};

typedef struct _scene_t scene_t;
struct _scene_t
{
    char* stages[8];

    /* states */

    char state;
    char leftdown;
    char rightdown;

    float speed;
    float frame;

    /* segment containers */

    voxel_t newsegment[ROWCOUNT][COLCOUNT]; // segment buffer for next segment

    voxel_t segments[ROWCOUNT * 2][COLCOUNT]; // actual segments
    voxel_t bubbles[ROWCOUNT * 2][2];         // bubble buffer

    int segmentstartindex;

    float headposition;
    float segmentbottompos;

    /* generator/iteration related */

    int generated_row; // actual needed row from generator
    int generator_run; // iterated generation is currently needed

    /* buffers and containers */

    vec_t* sparks;
    vec_t* shards;

    vec_t* glowshards;
    vec_t* mainshards;

    /* label */

    int  timer;
    char needsfillup;

    /* projection related */

    float scale;
    m4_t  proj_matrix;
};

void scene_init(
    float width,
    float height);

void scene_free(
    void);

extern scene_t scene;

#endif /* scene_h */

#if __INCLUDE_LEVEL__ == 0

#include <string.h>

#include "bus.c"
#include "pixeltext.c"
#include "zc_cstring.c"
#include "zc_memory.c"

#include "SDL.h"
#include "defaults.c"
#include "excavator.c"
#include "levels.c"
#include "maincube.c"
#include "particle.c"
#include "renderer.c"
#include "script.c"

scene_t scene = {0};

void scene_generatesegment(
    scene_t* controller);

void scene_onmessage(
    const char* name,
    void*       data);

void scene_generate_labels(
    void);

/* alloc */

void scene_init(
    float width,
    float height)
{
    bus_subscribe(
	"CTL",
	scene_onmessage);

    scene.stages[0] = intro;
    scene.stages[1] = stage1;
    scene.stages[2] = stage2;
    scene.stages[3] = stage3;
    scene.stages[4] = stage4;
    scene.stages[5] = stage5;
    scene.stages[6] = grats;
    scene.stages[7] = outro;

    scene.state     = STATE_ALIVE;
    scene.leftdown  = 0;
    scene.rightdown = 0;

    scene.speed = 20.0;
    scene.frame = 0.0;

    scene.generated_row = 0;
    scene.generator_run = 0;

    scene.shards = VNEW();
    scene.sparks = VNEW();

    scene.glowshards = VNEW();
    scene.mainshards = VNEW();

    script_init();
    buffers_init();
    generator_init();
    maincube_reset(
	PLAYCUBEY,
	BLOCKSIZE);

    scene.segmentstartindex = 0;
    scene.segmentbottompos  = 0.0;
    scene.headposition      = 0.0;

    scene.timer       = 0;
    scene.needsfillup = 0;

    scene.scale       = 1.0;
    scene.proj_matrix = m4_defaultidentity();
}

/* destruct */

void scene_free(
    void)
{
    REL(scene.shards);
    REL(scene.sparks);

    REL(scene.glowshards);
    REL(scene.mainshards);

    script_free();
    buffers_free();
    generator_free();
}

/* resets scene state */

void scene_reset(
    void)
{
    vec_reset(scene.sparks);
    vec_reset(scene.shards);

    vec_reset(scene.glowshards);
    vec_reset(scene.mainshards);

    script_reset();
    buffers_reset();
    generator_reset();
    maincube_reset(
	PLAYCUBEY,
	BLOCKSIZE);

    memset(&scene.segments, 0, sizeof(scene.segments));

    memset(&scene.bubbles, 0, sizeof(scene.bubbles));

    scene.state     = STATE_ALIVE;
    scene.leftdown  = 0;
    scene.rightdown = 0;

    scene.speed = 20.0;
    scene.frame = 0.0;

    scene.segmentstartindex = 0;
    scene.segmentbottompos  = 0;
    scene.headposition      = (BLOCKSIZE * ROWCOUNT) / 2;

    scene.generated_row = 0;
    scene.generator_run = 0;

    scene.needsfillup = 1;
}

/* collects voxels for given row in newsegment */

void scene_collectvoxels(
    int row)
{
    for (int col = 0;
	 col < COLCOUNT;
	 col++)
    {
	voxel_t cube = scene.newsegment[row][col];

	if (cube.model.z > -601.0)
	{
	    voxel_t white   = cube;
	    white.model.d   = 20.0;
	    white.model.z   = -580.0;
	    white.colors[0] = 0xFFFFFFFF;
	    white.colors[1] = 0xFFFFFFFF;
	    white.colors[2] = 0xFFFFFFFF;

	    voxel_collect_points(
		&white,
		buffers.buffertoptemp);
	}

	if (cube.model.z > -1200.0)
	{
	    voxel_collect_points(
		&cube,
		buffers.buffertemp);
	}
    }

    // copy background bubbles

    voxel_t bubble_a = generator.bubbles[row][0];
    voxel_t bubble_b = generator.bubbles[row][1];

    scene.bubbles[scene.segmentstartindex + row][0] = bubble_a;
    scene.bubbles[scene.segmentstartindex + row][1] = bubble_b;

    voxel_collect_points(
	&bubble_a,
	buffers.buffertemp);

    voxel_collect_points(
	&bubble_b,
	buffers.buffertemp);
}

/* fills up buffer line by line to reduce single-thread work */

void scene_fillbuffer_iteration(
    void)
{
    if (scene.generator_run == 1)
    {
	generator_generate_row(
	    scene.generated_row,
	    scene.segmentbottompos);

	void* target = &scene.newsegment[scene.generated_row];
	void* source = &generator.prevlinecubes;

	memcpy(target, source, sizeof(generator.prevlinecubes));

	scene_collectvoxels(scene.generated_row);

	scene.generated_row++;

	// finish generation

	if (scene.generated_row == ROWCOUNT)
	{
	    scene.generator_run = 0;
	    scene.generated_row = 0;
	}
    }
}

/* generate next segment */

void scene_swapsegments(
    void)
{
    /* switch segment and buffer */

    scene.segmentstartindex = ROWCOUNT - scene.segmentstartindex;

    if (scene.segmentstartindex == 0) buffers_switcha();
    else buffers_switchb();

    /* switch segment info */

    for (int row = 0;
	 row < ROWCOUNT;
	 row++)
    {
	void* target = &scene.segments[scene.segmentstartindex + row];
	void* source = &scene.newsegment[row];

	memcpy(target, source, sizeof(voxel_t) * COLCOUNT);
    }

    /* get blocks from generator */

    for (int row = 0;
	 row < ROWCOUNT;
	 row++)
    {
	scene_collectvoxels(scene.generated_row);
    }

    /* switch segments */

    scene.generator_run = 1;
    scene.segmentbottompos -= BLOCKSIZE * ROWCOUNT;
}

/* add new spark */

void scene_setup_sparks(
    char right)
{
    float x     = maincube.voxel.model.x + maincube.voxel.model.w * (float) right;
    float speed = 1.0 - 2.0 * (float) right;

    voxel_t cube =
	{

	    .model =
		{
		    x,
		    maincube.voxel.model.y,
		    maincube.voxel.model.z,
		    10.0,
		    10.0,
		    10.0},

	    .colors = {[0 ... 2] = 0xFFFFFFFF}

	};

    float dx = 5.0 * speed * (float) (rand() % 100) / 10.0;
    float dy = 5.0 - (float) (rand() % 200) / 10.0;
    float dz = (float) (rand() % 100) / 10.0;

    particle_t* particle = particle_create(
	cube,
	(v3_t){dx, dy, dz});

    VADD(scene.sparks, particle);

    REL(particle);
}

/* give new directions to all scene blocks */

void scene_setup_explosion(
    void)
{
    // generate encouraging text

    char* texts[] = {
	"NEVER GIVE UP!!!",
	"COME ON!!!",
	"CONCENTRATE!!!",
	"KEEP CALM!!!",
	"FEEL THE FLOW!!!"};

    if (script.label) REL(script.label);
    script.label = cstr_new_cstring(texts[rand() % 4]);

    scene_generate_labels();

    // play sound

    bus_notify("SCN", "EXPLOSION", NULL);

    // new shard particles from blocks

    for (int row = 0;
	 row < ROWCOUNT * 2;
	 row++)
    {
	for (int col = 0;
	     col < COLCOUNT;
	     col++)
	{
	    float dx = -0.5 + (float) (rand() % 10) / 10.0;
	    float dy = 0.5 + (float) (rand() % 10) / 10.0;
	    float dz = -0.5 + (float) (rand() % 10) / 10.0;

	    voxel_t voxel = scene.segments[row][col];

	    if (voxel.model.z > -601.0)
	    {
		// create white top

		voxel_t white   = voxel;
		white.model.d   = 20.0;
		white.model.z   = -580.0;
		white.colors[0] = 0xFFFFFFFF;
		white.colors[1] = 0xFFFFFFFF;
		white.colors[2] = 0xFFFFFFFF;

		particle_t* particle = particle_create(
		    white,
		    (v3_t){dx, dy, dz});

		VADD(scene.glowshards, particle);

		REL(particle);
	    }

	    particle_t* particle = particle_create(
		voxel,
		(v3_t){dx, dy, dz});

	    VADD(scene.shards, particle);

	    REL(particle);
	}
    }

    // new directions to background bubbles

    for (int row = 0;
	 row < ROWCOUNT * 2;
	 row++)
    {
	for (int col = 0;
	     col < 2;
	     col++)
	{
	    float dx = -0.5 + (float) (rand() % 10) / 10.0;
	    float dy = 0.5 + (float) (rand() % 10) / 10.0;
	    float dz = -0.5 + (float) (rand() % 10) / 10.0;

	    particle_t* particle = particle_create(
		scene.bubbles[row][col],
		(v3_t){dx, dy, dz});

	    VADD(scene.shards, particle);

	    REL(particle);
	}
    }

    // new direction to sparks

    for (int index = 0;
	 index < scene.sparks->length;
	 index++)
    {
	particle_t* particle = scene.sparks->data[index];

	particle->dir.x = -0.5 + (float) (rand() % 10) / 10.0;
	particle->dir.y = 0.5 + (float) (rand() % 10) / 10.0;
	particle->dir.z = -0.5 + (float) (rand() % 10) / 10.0;

	VADD(scene.mainshards, particle);
    }

    // new direction to maincube

    float dx = -0.5 + (float) (rand() % 10) / 10.0;
    float dy = 0.5 + (float) (rand() % 10) / 10.0;
    float dz = -0.5 + (float) (rand() % 10) / 10.0;

    particle_t* particle = particle_create(
	maincube.voxel,
	(v3_t){dx, dy, dz});

    VADD(scene.mainshards, particle);

    REL(particle);

    maincube.visible = 0;

    scene.state = STATE_DEAD;
}

/* create label and timer text if needed */

void scene_label(
    char*          label,
    uint32_t       color,
    float          size,
    float          x,
    float          y,
    float          z,
    floatbuffer_t* buffer)
{
    v2_t textsize = pixeltext_calcsize(label, size);

    x -= textsize.x / 2.0;

    vec_t* points = pixeltext_generate(label, size);

    for (int index = 0;
	 index < points->length;
	 index++)
    {

	v2_t* point = points->data[index];

	voxel_t voxel =
	    {

		.model = {x + point->x, y + point->y, z, size, size, 10.0},

		.colors = {0xFFFFFFFF, 0xAAAAAAFF, 0x222222FF}

	    };

	voxel_collect_points(&voxel, buffer);
    }

    REL(points);
}

/* generate HUD labels */

void scene_generate_labels(
    void)
{
    floatbuffer_reset(buffers.buffertext);

    if (script.label != NULL)
    {
	scene_label(
	    script.label,
	    0xFFFFFFFF,
	    15.0,
	    0.0,
	    350.0,
	    -400.0,
	    buffers.buffertext);
    }

    if (scene.timer >= 0)
    {
	char text[10];
	snprintf(text, 10, "%i", scene.timer);

	scene_label(
	    text,
	    0xFFFFFFFF,
	    15.0,
	    -750.0,
	    850.0,
	    -400.0,
	    buffers.buffertext);
    }
}

/* check collision of maincube and segments */

void scene_check_collision(
    void)
{
    int cursorrow = (int) ceilf(-(-scene.headposition + PLAYCUBEY) / BLOCKSIZE);
    int cursorcol = (int) roundf((maincube.voxel.model.x + HALFSCENE) / BLOCKSIZE);

    int upperrow = 0;
    int rowindex = 0;

    voxel_t cube  = {0};
    voxel_t lcube = {0};
    voxel_t rcube = {0};

    if (scene.segments[0][0].model.y >= -scene.headposition + PLAYCUBEY)
    {
	upperrow = (int) floorf(-scene.segments[0][0].model.y / BLOCKSIZE);
	rowindex = cursorrow - upperrow;
	cube     = scene.segments[rowindex][cursorcol];

	if (cursorcol > 0) lcube = scene.segments[rowindex][cursorcol - 1];
	if (cursorcol < 39) rcube = scene.segments[rowindex][cursorcol + 1];

	// printf( "\nrowindex 0 %i %f %f" , rowindex , segmenta[0][0].matrix.m31 , -scene.animator->trans_actual.y + 400.0 );
    }

    if (scene.segments[ROWCOUNT][0].model.y >= -scene.headposition + PLAYCUBEY)
    {
	upperrow = (int) floorf(-scene.segments[ROWCOUNT][0].model.y / BLOCKSIZE);
	rowindex = cursorrow - upperrow;

	if (rowindex > -1 && rowindex < ROWCOUNT)
	{
	    cube = scene.segments[ROWCOUNT + rowindex][cursorcol];
	    if (cursorcol > 0) lcube = scene.segments[ROWCOUNT + rowindex][cursorcol - 1];
	    if (cursorcol < 39) rcube = scene.segments[ROWCOUNT + rowindex][cursorcol + 1];
	}

	// printf( "\nrowindex 1 %i %f %f" , rowindex , segmentb[0][0].matrix.m31 , -scene.animator->trans_actual.y + 400.0 );
    }

    if (lcube.model.z == -600.0) scene_setup_sparks(0);
    if (rcube.model.z == -600.0) scene_setup_sparks(1);
    if (cube.model.z == -600.0) scene_setup_explosion();
}

/* load scene from descriptor */

void scene_load(
    char* data)
{
    scene_loadmsg_t* msg = (scene_loadmsg_t*) data;

    // reset if level changed

    if (scene.speed != msg->speed) scene_reset();

    script_reset();
    script_load(scene.stages[msg->stage]);

    scene.timer = 0;
    scene.speed = msg->speed;
    scene.state = STATE_ALIVE;
    scene.frame = 0.0;

    if (scene.needsfillup == 1)
    {
	scene.needsfillup = 0;
	scene_swapsegments();

	for (int index = 0;
	     index < ROWCOUNT;
	     index++)
	{
	    scene_fillbuffer_iteration();
	}
    }
}

/* update alive mode */

void scene_updatealive(
    float step)
{
    // update script

    if (scene.state == STATE_ALIVE) script_update(scene.frame);
    if (script.label != NULL) scene_generate_labels();
    if (script.frame_next == 0) scene.state = STATE_FINISHED;

    scene_fillbuffer_iteration();

    /* update main cube position */

    floatbuffer_reset(buffers.buffercube);

    if (maincube.visible == 1)
    {
	int timer = 60 - ((int) scene.frame - 120) / 60;

	if (timer >= 0 &&
	    timer != scene.timer)
	{

	    scene.timer = timer;
	    scene_generate_labels();
	}

	maincube_update(
	    step,
	    PLAYCUBEY);

	voxel_collect_points(
	    &maincube.voxel,
	    buffers.buffercube);
    }

    /* update sparks */

    if (scene.sparks->length > 0)
    {

	for (int index = 0;
	     index < scene.sparks->length;
	     index++)
	{

	    particle_t* particle = scene.sparks->data[index];

	    particle_update(
		particle,
		step);

	    voxel_collect_points(
		&particle->model,
		buffers.buffercube);

	    if (fabs(particle->model.model.x) > 3000.0 ||
		fabs(particle->model.model.y) > 3000.0 ||
		fabs(particle->model.model.z) > 3000.0)
	    {

		VREM(scene.sparks, particle);
	    }
	}
    }

    /* move scene if no collision */

    scene.headposition += scene.speed * step;

    if (scene.headposition > -scene.segmentbottompos - SWAPDISTANCE) scene_swapsegments();

    m4_t trans_matrix = m4_defaulttranslation(
	0.0,
	scene.headposition,
	0.0);

    m4_t angle_matrix = m4_defaultrotation(
	-M_PI / 6.0,
	0.0,
	0.0);

    trans_matrix = m4_multiply(
	angle_matrix,
	trans_matrix);

    trans_matrix = m4_multiply(
	scene.proj_matrix,
	trans_matrix);

    floatbuffer_setprojection(
	buffers.buffera,
	trans_matrix);

    floatbuffer_setprojection(
	buffers.bufferb,
	trans_matrix);

    floatbuffer_setprojection(
	buffers.buffertopa,
	trans_matrix);

    floatbuffer_setprojection(
	buffers.buffertopb,
	trans_matrix);

    scene_check_collision();
}

/* update dead state */

void scene_updatedead(
    float step)
{
    /* scatter blocks if collision */

    buffers_resetscene();

    for (int index = 0;
	 index < scene.shards->length;
	 index++)
    {

	particle_t* particle = scene.shards->data[index];

	particle_update(
	    particle,
	    step);

	voxel_collect_points(
	    &particle->model,
	    buffers.buffera);
    }

    for (int index = 0;
	 index < scene.glowshards->length;
	 index++)
    {

	particle_t* particle = scene.glowshards->data[index];

	particle_update(
	    particle,
	    step);

	voxel_collect_points(
	    &particle->model,
	    buffers.buffertopa);
    }

    for (int index = 0;
	 index < scene.mainshards->length;
	 index++)
    {

	particle_t* particle = scene.mainshards->data[index];

	particle_update(
	    particle,
	    step);

	voxel_collect_points(
	    &particle->model,
	    buffers.buffercube);
    }
}

/* update scene */

void scene_update(
    void* data)
{
    if (defaults.state != kStateGame) return;

    float ratio = *(float*) data;
    float step  = ratio * 50.0;

    // limit stepping
    if (step > 0.9) step = 0.9;

    if (scene.state != STATE_DEAD)
    {

	scene_updatealive(step);
    }
    else
    {

	scene_updatedead(step);
    }

    buffers_upload();

    scene.frame += step;
}

/* resize */

#define WIDTH 1000.0
#define HEIGHT 600

void scene_updatescale(
    void)
{
    float scale = 1.0;

    float wthratio   = defaults.display_size.x / WIDTH;
    float testheight = HEIGHT * wthratio;
    float hthratio   = defaults.display_size.y / HEIGHT;
    float testwidth  = WIDTH * hthratio;

    if (testwidth <= defaults.display_size.x) scale = hthratio;
    else if (testheight <= defaults.display_size.y) scale = wthratio;

    scene.scale = scale;
}

/* update perspective */

void scene_updateperspective(
    void)
{
    // calculate plane distance from focus point with simple trigonometry

    float camera_fov_y = M_PI / 4.0;
    float camera_eye_z =
	(defaults.display_size.y / 2.0) /
	(tanf(camera_fov_y / 2.0));

    float maxlength = WIDTH > HEIGHT ? WIDTH : HEIGHT;

    float min = camera_eye_z - maxlength * scene.scale;
    float max = camera_eye_z + maxlength * scene.scale + 2000.0;

    if (min < 10.0) min = 10.0;

    m4_t pers = m4_defaultperspective(
	camera_fov_y,
	defaults.display_size.x / defaults.display_size.y,
	min,
	max);

    m4_t trans = m4_defaulttranslation(
	0,
	0,
	-camera_eye_z);

    m4_t scale = m4_defaultscale(
	scene.scale,
	scene.scale,
	scene.scale);

    scene.proj_matrix = m4_multiply(
	pers,
	trans);

    scene.proj_matrix = m4_multiply(
	scene.proj_matrix,
	scale);
}

/* resize scene */

void scene_resize(
    void* data)
{
    scene_updatescale();
    scene_updateperspective();

    m4_t angle_matrix = m4_defaultrotation(
	-M_PI / 6,
	0.0,
	0.0);

    angle_matrix = m4_multiply(
	scene.proj_matrix,
	angle_matrix);

    floatbuffer_setprojection(
	buffers.buffercube,
	angle_matrix);

    floatbuffer_setprojection(
	buffers.buffertext,
	angle_matrix);
}

/* touch down */

void scene_touch_down(
    void* data)
{
    v2_t coord = *(v2_t*) data;

    if (defaults.state == kStateGame)
    {
	if (scene.state == STATE_DEAD ||
	    scene.state == STATE_FINISHED)
	{
	    scene.speed = 0.0;
	    bus_notify(
		"SCN",
		"OPENMENU",
		NULL);
	}

	if (coord.x < defaults.display_size.x / 2.0)
	{
	    scene.leftdown = 1;
	    maincube_set_direction(
		-1,
		scene.speed);
	}
	else
	{
	    scene.rightdown = 1;
	    maincube_set_direction(
		1,
		scene.speed);
	}
    }
}

/* touch up event */

void scene_touch_up(
    void* data)
{
    v2_t coord = *(v2_t*) data;

    if (coord.x < defaults.display_size.x / 2.0)
    {
	scene.leftdown = 0;
	maincube_set_direction(
	    scene.rightdown,
	    scene.speed);
    }
    else
    {
	scene.rightdown = 0;
	maincube_set_direction(
	    -scene.leftdown,
	    scene.speed);
    }
}

/* key down event */

void scene_key_down(
    void* data)
{
    SDL_Keycode code = *(SDL_Keycode*) data;

    if (code == SDLK_LEFT)
    {

	scene.leftdown = 1;
	maincube_set_direction(
	    -1,
	    scene.speed);
    }
    else if (code == SDLK_RIGHT)
    {

	scene.rightdown = 1;
	maincube_set_direction(
	    1,
	    scene.speed);
    }
}

/* key up event */

void scene_key_up(
    void* data)
{
    SDL_Keycode code = *(SDL_Keycode*) data;

    if (code == SDLK_LEFT)
    {
	scene.leftdown = 0;
	maincube_set_direction(
	    scene.rightdown,
	    scene.speed);
    }
    else if (code == SDLK_RIGHT)
    {

	scene.rightdown = 0;
	maincube_set_direction(
	    -scene.leftdown,
	    scene.speed);
    }
    else if (code == SDLK_RETURN)
    {
	scene.speed = 0.0;
	bus_notify(
	    "SCN",
	    "OPENMENU",
	    NULL);
    }
}

/* message arrived */

void scene_onmessage(
    const char* name,
    void*       data)
{
    if (strcmp(name, "UPDATE") == 0) scene_update(data);
    else if (strcmp(name, "RESIZE") == 0) scene_resize(data);
    else if (strcmp(name, "TOUCHDOWN") == 0) scene_touch_down(data);
    else if (strcmp(name, "TOUCHUP") == 0) scene_touch_up(data);
    else if (strcmp(name, "KEYDOWN") == 0) scene_key_down(data);
    else if (strcmp(name, "KEYUP") == 0) scene_key_up(data);
    else if (strcmp(name, "LOAD") == 0) scene_load(data);
}

#endif
