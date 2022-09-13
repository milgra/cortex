#ifndef menu_h
#define menu_h

#include "floatbuffer.c"
#include "pixeltext.c"
#include "scene.c"
#include "settings.c"
#include "voxel.c"
#include "zc_memory.c"
#include "zc_vector.c"
#include <stdio.h>

#define MENU_STATE_DEFAULT 0

typedef struct _menu_t menu_t;
struct _menu_t
{
    char state;

    float angle;
    float width;
    float height;
    float base_scale;

    m4_t proj_matrix;
    m4_t final_matrix;

    voxel_t        buttons[12];
    floatbuffer_t* buffer;
    floatbuffer_t* glowbuffer;
};

void menu_init(
    float width,
    float height);

void menu_free(
    void);

void menu_redraw(
    void);

void menu_set_color(
    v4_t  color,
    char* level);

extern menu_t menu;

#endif /* menu_h */

#if __INCLUDE_LEVEL__ == 0

#include "defaults.c"
#include "renderer.c"
#include <float.h>
#include <string.h>

#include "bus.c"

menu_t menu;

void menu_onmessage(
    const char* name,
    void*       data);

void menu_resize(
    float x,
    float y);

/* alloc */

void menu_init(
    float width,
    float height)
{
    bus_subscribe(
	"CTL",
	menu_onmessage);

    menu.buffer     = floatbuffer_alloc();
    menu.glowbuffer = floatbuffer_alloc();
    menu.state      = MENU_STATE_DEFAULT;
    menu.angle      = 0.0;
    menu.width      = width;
    menu.height     = height;
}

/* dealloc */

void menu_free(
    void)
{
    REL(menu.buffer);
}

/* get float color from uint */

void menu_float_from_uint(
    float*   r,
    float*   g,
    float*   b,
    float*   a,
    uint32_t color)
{
    *r = (float) ((color >> 24) & 0xFF) / 255.0;
    *g = (float) ((color >> 16) & 0xFF) / 255.0;
    *b = (float) ((color >> 8) & 0xFF) / 255.0;
    *a = (float) ((color) &0xFF) / 255.0;
}

uint32_t menu_color_from_floats(
    float r,
    float g,
    float b,
    float a)
{
    return (uint32_t) (r * 255.0) << 24 |
	   (uint8_t) (g * 255.0) << 16 |
	   (uint8_t) (b * 255.0) << 8 |
	   (uint8_t) (a * 255.0);
}

void menu_label(
    char*          label,
    uint32_t       color,
    float          size,
    float          x,
    float          y,
    float          z,
    floatbuffer_t* buffer)
{
    v2_t textsize = pixeltext_calcsize(
	label,
	size);

    x -= textsize.x / 2.0;

    vec_t* points = pixeltext_generate(
	label,
	size);

    for (int index = 0;
	 index < points->length;
	 index++)
    {

	v2_t* point = points->data[index];

	voxel_t voxel =
	    {
		.model =
		    {

			x + point->x,
			y + point->y,
			z,
			size,
			size,
			5.0

		    },

		.colors = {[0 ... 2] = color}

	    };

	voxel_collect_points(
	    &voxel,
	    buffer);
    }

    REL(points);
}

/* redraw menu */

