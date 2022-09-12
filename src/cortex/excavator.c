#ifndef excavator_h
#define excavator_h

#include "mtstr.c"
#include "zc_memory.c"
#include <math.h>

#define kExcavatorModeStill 0
#define kExcavatorModeLinear 1
#define kExcavatorModeRandom 2
#define kExcavatorModeWave 3
#define kExcavatorModeZigZag 4
#define kExcavatorModeStrong 5

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

typedef struct _excavator_t excavator_t;
struct _excavator_t
{

    short mode; // extracting mode

    short line_width;     // width of line
    short new_line_width; // new width of line

    short ext_pos;   // position of extracotr
    short ext_width; // width of extractor

    float eff_dir;     // direction of zigzag effect
    float eff_diff;    // difficulty of effect
    float eff_angle;   // angle counter for effects
    short eff_rnd_cnt; // random counter for strong effect

    float float_wth; // floating point width
    float float_pos; // floating point position
};

excavator_t* excavator_alloc(
    int width);

void excavator_reset(
    excavator_t* excavator,
    int          width);

void excavator_setmode(
    excavator_t* excavator,
    mtstr_t*     string);

void excavator_update(
    excavator_t* data,
    char*        line);

#endif /* excavator_h */

#if __INCLUDE_LEVEL__ == 0

#include "excavator.c"
#include <string.h>

void excavator_destruct(
    void* pointer);

excavator_t* excavator_alloc(
    int width)
{

    excavator_t* result = CAL(
	sizeof(excavator_t),
	NULL,
	NULL);

    result->mode       = kExcavatorModeLinear;
    result->line_width = width;

    result->ext_width      = width;
    result->new_line_width = width;
    result->ext_pos        = 0;

    result->eff_angle   = 90;
    result->eff_dir     = 1;
    result->eff_diff    = 1;
    result->eff_rnd_cnt = 0;

    result->float_wth = (float) width;
    result->float_pos = 0;

    return result;
}

void excavator_reset(
    excavator_t* excavator,
    int          width)
{

    excavator->mode       = kExcavatorModeLinear;
    excavator->ext_width  = width;
    excavator->line_width = width;

    excavator->new_line_width = width;
    excavator->ext_pos        = 0;

    excavator->eff_angle   = 90;
    excavator->eff_dir     = 1;
    excavator->eff_diff    = 1;
    excavator->eff_rnd_cnt = 0;

    excavator->float_wth = (float) width;
    excavator->float_pos = 0;
}

void excavator_setmode(
    excavator_t* excavator,
    mtstr_t*     string)
{

    char* bytes = mtstr_bytes(string);

    if (strcmp(bytes, "still") == 0) excavator->mode = kExcavatorModeStill;
    if (strcmp(bytes, "linear") == 0) excavator->mode = kExcavatorModeLinear;
    if (strcmp(bytes, "random") == 0) excavator->mode = kExcavatorModeRandom;
    if (strcmp(bytes, "wave") == 0) excavator->mode = kExcavatorModeWave;
    if (strcmp(bytes, "zigzag") == 0) excavator->mode = kExcavatorModeZigZag;
    if (strcmp(bytes, "strong") == 0) excavator->mode = kExcavatorModeStrong;

    REL(bytes);
}

void excavator_update_size(
    excavator_t* data)
{

    float delta;

    if (data->ext_width < data->new_line_width)
    {

	delta = data->eff_diff;
	if (delta > data->new_line_width - data->ext_width)
	{

	    delta = (float) data->new_line_width - (float) data->ext_width;
	}
    }
    else
    {

	delta = -data->eff_diff;
	if (delta > data->ext_width - data->new_line_width)
	{

	    delta = (float) data->ext_width - (float) data->new_line_width;
	}
    }

    data->float_wth += delta;
    data->float_pos -= delta / 2;

    if (data->float_pos < 0) data->float_pos = 0;

    data->ext_width = (short) data->float_wth;
    data->ext_pos   = (short) data->float_pos;

    if (data->ext_pos + data->ext_width >= data->line_width)
    {

	data->ext_pos = data->line_width - data->ext_width;
	if (data->ext_pos < 0) data->ext_pos = 0;
    }
}

