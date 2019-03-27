# swimmers-brownian-simulation

![comparison between an actual mesurement of the cell motion and the simulation of the model](swimmers-brownian-simulation/comparison.gif)


## How to make it works
Tested on linux ubuntu

### Install libraries
The first step is to obtain the necessary libraries:

#### GNU Scientific Library
- obtain [GNU scientific library](www.gnu.org/software/gsl/doc/html/index.html)for generating random numbers:
  * on ubuntu you can maybe try: ```apt-get install libgsl0ldbl``` or ```apt-get install libgsl0-dev``` or ```apt-get install gsl-bin libgsl0-dev```
  * you can also download the source code from <ftp://ftp.gnu.org/gnu/gsl/> ("gsl-latest.tar.gz") and compile it following the instruction in "INSTALL"

#### SDL2
- obtain [SDL2](www.libsdl.org) libraries for the visualization:
  * graphics: ```apt-get install libsdl2-dev``` (maybe only ```apt-get install libsdl2-2```)
  * fonts: ```apt-get install libsdl2-image-dev``` and ```apt-get install libsdl-ttf2-dev``` (maybe only ```apt-get install libsdl2-image``` and ```apt-get install libsdl-ttf2```)

#### meson
- [Meson](http://mesonbuild.com) for building the project:
```pip3 install meson```

### Compile and run
Build the project:
- ```meson build```

and run with:
- ```./initializer.py```

### Profiling
- install [Valgrind](http://valgrind.org/): ```apt-get install valgrind```
- install [kcachegrind](http://kcachegrind.sourceforge.net): ```apt-get install kcachegrind```
- memory leaks check: ```valgrind --leak-check=yes build/swimmers-brownian-simulation <input file>```
- profiling:
⋅⋅* ```valgrind --tool=callgrind build/swimmers-brownian-simulation <input file>```
⋅⋅* ```callgrind_annotate callgrind.out.<pid>```
