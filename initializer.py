#!/usr/bin/env python3

import os
from pathlib import Path
import subprocess
import json
from pprint import pprint

def createMake(gslCompileDir="/usr/local/include", gslLinkDir="/usr/local/lib", SDL2=True, debug=False, release=False):
    make = "COMPILER = g++"
    make += "\nCOMPILER_FLAGS = -std=c++0x -Wall -pedantic -I" + gslCompileDir
    if debug:
        make += " -ggdb"
    if release:
        make += " -O3"
    else:
        make += " -O0"
    if SDL2:
        make += " -DUSESDL=TRUE"
    make += "\nLINKER_FLAGS = -std=c++0x -lm -lgsl -lgslcblas -L" + gslLinkDir
    if SDL2:
        make += " -lSDL2 -lSDL2_ttf"
    if debug:
        make += " -ggdb"
    if release:
        make += " -O3"
    #-lSDL2_image #-pthread
    make += "\nSOURCES = src/*.cpp"
    make += "\nOBJECTS := ${subst src/,,$(SOURCES:.cpp=.o)}"
    make += """\n
all: bin/simulation

bin/simulation: $(OBJECTS)
\t$(COMPILER) $(OBJECTS) $(LINKER_FLAGS) -o bin/simulation

$(OBJECTS): $(SOURCES) $(SOURCES:.cpp=hpp)
\t$(COMPILER) -c $(SOURCES) $(COMPILER_FLAGS)
"""
    with open('Makefile', 'w') as makefile:
        makefile.write(make)

def createFolders():
    if not os.path.exists('./bin'):
        os.makedirs('./bin')
    if not os.path.exists('./input'):
        os.makedirs('./input')
    if not os.path.exists('./output'):
        os.makedirs('./output')

def createParameters():
    with open('./param/full_physics_parameters.json') as f:
        data = json.load(f)

print("--- Creating folders if needed:")
createFolders()

print("--- Creating makefile")
gslCompileDir = str(Path.joinpath(Path().absolute(), "gsl/include"))
gslLinkDir = str(Path.joinpath(Path().absolute(), "gsl/lib"))
createMake(SDL2=False, debug=True, gslCompileDir=gslCompileDir, gslLinkDir=gslLinkDir)

print("--- Compiling code:")
outcome = subprocess.run('make')
print("--- Outcome: " + str(outcome))

print("--- Create parameters files")
createParameters()

# pprint(data)

print("--- Running simulation program:")
myEnv = os.environ.copy()
myEnv['LD_LIBRARY_PATH'] = gslLinkDir
outcome = subprocess.run('bin/simulation', env=myEnv)
print("--- Outcome: " + str(outcome))

print("--- Running plotter script:")
outcome = subprocess.run('./plotter.py')
print("--- Outcome: " + str(outcome))
