#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "SDL.h"
#include "SDL_mixer.h"
#include "bus.c"
#include "defaults.c"

#ifdef EMSCRIPTEN
    #include <emscripten.h>
    #include <emscripten/html5.h>
#endif

#ifdef __linux__
    #include "linux/limits.h"
#endif
#include "menu.c"
#include "renderer.c"
#include "scene.c"
#include "settings.c"
#include "zc_cstring.c"
#include "zc_log.c"
#include "zc_path.c"
#include "zc_time.c"

char     quit   = 0;
float    scale  = 1.0;
int32_t  width  = 1000;
int32_t  height = 750;
uint32_t prevticks;

Mix_Music* gamemusic;
Mix_Music* outromusic;
Mix_Chunk* breaksound;
Mix_Music* actualmusic;

SDL_Window*   window;
SDL_GLContext context;

char* res_path;
char* base_path;

void main_open(char* url)
{
    char newurl[100];
    snprintf(newurl, 100, "xdg-open %s", url);
    int result = system(newurl);
    if (result < 0)
	zc_log_error("system call error %s", newurl);
}

void main_buy(char* item)
{
    main_open("https://paypal.me/milgra");
}

void main_onmessage(
    const char* name,
    void*       data)
{
    if (strcmp(name, "DONATE") == 0)
    {
	main_buy((char*) data);
    }
    else if (strcmp(name, "FEEDBACK") == 0)
    {
	main_open((char*) "http://www.milgra.com/cortex.html");
    }
    else if (strcmp(name, "HOMEPAGE") == 0)
    {
	main_open((char*) "http://www.milgra.com");
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

	scene_loadmsg_t msg = {
	    25.0,
	    defaults.stage_a};

	bus_notify(
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

	scene_loadmsg_t msg = {
	    30.0,
	    defaults.stage_b};

	bus_notify(
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

	scene_loadmsg_t msg = {
	    35.0,
	    defaults.stage_c};

	bus_notify(
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

		scene_loadmsg_t msg = {
		    scene.speed,
		    7};

		bus_notify(
		    "CTL",
		    "LOAD",
		    &msg);
	    }
	    else
	    {
		scene_loadmsg_t msg = {
		    scene.speed,
		    6};

		bus_notify(
		    "CTL",
		    "LOAD",
		    &msg);
	    }
	}
	else
	{
	    defaults.currentstage += 1;
	    defaults_save();

	    scene_loadmsg_t msg = {
		scene.speed,
		defaults.currentstage};

	    bus_notify(
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

    defaults.state = kStateMenu;

    settings_init(
	base_path,
	(char*) "cortex.state");

    defaults_init();

    bus_init(); // FREE

    bus_subscribe(
	"MNU",
	main_onmessage);

    bus_subscribe(
	"SCN",
	main_onmessage);

    v2_t dimensions = {
	.x = width * scale,
	.y = height * scale};

    defaults.display_size = dimensions;

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
	"%s/game.wav",
	res_path,
	NULL);

    char* breaksndpath = cstr_new_format(
	PATH_MAX,
	"%s/break.wav",
	res_path,
	NULL);

    char* outrosndpath = cstr_new_format(
	PATH_MAX,
	"%s/outro.wav",
	res_path,
	NULL);

    gamemusic  = Mix_LoadMUS(gamesndpath);
    outromusic = Mix_LoadMUS(outrosndpath);
    breaksound = Mix_LoadWAV(breaksndpath);

    if (!gamemusic) zc_log_error("Couldn't load gamemusic from %s\n", gamesndpath);
    if (!outromusic) zc_log_error("Couldn't load gamemusic from %s\n", outrosndpath);
    if (!breaksound) zc_log_error("Couldn't load gamemusic from %s\n", breaksndpath);

    REL(gamesndpath);
    REL(outrosndpath);
    REL(breaksndpath);

    bus_notify(
	"CTL",
	"RESIZE",
	&dimensions);

    SDL_free(base_path);
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

    bus_free();
    settings_free();
}

int main_loop(double time, void* userdata)
{
    SDL_Event event;

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

	    v2_t dimensions = {
		.x = x * scale,
		.y = y * scale};

	    if (event.type == SDL_MOUSEBUTTONDOWN)
	    {
		bus_notify(
		    "CTL",
		    "TOUCHDOWN",
		    &dimensions);
	    }
	    else if (event.type == SDL_MOUSEBUTTONUP)
	    {
		bus_notify(
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

		v2_t dimensions = {
		    .x = width * scale,
		    .y = height * scale};

		zc_log_debug("resized %f %f", dimensions.x, dimensions.y);

		defaults.display_size = dimensions;

		bus_notify(
		    "CTL",
		    "RESIZE",
		    &dimensions);
	    }
	}
	else if (event.type == SDL_KEYDOWN)
	{
	    bus_notify(
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

	    bus_notify(
		"CTL",
		"KEYUP",
		&event.key.keysym.sym);
	}
	else if (event.type == SDL_APP_WILLENTERFOREGROUND)
	{
	    bus_notify(
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

	bus_notify(
	    "CTL",
	    "UPDATE",
	    &ratio);

	bus_notify(
	    "CTL",
	    "RENDER",
	    &ticks);
    }

    prevticks = ticks;

    SDL_GL_SwapWindow(window);

    return 1;
}

int main(int argc, char* argv[])
{
    zc_log_use_colors(isatty(STDERR_FILENO));
    zc_log_level_info();
    zc_time(NULL);

    printf("Cortex v" CORTEX_VERSION " by Milan Toth ( www.milgra.com )\n");

    const char* usage =
	"Usage: cortex [options]\n"
	"\n"
	"  -h, --help                          Show help message and quit.\n"
	"  -v                                  Increase verbosity of messages, defaults to errors and warnings only.\n"
	"  -r --resources= [resources folder] \t use resources dir for session\n"
	"\n";

    const struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"verbose", no_argument, NULL, 'v'},
	{"resources", optional_argument, 0, 'r'}};

    char* res_par = NULL;

    int option       = 0;
    int option_index = 0;

    while ((option = getopt_long(argc, argv, "vhr:", long_options, &option_index)) != -1)
    {
	switch (option)
	{
	    case '?': printf("parsing option %c value: %s\n", option, optarg); break;
	    case 'r': res_par = cstr_new_cstring(optarg); break; // REL 1
	    case 'v': zc_log_inc_verbosity(); break;
	    default: fprintf(stderr, "%s", usage); return EXIT_FAILURE;
	}
    }

    srand((unsigned int) time(NULL));

    char  cwd[PATH_MAX] = {"~"};
    char* res           = getcwd(cwd, sizeof(cwd));
    if (res == NULL)
	zc_log_error("CWD error");

    char* sdl_base = SDL_GetBasePath();
    char* wrk_path = path_new_normalize(sdl_base, NULL); // REL 6
    SDL_free(sdl_base);

    res_path = res_par ? path_new_normalize(res_par, wrk_path) : cstr_new_cstring(PKG_DATADIR); // REL 7

    base_path = SDL_GetPrefPath(
	"milgra",
	"cortex");

    // print path info to console

    zc_log_debug("resource path : %s", res_path);
    zc_log_debug("base path : %s", base_path);

    // enable high dpi

    SDL_SetHint(
	SDL_HINT_VIDEO_HIGHDPI_DISABLED,
	"0");

    // init audio

    if (SDL_Init(SDL_INIT_AUDIO) != 0)
	zc_log_error("SDL Audio init error %s\n", SDL_GetError());

    Uint16 audio_format   = AUDIO_S16SYS;
    int    audio_rate     = 44100;
    int    audio_channels = 1;
    int    audio_buffers  = 4096;

    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
	zc_log_error("Unable to initialize audio: %s\n", Mix_GetError());

    // init sdl

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == 0)
    {
	// setup opengl version

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

	SDL_GL_SetAttribute(
	    SDL_GL_CONTEXT_MAJOR_VERSION,
	    2);

	SDL_GL_SetAttribute(
	    SDL_GL_CONTEXT_MINOR_VERSION,
	    0);

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
	    SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);

	if (window != NULL)
	{
	    // create context

	    context = SDL_GL_CreateContext(window);

	    if (context != NULL)
	    {
		GLint GlewInitResult = glewInit();
		if (GLEW_OK != GlewInitResult)
		{
		    zc_log_error("Glew init error %s", glewGetErrorString(GlewInitResult));
		}

		// calculate scaling

		int nw;
		int nh;

		SDL_GL_GetDrawableSize(
		    window,
		    &nw,
		    &nh);

		scale = nw / width;

		// try to set up vsync

		if (SDL_GL_SetSwapInterval(1) < 0)
		    zc_log_error("SDL swap interval error %s", SDL_GetError());

		main_init();

#ifdef EMSCRIPTEN
		// setup the main thread for the browser and release thread with return
		emscripten_request_animation_frame_loop(main_loop, 0);
		return 0;
#else
		// infinite loop til quit
		while (!quit)
		{
		    main_loop(0, NULL);
		}
#endif

		main_free();

		// cleanup

		SDL_GL_DeleteContext(context);
	    }
	    else
		zc_log_error("SDL context creation error %s", SDL_GetError());

	    // cleanup

	    SDL_DestroyWindow(window);
	}
	else
	    zc_log_error("SDL window creation error %s", SDL_GetError());

	// cleanup

	SDL_Quit();
    }
    else
	zc_log_error("SDL init error %s", SDL_GetError());

    Mix_CloseAudio();

    if (res_par)
	REL(res_par);
    REL(wrk_path);

#ifdef DEBUG
    //    mem_stats();
#endif

    return 0;
}
