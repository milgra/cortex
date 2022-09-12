#include <stdio.h>
#include <string.h>
#include <time.h>

#include "SDL.h"
#include "SDL_mixer.h"
#include "bridge.h"
#include "cmd.c"
#include "defaults.c"
#include "linux/limits.h"
#include "menu.c"
#include "mtbus.c"
#include "renderer.c"
#include "scene.c"
#include "settings.c"
#include "zc_cstring.c"

float scale = 1.0;

int32_t width  = 1000;
int32_t height = 750;

uint32_t prevticks;

SDL_Window*   window;
SDL_GLContext context;

char quit = 0;

Mix_Music* gamemusic;
Mix_Music* outromusic;
Mix_Chunk* breaksound;
Mix_Music* actualmusic;

void main_onmessage(
    const char* name,
    void*       data)
{

    if (strcmp(name, "DONATE") == 0)
    {
	bridge_buy((char*) data);
    }
    else if (strcmp(name, "FEEDBACK") == 0)
    {

	bridge_open((char*) "http://www.milgra.com/cortex.html");
    }
    else if (strcmp(name, "HOMEPAGE") == 0)
    {

	bridge_open((char*) "http://www.milgra.com");
    }
    else if (strcmp(name, "FULLSCREEN") == 0)
    {

	int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;

	char fullscreen = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP;

	if (fullscreen == 1)
	{

	    SDL_SetWindowFullscreen(
		window,
		flags);
	}
	else
	{

	    SDL_SetWindowFullscreen(
		window,
		flags | SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
    }
    else if (strcmp(name, "RESET") == 0)
    {

	settings_reset();
    }
    else if (strcmp(name, "EXIT") == 0)
    {

	quit = 1;
    }
    else if (strcmp(name, "OPENMENU") == 0)
    {

	Mix_PauseMusic();
	renderer_reset_buffers();
	menu_redraw();
	defaults.state = kStateMenu;
    }
    else if (strcmp(name, "RESETGAME") == 0)
    {

	defaults_reset();
	menu_redraw();
    }
    else if (strcmp(name, "LEVELA") == 0)
    {

	renderer_reset_buffers();

	defaults.state        = kStateGame;
	defaults.currentlevel = "levelA";
	defaults.currentstage = defaults.stage_a;

	if (actualmusic != gamemusic)
	{
	    Mix_PauseMusic();
	    Mix_PlayMusic(gamemusic, -1);
	    actualmusic = gamemusic;
	    Mix_VolumeMusic(64);
	}
	else Mix_ResumeMusic();

	scene_loadmsg_t msg =
	    {
		15.0,
		defaults.stage_a};

	mtbus_notify(
	    "CTL",
	    "LOAD",
	    &msg);
    }
    else if (strcmp(name, "LEVELB") == 0)
    {

	renderer_reset_buffers();

	defaults.state        = kStateGame;
	defaults.currentlevel = "levelB";
	defaults.currentstage = defaults.stage_b;

	if (actualmusic != gamemusic)
	{
	    Mix_PauseMusic();
	    Mix_PlayMusic(gamemusic, -1);
	    actualmusic = gamemusic;
	    Mix_VolumeMusic(64);
	}
	else Mix_ResumeMusic();

	scene_loadmsg_t msg =
	    {
		23.0,
		defaults.stage_b};

	mtbus_notify(
	    "CTL",
	    "LOAD",
	    &msg);
    }
    else if (strcmp(name, "LEVELC") == 0)
    {

	renderer_reset_buffers();

	defaults.state        = kStateGame;
	defaults.currentlevel = "levelC";
	defaults.currentstage = defaults.stage_c;

	if (actualmusic != gamemusic)
	{
	    Mix_PauseMusic();
	    Mix_PlayMusic(gamemusic, -1);
	    actualmusic = gamemusic;
	    Mix_VolumeMusic(64);
	}
	else Mix_ResumeMusic();

	scene_loadmsg_t msg =
	    {
		32.0,
		defaults.stage_c};

	mtbus_notify(
	    "CTL",
	    "LOAD",
	    &msg);
    }
    else if (strcmp(name, "NEXTSCENE") == 0)
    {

	if (defaults.currentstage == 5)
	{

	    if (strcmp(defaults.currentlevel, "levelC") == 0)
	    {

		Mix_HaltMusic();
		Mix_PlayMusic(outromusic, -1);
		Mix_VolumeMusic(64);

		scene_loadmsg_t msg =
		    {
			scene.speed,
			7};

		mtbus_notify(
		    "CTL",
		    "LOAD",
		    &msg);
	    }
	    else
	    {
		scene_loadmsg_t msg =
		    {
			scene.speed,
			6};

		mtbus_notify(
		    "CTL",
		    "LOAD",
		    &msg);
	    }
	}
	else
	{

	    defaults.currentstage += 1;
	    defaults_save();

	    scene_loadmsg_t msg =
		{
		    scene.speed,
		    defaults.currentstage};

	    mtbus_notify(
		"CTL",
		"LOAD",
		&msg);

	    menu_set_color(
		generator.newcolor,
		defaults.currentlevel);
	}

	defaults.state = kStateGame;
    }
    else if (strcmp(name, "EXPLOSION") == 0)
    {

	Mix_PauseMusic();
	Mix_PlayChannel(-1, breaksound, 0);
    }
    else if (strcmp(name, "EFFECTS") == 0)
    {

	defaults.effects_level += 1;
	if (defaults.effects_level == 3) defaults.effects_level = 0;
	defaults_save();
    }
}

void main_init(
    void)
{

    srand((unsigned int) time(NULL));

    char* basepath = SDL_GetPrefPath(
	"milgra",
	"cortex");

    char* respath = SDL_GetBasePath();

    defaults.state = kStateMenu;

    settings_init(
	basepath,
	(char*) "cortex.state");

    defaults_init();

    mtbus_init();

    mtbus_subscribe(
	"MNU",
	main_onmessage);

    mtbus_subscribe(
	"SCN",
	main_onmessage);

    v2_t dimensions =
	{

	    .x = width * scale,
	    .y = height * scale

	};

    defaults.display_size = dimensions;

    bridge_init(); // request donation prices from app store, init text scaling, init glfw

    int framebuffer  = 0;
    int renderbuffer = 0;

    glGetIntegerv(
	GL_FRAMEBUFFER_BINDING,
	&framebuffer);

    glGetIntegerv(
	GL_RENDERBUFFER_BINDING,
	&renderbuffer);

    renderer_init(
	framebuffer,
	renderbuffer);

    menu_init(
	width * scale,
	height * scale);

    scene_init(
	width * scale,
	height * scale);

    char* gamesndpath = cstr_new_format(
	PATH_MAX,
	"%sgame.wav",
	respath,
	NULL);

    char* breaksndpath = cstr_new_format(
	PATH_MAX,
	"%sbreak.wav",
	respath,
	NULL);

    char* outrosndpath = cstr_new_format(
	PATH_MAX,
	"%soutro.wav",
	respath,
	NULL);

    gamemusic  = Mix_LoadMUS(gamesndpath);
    outromusic = Mix_LoadMUS(outrosndpath);
    breaksound = Mix_LoadWAV(breaksndpath);

    REL(gamesndpath);
    REL(outrosndpath);
    REL(breaksndpath);

    mtbus_notify(
	"CTL",
	"RESIZE",
	&dimensions);

    SDL_free(basepath);
#ifndef ANDROID
    SDL_free(respath);
#endif
}

void main_free(
    void)
{

    Mix_FreeMusic(gamemusic);
    Mix_FreeMusic(outromusic);
    Mix_FreeChunk(breaksound);

    scene_free();
    menu_free();
    renderer_free();

    bridge_free();

    mtbus_free();
    settings_free();
}

void main_loop(
    void)
{

    SDL_Event event;

    while (!quit)
    {

	while (SDL_PollEvent(&event) != 0)
	{

	    if (event.type == SDL_MOUSEBUTTONDOWN ||
		event.type == SDL_MOUSEBUTTONUP ||
		event.type == SDL_MOUSEMOTION)
	    {

		int x = 0;
		int y = 0;

		SDL_GetMouseState(
		    &x,
		    &y);

		v2_t dimensions =
		    {

			.x = x * scale,
			.y = y * scale

		    };

		if (event.type == SDL_MOUSEBUTTONDOWN)
		{

		    mtbus_notify(
			"CTL",
			"TOUCHDOWN",
			&dimensions);
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{

		    mtbus_notify(
			"CTL",
			"TOUCHUP",
			&dimensions);
		}
	    }
	    else if (event.type == SDL_QUIT)
	    {

		quit = 1;
	    }
	    else if (event.type == SDL_WINDOWEVENT)
	    {

		if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
		    event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		{

		    width  = event.window.data1;
		    height = event.window.data2;

		    v2_t dimensions =
			{

			    .x = width * scale,
			    .y = height * scale

			};

		    printf("resized %f %f\n", dimensions.x, dimensions.y);

		    defaults.display_size = dimensions;

		    mtbus_notify(
			"CTL",
			"RESIZE",
			&dimensions);
		}
	    }
	    else if (event.type == SDL_KEYDOWN)
	    {

		mtbus_notify(
		    "CTL",
		    "KEYDOWN",
		    &event.key.keysym.sym);
	    }
	    else if (event.type == SDL_KEYUP)
	    {

		switch (event.key.keysym.sym)
		{

		    case SDLK_f:

			main_onmessage((char*) "FULLSCREEN", NULL);
			break;

		    case SDLK_ESCAPE:

			main_onmessage((char*) "FULLSCREEN", NULL);
			break;
		}

		mtbus_notify(
		    "CTL",
		    "KEYUP",
		    &event.key.keysym.sym);
	    }
	    else if (event.type == SDL_APP_WILLENTERFOREGROUND)
	    {

		mtbus_notify(
		    "CTL",
		    "RESETTIME",
		    NULL); // reset scene timer to avoid giant step
	    }
	}

	// update simulation

	uint32_t ticks = SDL_GetTicks();

	// avoid first iteration ( ticks == 0 ) or type overflow

	if (prevticks > 0 &&
	    prevticks < ticks)
	{

	    int32_t delta = ticks - prevticks;
	    float   ratio = ((float) delta / 1000.0);

	    mtbus_notify(
		"CTL",
		"UPDATE",
		&ratio);

	    mtbus_notify(
		"CTL",
		"RENDER",
		&ticks);
	}

	prevticks = ticks;

	SDL_GL_SwapWindow(window);
    }
}

int main(int argc, char* argv[])
{

    // enable high dpi

    SDL_SetHint(
	SDL_HINT_VIDEO_HIGHDPI_DISABLED,
	"0");

    // init audio

    if (SDL_Init(SDL_INIT_AUDIO) != 0) printf("SDL Audio init error %s\n", SDL_GetError());

    Uint16 audio_format   = AUDIO_S16SYS;
    int    audio_rate     = 44100;
    int    audio_channels = 1;
    int    audio_buffers  = 4096;

    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());

    // init sdl

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == 0)
    {

	// setup opengl version

	SDL_GL_SetAttribute(
	    SDL_GL_CONTEXT_MAJOR_VERSION,
	    2);

	SDL_GL_SetAttribute(
	    SDL_GL_CONTEXT_MINOR_VERSION,
	    1);

	SDL_GL_SetAttribute(
	    SDL_GL_DOUBLEBUFFER,
	    1);

	SDL_GL_SetAttribute(
	    SDL_GL_DEPTH_SIZE,
	    24);

	// window size should be full screen on phones, scaled down on desktops

	SDL_DisplayMode displaymode;
	SDL_GetCurrentDisplayMode(0, &displaymode);

	if (displaymode.w < 800 ||
	    displaymode.h < 400)
	{

	    width  = displaymode.w;
	    height = displaymode.h;
	}
	else
	{

	    width  = displaymode.w * 0.8;
	    height = displaymode.h * 0.8;
	}

	// create window

	window = SDL_CreateWindow(
	    "Cortex",
	    SDL_WINDOWPOS_UNDEFINED,
	    SDL_WINDOWPOS_UNDEFINED,
	    width,
	    height,
	    SDL_WINDOW_OPENGL |
		SDL_WINDOW_SHOWN |
		SDL_WINDOW_RESIZABLE |
		SDL_WINDOW_ALLOW_HIGHDPI |
		SDL_WINDOW_SHOWN);

	if (window != NULL)
	{

	    // create context

	    context = SDL_GL_CreateContext(window);

	    if (context != NULL)
	    {

		GLint GlewInitResult = glewInit();
		if (GLEW_OK != GlewInitResult)
		{
		    printf("ERROR: %s", glewGetErrorString(GlewInitResult));
		}
		else printf("GLEW OKAY\n");

		// calculate scaling

		int nw;
		int nh;

		SDL_GL_GetDrawableSize(
		    window,
		    &nw,
		    &nh);

		scale = nw / width;

		// try to set up vsync

		if (SDL_GL_SetSwapInterval(1) < 0) printf("SDL swap interval error %s\n", SDL_GetError());

		main_init();
		main_loop();
		main_free();

		// cleanup

		SDL_GL_DeleteContext(context);
	    }
	    else printf("SDL context creation error %s\n", SDL_GetError());

	    // cleanup

	    SDL_DestroyWindow(window);
	}
	else printf("SDL window creation error %s\n", SDL_GetError());

	// cleanup

	SDL_Quit();
    }
    else printf("SDL init error %s\n", SDL_GetError());

    Mix_CloseAudio();

    return 0;
}
