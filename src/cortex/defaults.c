#ifndef defaults_h
#define defaults_h

#include "math2.c"
#include "math3.c"
#include <stdlib.h>

#define kStateMenu 0
#define kStateGame 1

typedef struct _defaults_t
{

    char stage_a;
    char stage_b;
    char stage_c;

    uint32_t color_a;
    uint32_t color_b;
    uint32_t color_c;

    int effects_level;

    // runtime

    char  state;
    char  currentstage;
    char* currentlevel;

    v3_t backcolor;
    v2_t display_size;

    float text_scale;

    char donation_arrived; // purchased items arrived from the store
    char prices_arrived;   // prices arrived for store items
    char prices[3][100];

} defaults_t;

extern defaults_t defaults;

void defaults_init(
    void);

void defaults_save(
    void);

void defaults_reset(
    void);

#endif

#if __INCLUDE_LEVEL__ == 0

#include "settings.c"
#include "zc_string.c"

defaults_t defaults = {0};

void defaults_init(
    void)
{

    int inited = settings_getint("initialized");

    if (inited == 0)
    {
	settings_setint(
	    "initialized",
	    1);

	settings_setint(
	    "donation_arrived",
	    0);

	defaults.donation_arrived = 0;
	defaults.effects_level    = 2;

#ifdef RASPBERRY
	defaults.effects_level = 0;
#endif

	settings_setint(
	    "levelA",
	    0);

	settings_setint(
	    "levelB",
	    0);

	settings_setint(
	    "levelC",
	    0);

	settings_setunsigned(
	    "colorA",
	    0x444488FF);

	settings_setunsigned(
	    "colorB",
	    0x444488FF);

	settings_setunsigned(
	    "colorC",
	    0x444488FF);
    }
    else
    {

	defaults.effects_level    = settings_getint("effects_level");
	defaults.donation_arrived = settings_getint("donation_arrived");
    }

    defaults.currentlevel = "levelA";

    defaults.stage_a = settings_getint("levelA");
    defaults.stage_b = settings_getint("levelB");
    defaults.stage_c = settings_getint("levelC");

    defaults.color_a = settings_getunsigned("colorA");
    defaults.color_b = settings_getunsigned("colorB");
    defaults.color_c = settings_getunsigned("colorC");

    defaults.text_scale = 1.0;
}

void defaults_reset()
{

    defaults.stage_a = 0;
    defaults.stage_b = 0;
    defaults.stage_c = 0;

    defaults.currentlevel = "levelA";
    defaults.currentstage = 0;

    str_t* ns = str_new();
    str_add_bytearray(ns, "true");

    settings_setstring(
	"inited",
	str_frombytes("true"));

    settings_setint(
	"levelA",
	0);

    settings_setint(
	"levelB",
	0);

    settings_setint(
	"levelC",
	0);

    settings_setunsigned(
	"colorA",
	0x444488FF);

    settings_setunsigned(
	"colorB",
	0x444488FF);

    settings_setunsigned(
	"colorC",
	0x444488FF);
}

void defaults_save()
{

    if (strcmp(defaults.currentlevel, "levelA") == 0) defaults.stage_a = defaults.currentstage;
    if (strcmp(defaults.currentlevel, "levelB") == 0) defaults.stage_b = defaults.currentstage;
    if (strcmp(defaults.currentlevel, "levelC") == 0) defaults.stage_c = defaults.currentstage;

    settings_setint(
	"levelA",
	defaults.stage_a);

    settings_setint(
	"levelB",
	defaults.stage_b);

    settings_setint(
	"levelC",
	defaults.stage_c);

    settings_setunsigned(
	"colorA",
	defaults.color_a);

    settings_setunsigned(
	"colorB",
	defaults.color_b);

    settings_setunsigned(
	"colorC",
	defaults.color_c);

    settings_setint(
	"effects_level",
	defaults.effects_level);

    settings_setint(
	"donation_arrived",
	defaults.donation_arrived);
}

#endif
