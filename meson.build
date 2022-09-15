project(
  'cortex',
  'c',
  version: '0.8',
  license: 'GPL',
  default_options: ['c_std=gnu99']
)

cc = meson.get_compiler('c')

math = cc.find_library('m',static:false)
gl = cc.find_library('GL',static:false)
glew = cc.find_library('GLEW',static:false)
glu = cc.find_library('GLU',static:false)
sdl = cc.find_library('SDL2',static:false)
sdlmixer = cc.find_library('SDL2_mixer',static:false)
pthread = cc.find_library('pthread',static:false)

cortex_dependencies = [math,gl,glew,sdl,sdlmixer,pthread]

cortex_version = '"@0@"'.format(meson.project_version())
pkg_datadir = join_paths(get_option('prefix'), get_option('datadir')) / 'cortex'
add_project_arguments('-DPKG_DATADIR="' + pkg_datadir + '"',
		      '-DCORTEX_VERSION=@0@'.format(cortex_version),
		      language: 'c')
add_project_arguments('-Wno-unused-but-set-variable', language: 'c')

if get_option('buildtype') == 'debug'
#    add_project_arguments('-DDEBUG',language: 'c')
endif

cortex_inc = include_directories(
	'/usr/include',
	'/usr/include/GL',
	'/usr/include/SDL2',
	'src/cortex',
	'src/cortex/gl',
	'src/zc_core',
	'src/zc_math')


com_sources = ['src/cortex/buffers.c',
	       'src/cortex/defaults.c',
	       'src/cortex/excavator.c',
	       'src/cortex/generator.c',
	       'src/cortex/levels.c',
	       'src/cortex/main.c',
	       'src/cortex/maincube.c',
	       'src/cortex/menu.c',
	       'src/cortex/particle.c',
	       'src/cortex/scene.c',
	       'src/cortex/script.c',
	       'src/cortex/file.c',
	       'src/cortex/settings.c',
	       'src/cortex/voxel.c',
	       'src/cortex/bus.c',
	       'src/cortex/pixeltext.c',
	       'src/cortex/str_util.c',

	       'src/cortex/gl/floatbuffer.c',	
	       'src/cortex/gl/ogl.c',
	       'src/cortex/gl/renderer.c',

	       'src/zc_core/zc_cstring.c',
	       'src/zc_core/zc_map.c',
	       'src/zc_core/zc_memory.c',
	       'src/zc_core/zc_string.c',
	       'src/zc_core/zc_vector.c',
	       'src/zc_core/zc_log.c',
	       'src/zc_core/zc_time.c',
	       'src/zc_core/zc_path.c',

	       'src/zc_math/zc_vec2.c',
	       'src/zc_math/zc_util2.c',
	       'src/zc_math/zc_vec3.c',
	       'src/zc_math/zc_util3.c',
	       'src/zc_math/zc_vec4.c',
	       'src/zc_math/zc_mat4.c']

install_data('res/break.wav', install_dir : pkg_datadir )
install_data('res/game.wav', install_dir : pkg_datadir )
install_data('res/outro.wav', install_dir : pkg_datadir )

executable(
  'cortex',
  com_sources,
  include_directories: [cortex_inc],
  dependencies: cortex_dependencies,
  install: true,
)