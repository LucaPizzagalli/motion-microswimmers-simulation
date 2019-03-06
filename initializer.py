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


def getUpdatedTree(tree, path, value):
    if len(path) == 1:
        tree[path[0]] = value
    else:
        getUpdatedTree(tree[path[0]], path[1:], value)


def getListTrees(tree, referenceTree, path):
    allTrees = []
    if type(tree) == dict:
        for key, subTree in tree.items():
            allTrees.extend(getListTrees(subTree, referenceTree, path + [key]))
    elif type(tree) == list:
        newList = []
        for element in tree[1:]:
            newTree = deepcopy(referenceTree)
            getUpdatedTree(newTree, path, element)
            newList.append((tree[0] + ' = ' + str(element) + ' * ', newTree))
        allTrees.append(newList)
    return allTrees


def generateinitialConditions(initialConditions):
    for kind in initialConditions:
        for index, element in enumerate(initialConditions[kind]):
            if 'grid' in element:
                array = []
                rows = element['grid']['rows']
                columns = element['grid']['rows']
                space = element['grid']['separation']
                centerX = element['grid']['position']['x']
                centerY = element['grid']['position']['y']
                for x in range(columns):
                    for y in range(rows):
                        dictionary = {
                            'direction': element['grid']['direction']}
                        dictionary['position'] = {'x': centerX + (x - (columns-1)/2) * space, 'y': centerY + (y - (rows-1)/2) * space}
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

    allTrees = getListTrees(fullData, articleData, [])
    for treeList in allTrees:
        for tree in treeList:
            filename = tree[0] + '.json'
            output = {'unitOfMeasure': unitOfMeasure, 'parameters': tree[1], 'initialConditions': initialConditions}
            with open('input/' + filename, 'w') as outfile:
                json.dump(output, outfile)
    return allTrees


def runSimulations(allTrees):
    myEnv = os.environ.copy()
    # myEnv['LD_LIBRARY_PATH'] = gslLinkDir
    for treeList in allTrees:
        print('\n--- Varying element: ' + treeList[0][0].split(' = ')[0])
        for tree in treeList:
            filename = tree[0] + '.json'
            print('\n--- build/swimmers-brownian-simulation ' + filename)
            subprocess.run(['build/swimmers-brownian-simulation', filename], env=myEnv)


def parseArguments():
    parser = argparse.ArgumentParser(
        description='Compiles, runs simulations and plots results')
    parser.add_argument('-d', '--debug', action='store_true', help='compile in debug mode')
    parser.add_argument('-r', '--release', action='store_true', help='compile in release mode')
    parser.add_argument('-c', '--clear', action='store_true', help='clear the folders input and output before the computation')
    parser.add_argument('--SDL2', action='store_true', help='do not compile with SDL2 libraries')
    parser.add_argument('-gslC', '--gslCompileDir', action='store', default='/usr/local/include', help='the absolute path of the directory where the gsl library include is installed')
    parser.add_argument('-gslL', '--gslLinkDir', action='store', default='/usr/local/lib', help='the absolute path of the directory where the gsl library lib is installed')
    return parser.parse_args()


def plotResults(allTrees):
    with open('./param/simulation_parameters.json') as f:
        simulationParameters = json.load(f)

    if simulationParameters['save_trajectory']:
        subprocess.run(['./plotter.py', '-t', '0_trajectory.csv'])

    for treeList in allTrees:
        if simulationParameters['plot_probability_map']:
            for tree in treeList:
                subprocess.run(['./plotter.py', '-m', tree[0] + '_probability_map.csv'])
        if simulationParameters['plot_radial_probability']:
            radialProbabilityFile = []
            for tree in treeList:
                radialProbabilityFile.append(tree[0] + '_radial_probability.csv')
            subprocess.run(['./plotter.py', '-ra'] + radialProbabilityFile)
        if simulationParameters['compute_displacement']:
            displacementFile = []
            for tree in treeList:
                displacementFile.append(tree[0] + '_displacement.csv')
            subprocess.run(['./plotter.py', '-da'] + displacementFile)


# ./initializer.py -d --SDL2 -c
if __name__ == '__main__':
    args = parseArguments()

    print('\n--- Deleting/creating folders if needed')
    createFolders(args.clear)

    print('\n--- Creating parameters files')
    allTrees = createParameters()

    print('\n--- Compiling code:')
    subprocess.run('ninja', cwd='build')

    print('\n--- Running simulation program:')
    runSimulations(allTrees)

    print('\n--- Running plotter script:')
    plotResults(allTrees)

    print('\n--- All done.')
