# swimmers-brownian-simulation

## How to make it works

### Install libraries
first step is to obtain the libraries
#### GNU Scientific Library
- on ubuntu you can maybe try: ```apt-get install libgsl0ldbl``` or ```apt-get install libgsl0-dev``` or ```apt-get install gsl-bin libgsl0-dev```
- you can also download the source code from ftp://ftp.gnu.org/gnu/gsl/ ("gsl-latest.tar.gz") and compile it following the instruction in "INSTALL"

#### SDL2
- obtain sdl2 libraries for the visualization:
. graphics: ```apt-get install libsdl2-dev``` (maybe only ```apt-get install libsdl2-2```)
. fonts: ```apt-get install libsdl2-image-dev``` and ```apt-get install libsdl-ttf2-dev``` (maybe only ```apt-get install libsdl2-image``` and ```apt-get install libsdl-ttf2```)

### Compile and run
-   ```./initializer.py```

### Profiling
- install Valgrind: ```apt-get install valgrind```
- memory leaks check: ```valgrind --leak-check=yes bin/simulation```
- profiling:
. ```valgrind --tool=callgrind bin/simulation```
. ```callgrind_annotate callgrind.out.<pid>```