void menu_redraw(
    void)
{
    floatbuffer_reset(menu.buffer);
    floatbuffer_reset(menu.glowbuffer);

    uint32_t ucolorA = settings_getunsigned("colorA");
    uint32_t ucolorB = settings_getunsigned("colorB");
    uint32_t ucolorC = settings_getunsigned("colorC");

    v4_t colorA;
    v4_t colorB;
    v4_t colorC;
    v4_t color = (v4_t){0.17, 0.17, 0.34, 0.9};

    menu_float_from_uint(
	&colorA.x,
	&colorA.y,
	&colorA.z,
	&colorA.w,
	ucolorA);

    menu_float_from_uint(
	&colorB.x,
	&colorB.y,
	&colorB.z,
	&colorB.w,
	ucolorB);

    menu_float_from_uint(
	&colorC.x,
	&colorC.y,
	&colorC.z,
	&colorC.w,
	ucolorC);

    colorA.w = 0.7;
    colorB.w = 0.7;
    colorC.w = 0.7;

    uint32_t col = menu_color_from_floats(
	color.x,
	color.y,
	color.z,
	color.w);

    uint32_t colA = menu_color_from_floats(
	colorA.x,
	colorA.y,
	colorA.z,
	colorA.w);

    uint32_t colB = menu_color_from_floats(
	colorB.x,
	colorB.y,
	colorB.z,
	colorB.w);

    uint32_t colC = menu_color_from_floats(
	colorC.x,
	colorC.y,
	colorC.z,
	colorC.w);

    voxel_t cube;

    /* background voxels */

    for (int index = 0;
	 index < 200;
	 index++)
    {

	int x = -1200 + (float) (rand() % 3000);
	int y = -1400 + (float) (rand() % 2400);
	int z = -800 - (float) (rand() % 1400);

	v4_t midcolor = (v4_t){0.1, 0.1, (2200 + z) / 6000.0, 1.0};
	v4_t topcolor = (v4_t){0.1, 0.1, 0.1 + (2200 + z) / 6000.0, 1.0};

	midcolor.w = 1.0;
	topcolor.w = 1.0;

	v4_t btmcolor = (v4_t){0.0, 0.0, 0.0, 0.0};

	cube = (voxel_t){
	    .model =
		{
		    x,
		    y,
		    z,
		    150.0,
		    150.0,
		    150.0},

	    .colors = {0}};

	voxel_set_color(
	    &cube,
	    topcolor,
	    midcolor,
	    btmcolor);

	voxel_collect_points(
	    &cube,
	    menu.buffer);
    }

    // title

    cube = (voxel_t){
	.model =
	    {

		-1400.0,
		150.0,
		-610.0,
		3700.0,
		200.0,
		5.0

	    },
	.colors = {[0 ... 2] = col}};

    voxel_collect_points(
	&cube,
	menu.buffer);

    menu_label(
	"-CORTEX-",
	0xFFFFFF88,
	30.0,
	130.0,
	127.0,
	-600.0,
	menu.glowbuffer);

    // level buttons

    cube = (voxel_t){

	.model =
	    {

		-1400.0,
		-125.0,
		-610.0,
		1300.0,
		150.0,
		5.0

	    },

	.colors = {[0 ... 2] = colA}

    };

    voxel_collect_points(
	&cube,
	menu.buffer); // slow

    menu.buttons[0] = cube;

    menu_label(
	"SLOW",
	0xFFFFFF88,
	20.0,
	-300.0,
	-145.0,
	-600.0,
	menu.glowbuffer);

    cube = (voxel_t){
	.model  = {-50.0, -125.0, -610.0, 400.0, 150.0, 5.0},
	.colors = {[0 ... 2] = colB}};

    voxel_collect_points(&cube, menu.buffer); // fast
    menu.buttons[1] = cube;

    menu_label(
	"FAST",
	0xFFFFFF88,
	20.0,
	150.0,
	-145.0,
	-600.0,
	menu.glowbuffer);

    cube = (voxel_t){
	.model  = {400.0, -125.0, -610.0, 1300.0, 150.0, 5.0},
	.colors = {[0 ... 2] = colC}};

    voxel_collect_points(&cube, menu.buffer); // insane
    menu.buttons[2] = cube;

    menu_label(
	"INSANE",
	0xFFFFFF88,
	20.0,
	640.0,
	-145.0,
	-600.0,
	menu.glowbuffer);

    // donate

    cube = (voxel_t){
	.model  = {-1400.0, -360.0, -610.0, 3700.0, 85.0, 5.0},
	.colors = {[0 ... 2] = col}};

    voxel_collect_points(&cube, menu.buffer);
    menu.buttons[11] = cube;

    char* label = "BUY ME A COFFEE";

    menu_label(
	label,
	0xFFFFFF88,
	10.0,
	160.0,
	-375.0,
	-600.0,
	menu.glowbuffer);

    // reset

    cube = (voxel_t){
	.model  = {450.0, 270.0, -610.0, 1200.0, 80.0, 5.0},
	.colors = {[0 ... 2] = col}};

    voxel_collect_points(&cube, menu.buffer); // reset state
    menu.buttons[3] = cube;

    menu_label("RESET", 0xFFFFFF88, 8.0, 640.0, 250.0, -600.0, menu.glowbuffer);

    // effects

    cube = (voxel_t){
	.model  = {-1400.0, 270.0, -610.0, 1200.0, 80.0, 5.0},
	.colors = {[0 ... 2] = col}};

    voxel_collect_points(&cube, menu.buffer); // effects
    menu.buttons[4] = cube;

    menu_label("EFFECTS", 0xFFFFFF88, 8.0, -410.0, 250.0, -600.0, menu.glowbuffer);

    // fullscreen button on desktop

    cube = (voxel_t){
	.model  = {-170.0, 270.0, -610.0, 590.0, 80.0, 5.0},
	.colors = {[0 ... 2] = col}};

    voxel_collect_points(&cube, menu.buffer); // fullscreen
    menu.buttons[5] = cube;

    menu_label("FULLSCREEN", 0xFFFFFF88, 8.0, 120.0, 250.0, -600.0, menu.glowbuffer);

    // impressum

    cube = (voxel_t){
	.model  = {-1400.0, -470.0, -610.0, 3700.0, 150.0, 5.0},
	.colors = {[0 ... 2] = col}};

    voxel_collect_points(&cube, menu.buffer);
    menu.buttons[6] = cube;

    menu_label("CODE AND DESIGN BY MILAN TOTH", 0xFFFFFF88, 8.0, 150.0, -480.0, -600.0, menu.glowbuffer);

    menu_label("MUSIC BY TAMAS KATAI", 0xFFFFFF88, 8.0, 180.0, -550.0, -600.0, menu.glowbuffer);

    // exit button on desktop

    cube = (voxel_t){
	.model  = {0.0, -700.0, -650.0, 350.0, 100.0, 5.0},
	.colors = {[0 ... 2] = col}};

    voxel_collect_points(&cube, menu.buffer);
    menu.buttons[7] = cube;

    menu_label("EXIT", 0xFFFFFF88, 15.0, 170.0, -680.0, -600.0, menu.glowbuffer);
}