void excavator_update_random(
    excavator_t* data)
{

    float delta;

    data->eff_angle += (-5 + rand() % 10) * data->eff_diff;

    if (data->eff_angle > 360) data->eff_angle -= 360;
    if (data->eff_angle < 0) data->eff_angle += 360;

    delta = cos(data->eff_angle * M_PI / 180);

    data->float_pos += delta;

    if (data->float_pos < 0)
    {
	data->float_pos = 0;
	data->eff_angle += 90;
    }

    if (data->float_pos + data->float_wth > (float) data->line_width)
    {
	data->float_pos = (float) data->line_width - data->float_wth;
	data->eff_angle += 90;
    }

    data->ext_pos = (short) data->float_pos;
}

void excavator_update_wave(
    excavator_t* data)
{

    float delta;

    data->eff_angle += data->eff_diff * 2;

    if (data->eff_angle > 360) data->eff_angle -= 360;

    delta = cos(data->eff_angle * M_PI / 180);

    if (data->float_pos + delta / 2 < 0)
    {
	data->float_pos -= delta / 2;
    }
    else
    {
	data->float_pos += delta / 2;
    }

    if (data->float_pos + data->float_wth > (float) data->line_width)
    {
	data->float_pos = (float) data->line_width - data->float_wth;
    }

    data->ext_pos = (short) data->float_pos;
}

void excavator_update_zigzag(
    excavator_t* data)
{

    float newDiff;

    newDiff = (data->eff_diff / 2);
    if (newDiff > 1.2) newDiff = 1.2;
    data->float_pos += newDiff * data->eff_dir;

    if (data->float_pos < 0)
    {
	data->float_pos = 0;
	data->eff_dir *= -1;
    }
    else if (data->float_pos + data->float_wth > (float) (data->line_width))
    {
	data->float_pos = (float) data->line_width - data->float_wth;
	data->eff_dir *= -1;
    }

    if (rand() % 40 < 2) data->eff_dir *= -1;
    data->ext_pos = (short) data->float_pos;
}

void excavator_update_string(
    excavator_t* data)
{

    float pick;

    if (++data->eff_rnd_cnt == 10)
    {
	data->eff_rnd_cnt = 0;

	pick = (-data->float_wth + rand() % (int) (2 * data->float_wth)) * (data->eff_diff / 6);

	if (data->float_pos + pick < 0)
	{
	    data->float_pos -= pick;
	}
	else
	{
	    data->float_pos += pick;
	}

	if (data->float_pos + data->float_wth > (float) (data->line_width))
	{
	    data->float_pos = (float) data->line_width - data->float_wth;
	}

	data->ext_pos = (short) data->float_pos;
    }
}

void excavator_update(
    excavator_t* data,
    char*        line)
{

    int x, left, right;

    // update excavator based on mode

    if (data->mode == kExcavatorModeStill) return;
    else if (data->mode == kExcavatorModeRandom) excavator_update_random(data);
    else if (data->mode == kExcavatorModeWave) excavator_update_wave(data);
    else if (data->mode == kExcavatorModeZigZag) excavator_update_zigzag(data);
    else if (data->mode == kExcavatorModeStrong) excavator_update_string(data);

    // update size if needed

    if (data->ext_width != data->new_line_width) excavator_update_size(data);

    left  = data->ext_pos;
    right = data->ext_pos + data->ext_width;

    if (left < 1) left = 1;
    if (right >= data->line_width) right = data->line_width;

    for (x = left; x < right; x++) line[x] = 1;
    line[39] = 0;

    // printf( "\n" );
    // for ( int index = 0 ; index < data->line_width ; index++ ) printf( "%i" , line[index] );
}

#endif
