#!/usr/bin/env python3
import os
from pathlib import Path
import subprocess
import argparse
import json
from copy import deepcopy
import math


def createFolders(clear):
    if clear:
        subprocess.run(['rm', '-r', './input'])
        subprocess.run(['rm', '-r', './output'])
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
            adapted.append(([key] + element[0], {key: deepcopy(element[1])}))
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
            outcome += getCombinationTrees(element,
                                           referenceTree, path + [index])
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


def generateinitialConditions(initialConditions):
    for kind in initialConditions:
        for index, element in enumerate(initialConditions[kind]):
            if 'grid' in element:
                array = []
                edge = int(math.sqrt(element['grid']['number']) + 0.5)
                space = element['grid']['separation']
                centerX = element['grid']['position']['x']
                centerY = element['grid']['position']['y']
                for i in range(edge):
                    for j in range(edge):
                        dictionary = {
                            'direction': element['grid']['direction']}
                        dictionary['position'] = {
                            'x': centerX + (i - (edge-1)/2) * space, 'y': centerY + (j - (edge-1)/2) * space}
                        array.append(dictionary)
                del initialConditions[kind][index]
                initialConditions[kind].extend(array)
    return initialConditions


def createParameters():
    with open('./param/article_physics_parameters.json') as f:
        articleData = json.load(f)['parameters']
    with open('./param/full_physics_parameters.json') as f:
        fullData = json.load(f)
        unitOfMeasure = fullData['unitOfMeasure']
        initialConditions = fullData['initialConditions']
        fullData = fullData['parameters']

    initialConditions = generateinitialConditions(initialConditions)

    paramList = getCombinationTrees(fullData, articleData, [])
    paramDict = fromListToDict(paramList)
    nameDict = {}
    for key, value in paramDict.items():
        nameDict[key] = []
        for index, element in enumerate(value):
            filename = '_'.join(key) + '_' + str(index) + '.json'
            nameDict[key].append(filename)
            output = {'unitOfMeasure': unitOfMeasure, 'parameters': element,
                      'initialConditions': initialConditions}
            with open('input/' + filename, 'w') as outfile:
                json.dump(output, outfile)
    return nameDict


def runSimulations(nameDict):
    myEnv = os.environ.copy()
    # myEnv['LD_LIBRARY_PATH'] = gslLinkDir
    for key, value in nameDict.items():
        print('\n--- Varying element: ' + str(key))
        for element in value:
            print('\n--- build/swimmers-brownian-simulation ' + element)
            subprocess.run(
                ['build/swimmers-brownian-simulation', element], env=myEnv)


def parseArguments():
    parser = argparse.ArgumentParser(
        description='Compiles, runs simulations and plots results')
    parser.add_argument('-d', '--debug', action='store_true',
                        help='compile in debug mode')
    parser.add_argument('-r', '--release', action='store_true',
                        help='compile in release mode')
    parser.add_argument('-c', '--clear', action='store_true',
                        help='clear the folders input and output before the computation')
    parser.add_argument('--SDL2', action='store_true',
                        help='do not compile with SDL2 libraries')
    parser.add_argument('-gslC', '--gslCompileDir', action='store', default='/usr/local/include',
                        help='the absolute path of the directory where the gsl library include is installed')
    parser.add_argument('-gslL', '--gslLinkDir', action='store', default='/usr/local/lib',
                        help='the absolute path of the directory where the gsl library lib is installed')
    return parser.parse_args()


# ./initializer.py -d --SDL2 -c
if __name__ == '__main__':
    args = parseArguments()

    print('\n--- Deleting/creating folders if needed')
    createFolders(args.clear)

    print('\n--- Creating parameters files')
    nameDict = createParameters()

    print('\n--- Compiling code:')
    subprocess.run('ninja', cwd='build')

    print('\n--- Running simulation program:')
    runSimulations(nameDict)

    print('\n--- Running plotter script:')
    with open('./param/simulation_parameters.json') as f:
        simulationParameters = json.load(f)

    for key, value in nameDict.items():
        for element in value:
            if simulationParameters['compute_probability_map']:
                subprocess.run(['./plotter.py', '-m', element[:-5] + '_probability_map.csv', '-r', element[:-5] + '_radial_probability.csv'])
            if simulationParameters['compute_displacement']:
                subprocess.run(['./plotter.py', '-d', element[:-5] + '_displacement.csv'])
