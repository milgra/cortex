project(
  'cortex',
  'c',
  version: '0.8',
  license: 'MIT',
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
pkg_datadir = join_paths(get_option('prefix'), get_option('datadir')) / 'sov'
add_project_arguments('-DPKG_DATADIR="' + pkg_datadir + '"',
		      '-DCORTEX_VERSION=@0@'.format(cortex_version),
		      language: 'c')
add_project_arguments('-Wno-unused-but-set-variable', language: 'c')

cortex_inc = include_directories(
	'/usr/include',
	'/usr/include/GL',
	'/usr/include/SDL2',
	'src/cortex',
	'src/modules',
	'src/modules/core',
	'src/modules/math',
	'src/modules/utils')


com_sources = ['src/cortex/bridge.c',
	       'src/cortex/buffers.c',
	       'src/cortex/defaults.c',
	       'src/cortex/excavator.c',
	       'src/cortex/generator.c',
	       'src/cortex/levels.c',
	       'src/cortex/main.c',
	       'src/cortex/maincube.c',
	       'src/cortex/menu.c',
	       'src/cortex/particle.c',
	       'src/cortex/renderer.c',
	       'src/cortex/scene.c',
	       'src/cortex/script.c',
	       'src/cortex/voxel.c',
	       'src/cortex/cmd.c',
	       'src/cortex/floatbuffer.c',
	       'src/cortex/ogl.c',
	       'src/cortex/physics2.c',
	       'src/cortex/pixeltext.c',
	       'src/cortex/settings.c',
	       'src/cortex/triangulate.c',
	       'src/cortex/mtbus.c',
	       'src/cortex/mtfile.c',

	       'src/modules/core/zc_cstring.c',
	       'src/modules/core/zc_map.c',
	       'src/modules/core/zc_memory.c',
	       'src/modules/core/zc_string.c',
	       'src/modules/core/zc_vector.c',

	       'src/modules/math/zc_vec2.c',
	       'src/modules/math/zc_util2.c',
	       'src/modules/math/zc_vec3.c',
	       'src/modules/math/zc_util3.c',
	       'src/modules/math/zc_vec4.c',
	       'src/modules/math/zc_mat4.c',

	       'src/modules/utils/str_util.c']

pkg_datadir = join_paths(get_option('prefix'), get_option('datadir')) / 'sov'
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