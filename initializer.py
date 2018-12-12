#!/usr/bin/env python3

import os
from pathlib import Path
import subprocess
import argparse
import json
from copy import deepcopy


def createMake(gslCompileDir='/usr/local/include', gslLinkDir='/usr/local/lib', SDL2=True, debug=False, release=False):
    make = 'COMPILER = g++'
    make += '\nCOMPILER_FLAGS = -std=c++0x -Wall -pedantic -I' + gslCompileDir
    if debug:
        make += ' -ggdb'
    if release:
        make += ' -O3'
    else:
        make += ' -O0'
    if SDL2:
        make += ' -Dusesdl'
    make += '\nLINKER_FLAGS = -std=c++0x -lm -lgsl -lgslcblas -L' + gslLinkDir
    if SDL2:
        make += ' -lSDL2 -lSDL2_ttf'
    if debug:
        make += ' -ggdb'
    if release:
        make += ' -O3'
    # -lSDL2_image #-pthread
    make += '\nSOURCES = src/*.cpp'
    make += '\nOBJECTS := ${subst src/,,$(SOURCES:.cpp=.o)}'
    make += ('\n\nall: bin/simulation'
             '\n\nbin/simulation: $(OBJECTS)'
             '\n\t$(COMPILER) $(OBJECTS) $(LINKER_FLAGS) -o bin/simulation'
             '\n\n$(OBJECTS): $(SOURCES) $(SOURCES:.cpp=hpp)'
             '\n\t$(COMPILER) -c $(SOURCES) $(COMPILER_FLAGS)')
    with open('Makefile', 'w') as makefile:
        makefile.write(make)


def createFolders():
    if not os.path.exists('./bin'):
        os.makedirs('./bin')
    if not os.path.exists('./input'):
        os.makedirs('./input')
    if not os.path.exists('./output'):
        os.makedirs('./output')

def mergeGraph(branch):
    merged = None
    for key, child in branch.iteritems():
        adapted = []
        for element in child:
            adapted.append({key:deepcopy(element)})
        if not merged:
            merged = adapted
        else:
            oldMerged = merged
            merged = []
            for el1 in merged:
                for el2 in adapted:



def function(articleData, data, path, results):
    local = data
    for key in path:
        local = local[key]
    if type(local) == list:
        for index, _ in enumerate(local):
            if (not function(articleData, data, path + [index], results))
            results.append()
        return True
    elif type(local) == dict:
        outcome = {}
        branch = {}
        for key in local.keys():
            children.[key] = function(articleData, data, path + [key], results)
        mergeGraph(branch)
        return outcome
    else:
        return [deepcopy(local)]

def createParameters():
    with open('./param/article_physics_parameters.json') as f:
        articleData = json.load(f)
    with open('./param/full_physics_parameters.json') as f:
        fullData = json.load(f)
    paramList = function(articleData, fullData, [], [])
    print(paramList)

# ./initializer.py -d --SDL2
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Compiles, runs simulations and plots results')
    parser.add_argument('-d', '--debug', action='store_true', help='compile in debug mode')
    parser.add_argument('-r', '--release', action='store_true', help='compile in release mode')
    parser.add_argument('--SDL2', action='store_true', help='do not compile with SDL2 libraries')
    parser.add_argument('-gslC', '--gslCompileDir', action='store', default='/usr/local/include', help='the absolute path of the directory where the gsl library include is installed')
    parser.add_argument('-gslL', '--gslLinkDir', action='store', default='/usr/local/lib', help='the absolute path of the directory where the gsl library lib is installed')
    args = parser.parse_args()
        
    print('\n--- Creating folders if needed')
    createFolders()

    print('\n--- Create parameters files')
    createParameters()

    # print('\n--- Creating makefile')
    # # gslCompileDir = str(Path.joinpath(Path().absolute(), 'gsl/include'))
    # # gslLinkDir = str(Path.joinpath(Path().absolute(), 'gsl/lib'))
    # createMake(SDL2=args.SDL2, debug=args.debug, release=args.release, gslCompileDir=args.gslCompileDir, gslLinkDir=args.gslLinkDir)

    # print('\n--- Compiling code:')
    # outcome = subprocess.run('make')
    # print('--- Outcome: ' + str(outcome))


    # print('\n--- Running simulation program:')
    # myEnv = os.environ.copy()
    # # myEnv['LD_LIBRARY_PATH'] = gslLinkDir
    # outcome = subprocess.run('bin/simulation', env=myEnv)
    # print('--- Outcome: ' + str(outcome))

    # print('\n--- Running plotter script:')
    # outcome = subprocess.run('./plotter.py')
    # print('--- Outcome: ' + str(outcome))