/* touch happened, select button */

void menu_touch_down(
    float x,
    float y)
{
    if (defaults.state != kStateMenu) return;

    /* create screen-plane-normal vectors */

    v4_t front = v4_init(x, menu.height - y, -1.0, 0.0);
    v4_t back  = v4_init(x, menu.height - y, 0.0, 0.0);

    /* unproject vectors to 3d world */

    v3_t frontprj = m4_screen_to_world_coords(menu.final_matrix, front, menu.width, menu.height);
    v3_t backprj  = m4_screen_to_world_coords(menu.final_matrix, back, menu.width, menu.height);

    for (int index = 0;
	 index < 12;
	 index++)
    {
	voxel_t* cube = &menu.buttons[index];
	v3_t     is   = voxel_intersect(cube, frontprj, backprj, "03");

	if (is.x != FLT_MAX)
	{
	    if (index == 0) bus_notify("MNU", "LEVELA", NULL);
	    else if (index == 1) bus_notify("MNU", "LEVELB", NULL);
	    else if (index == 2) bus_notify("MNU", "LEVELC", NULL);
	    else if (index == 3)
	    {
		menu.state = MENU_STATE_DEFAULT;
		bus_notify("MNU", "RESETGAME", NULL);
	    }
	    else if (index == 4) bus_notify("MNU", "EFFECTS", NULL);
	    else if (index == 5) bus_notify("MNU", "FULLSCREEN", NULL);
	    else if (index == 6) bus_notify("MNU", "HOMEPAGE", NULL);
	    else if (index == 7) bus_notify("MNU", "EXIT", NULL);
	    else if (index == 11) bus_notify("MNU", "DONATE", NULL);
	}
    }
}

#define MENUWIDTH 1000.0
#define MENUHEIGHT 600

void menu_updatescale(
    void)
{
    float scale = 1.0;

    float wthratio =
	defaults.display_size.x / MENUWIDTH;

    float testheight =
	MENUHEIGHT * wthratio;

    float hthratio =
	defaults.display_size.y / MENUHEIGHT;

    float testwidth =
	MENUWIDTH * hthratio;

    if (testwidth <= defaults.display_size.x)
    {
	scale = hthratio;
    }
    else if (testheight <= defaults.display_size.y)
    {
	scale = wthratio;
    }

    menu.base_scale = scale * defaults.text_scale;
}

