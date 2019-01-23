project('/home/parapappo/projects/swimmers-brownian-simulation/meson.build', 'cpp',
  version : '0.1',
  license : 'MIT')

add_global_arguments('-Dusesdl', language : 'cpp')

nlohmann_json_proj = subproject('nlohmann_json')

nlohmann_json_dep = nlohmann_json_proj.get_variable('nlohmann_json_dep')
depSdl2 = dependency('sdl2')
depSdl2_ttf = dependency('SDL2_ttf')
depGsl = dependency('gsl')
depThreads = dependency('threads')

sources = ['src/actor.cpp', 'src/analyzer.cpp', 'src/cell.cpp', 'src/diskWall.cpp', 'src/main.cpp', 'src/simulation.cpp', 'src/visualization.cpp', 'src/definition.hpp']

executable('swimmers-brownian-simulation', sources, dependencies : [depSdl2, depSdl2_ttf, depGsl, depThreads, nlohmann_json_dep])