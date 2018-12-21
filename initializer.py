#!/usr/bin/env python3

import os
from pathlib import Path
import subprocess
import argparse
import json
from copy import deepcopy
import pprint


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
    make += ' -pthread'
    # -lSDL2_image
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
    subprocess.run(['rm', '-r', 'obj'])
    subprocess.run(['rm', '-r', 'input'])
    subprocess.run(['rm', '-r', 'output'])
    if not os.path.exists('./bin'):
        os.makedirs('./bin')
    if not os.path.exists('./input'):
        os.makedirs('./input')
    if not os.path.exists('./output'):
        os.makedirs('./output')


def branchExist(tree, path):
    local = tree
    for key in path:
        if key in local:
            local = local[key]
        else:
            return False
    return local

def mergeGraph(listTree, keys):
    merged = None
    for key, child in listTree.items():
        adapted = []
        for element in child:
            adapted.append(([key] + element[0], {key:deepcopy(element[1])}))
        if not merged:
            merged = adapted
        else:
            oldMerged = merged
            merged = []
            for el1 in oldMerged:
                for el2 in adapted:
                    merged.append({**el1, **el2})
    return merged

def complementListTree(toComplement, referenceBranch):
    outcome = []
    for element in toComplement:
        keys = []
        result = {}
        for key in referenceBranch:
            if key in element[1]:
                result[key] = deepcopy(element[1][key])
                keys.append(key)
            else:
                result[key] = deepcopy(referenceBranch[key])
        outcome.append((keys + element[0], result))
    return outcome

def fromTreeListToListTree(tree, referenceTree, path):
    outcome = []
    base = {}
    localReference = branchExist(referenceTree, path)
    if localReference:
        for key in tree:
            child = getCombinationTrees(tree[key], referenceTree, path + [key])
            if len(child) == 1:
                base[key] = child[0][1]
            else:
                listTree = [(element[0], {key: element[1]}) for element in child]
                outcome += complementListTree(listTree, localReference)
        if not outcome:
            return [([], base)]
    else:
        toMerge = {}
        keys = []
        for key, value in tree.items():
            child = getCombinationTrees(value, referenceTree, path + [key])
            toMerge[key] = child
            if len(child) > 1:
                keys.append(key)
        outcome = mergeGraph(toMerge, keys)
    return outcome

def getCombinationTrees(tree, referenceTree, path):
    if type(tree) == list:
        outcome = []
        for index, element in enumerate(tree):
            outcome += getCombinationTrees(element, referenceTree, path + [index])
        return outcome
    elif type(tree) == dict:
        return fromTreeListToListTree(tree, referenceTree, path)
    else:
        return [([], deepcopy(tree))]

def fromListToDict(data):
    dictionary = {}
    for element in data:
        key = tuple(element[0])
        if key not in dictionary:
            dictionary[key] = []
        dictionary[key].append(element[1])
    return dictionary

def createParameters():
    with open('./param/article_physics_parameters.json') as f:
        articleData = json.load(f)
    with open('./param/full_physics_parameters.json') as f:
        fullData = json.load(f)

    # fullData = {"1":{"H":"H","A":["A","W"]},"2":2,"3":{"U":"U"}}
    # articleData = {"1":{"H":"H","A":"A"},"2":2,"3":{"3":{"U":"U"}}}
    
    paramList = getCombinationTrees(fullData, articleData, [])
    paramDict = fromListToDict(paramList)
    nameDict = {}
    for key, value in paramDict.items():
        nameDict[key] = []
        for index, element in enumerate(value):
            filename = '_'.join(key) + '_' + str(index) + '.json'
            nameDict[key].append(filename)
            with open('input/' + filename, 'w') as outfile:
                json.dump(element, outfile)
    return nameDict


# ./initializer.py -d --SDL2 -c
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Compiles, runs simulations and plots results')
    parser.add_argument('-d', '--debug', action='store_true', help='compile in debug mode')
    parser.add_argument('-r', '--release', action='store_true', help='compile in release mode')
    parser.add_argument('-c', '--clear', action='store_true', help='clear the folders input and output before the computation')
    parser.add_argument('--SDL2', action='store_true', help='do not compile with SDL2 libraries')
    parser.add_argument('-gslC', '--gslCompileDir', action='store', default='/usr/local/include', help='the absolute path of the directory where the gsl library include is installed')
    parser.add_argument('-gslL', '--gslLinkDir', action='store', default='/usr/local/lib', help='the absolute path of the directory where the gsl library lib is installed')
    args = parser.parse_args()
    
    print('\n--- Deleting/creating folders if needed')
    createFolders()

    print('\n--- Creating parameters files')
    nameDict = createParameters()

    print('\n--- Creating makefile')
    # gslCompileDir = str(Pathttps://electrek.co/h.joinpath(Path().absolute(), 'gsl/include'))
    # gslLinkDir = str(Path.joinpath(Path().absolute(), 'gsl/lib'))
    createMake(SDL2=args.SDL2, debug=args.debug, release=args.release, gslCompileDir=args.gslCompileDir, gslLinkDir=args.gslLinkDir)

    print('\n--- Compiling code:')
    subprocess.run('make')

    print('\n--- Running simulation program:')
    myEnv = os.environ.copy()
    # myEnv['LD_LIBRARY_PATH'] = gslLinkDir
    for key, value in nameDict.items():
        print('\n---' + str(key))
        for element in value:
            print('\n--- bin/simulation ' + element)
            outcome = subprocess.run(['bin/simulation', element], env=myEnv)

    print('\n--- Running plotter script:')
    subprocess.run('./plotter.py')