void menu_updateperspective(
    void)
{
    // calculate plane distance from focus point with simple trigonometry

    float camera_fov_y = M_PI / 4.0;

    float camera_eye_z =
	(defaults.display_size.y / 2.0) /
	(tanf(camera_fov_y / 2.0));

    float maxlength = MENUWIDTH > MENUHEIGHT ? MENUWIDTH : MENUHEIGHT;

    float min = camera_eye_z - maxlength * menu.base_scale;
    float max = camera_eye_z + maxlength * menu.base_scale + 3000.0;

    if (min < 10.0) min = 10.0;

    m4_t pers = m4_defaultperspective(
	camera_fov_y,
	defaults.display_size.x / defaults.display_size.y,
	min,
	max);

    m4_t trans = m4_defaulttranslation(0, 0, -camera_eye_z);
    m4_t scale = m4_defaultscale(menu.base_scale, menu.base_scale, menu.base_scale);

    menu.proj_matrix = m4_multiply(pers, trans);
    menu.proj_matrix = m4_multiply(menu.proj_matrix, scale);
}

void menu_resize(
    float width,
    float height)
{
    menu_updatescale();
    menu_updateperspective();

    menu.width  = width;
    menu.height = height;

    floatbuffer_setprojection(
	menu.buffer,
	menu.proj_matrix);

    floatbuffer_setprojection(
	menu.glowbuffer,
	menu.proj_matrix);
}

/* update iteration */

void menu_update(
    float ratio)
{
    if (defaults.state != kStateMenu) return;

    menu.angle += ratio * 0.5;
    if (menu.angle > M_PI * 2) menu.angle -= M_PI * 2;

    m4_t angle_matrix = m4_defaultrotation(M_PI / 10, M_PI / 15.0, 0.0);
    m4_t trans_matrix = m4_defaulttranslation(sinf(menu.angle) * 40.0, cosf(menu.angle) * 25.0, 0.0);

    angle_matrix = m4_multiply(trans_matrix, angle_matrix);
    angle_matrix = m4_multiply(menu.proj_matrix, angle_matrix);

    floatbuffer_setprojection(menu.buffer, angle_matrix);
    floatbuffer_setprojection(menu.glowbuffer, angle_matrix);

    menu.final_matrix = angle_matrix;

    if (menu.buffer->changed == 1)
    {

	renderdata_t data = {0, menu.buffer};
	bus_notify("SCN", "UPDBUFF", &data);
	menu.buffer->changed = 0;
    }

    if (menu.glowbuffer->changed == 1)
    {

	renderdata_t data = {3, menu.glowbuffer};
	bus_notify("SCN", "UPDBUFF", &data);
	menu.glowbuffer->changed = 0;
    }

    renderdata_t d1 = {0, menu.buffer};
    bus_notify("SCN", "UPDPRJ", &d1);

    renderdata_t d2 = {3, menu.glowbuffer};
    bus_notify("SCN", "UPDPRJ", &d2);
}

/* update color for specific level */

void menu_set_color(
    v4_t  color,
    char* level)
{
    if (strcmp(level, "levelA") == 0)
    {
	defaults.color_a = menu_color_from_floats(color.x, color.y, color.z, color.w);
    }
    if (strcmp(level, "levelB") == 0)
    {
	defaults.color_b = menu_color_from_floats(color.x, color.y, color.z, color.w);
    }
    if (strcmp(level, "levelC") == 0)
    {
	defaults.color_c = menu_color_from_floats(color.x, color.y, color.z, color.w);
    }

    defaults_save();
}

// message arrived from main

void menu_onmessage(
    const char* name,
    void*       data)
{
    if (strcmp(name, "UPDATE") == 0)
    {

	float* ratio = (float*) data;
	menu_update(*ratio);
    }
    else if (strcmp(name, "RESIZE") == 0)
    {

	v2_t* dimensions = data;

	menu_resize(dimensions->x, dimensions->y);
	menu_redraw();
    }
    else if (strcmp(name, "TOUCHDOWN") == 0)
    {

	v2_t* dimensions = data;

	menu_touch_down(dimensions->x, dimensions->y);
    }
}

#endif
