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
	'src/modules/tools')


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

	       'src/modules/core/mtbmp.c',
	       'src/modules/core/mtbus.c',
	       'src/modules/core/mtcstr.c',
	       'src/modules/core/mtfile.c',
	       'src/modules/core/mtmap.c',
	       'src/modules/core/mtmem.c',
	       'src/modules/core/mtpipe.c',
	       'src/modules/core/mtstr.c',
	       'src/modules/core/mtvec.c',

	       'src/modules/tools/cmd.c',
	       'src/modules/tools/floatbuffer.c',
	       'src/modules/tools/math1.c',
	       'src/modules/tools/math2.c',
	       'src/modules/tools/math3.c',
	       'src/modules/tools/math4.c',
	       'src/modules/tools/ogl.c',
	       'src/modules/tools/physics2.c',
	       'src/modules/tools/pixeltext.c',
	       'src/modules/tools/settings.c',
	       'src/modules/tools/triangulate.c']

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