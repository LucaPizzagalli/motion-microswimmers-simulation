#!/usr/bin/env python3
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
import numpy as np
import math
from scipy.optimize import curve_fit
import glob
import argparse


# def my_distribution_func(x, alpha, gamma=1):
#     return alpha**(gamma+1)/math.gamma(gamma+1)*x**gamma*math.e**(-alpha*x)

# def force_func(distance, radius):
#     if distance < radius*pow(2, 1/6):
#         return 24*10*250*(2*(radius**12)/(distance**13)-(radius**6)/(distance**7))
#     else:
#         return 0


# def plot_my_distribution():
#     points = np.linspace(1/8, 4-1/8, 16)
#     experimental_values = [2.5, 40, 185, 132.5, 62.5, 25, 17.5, 7.5, 7.5, 7.5, 5, 5, 5, 5, 2.5, 0]
#     experimental_values = [num/(sum(experimental_values)*0.25) for num in experimental_values]
#     plt.figure(figsize=(8, 8), dpi=80, facecolor='w', edgecolor='k')
#     plt.plot(points, experimental_values)

#     params, _ = curve_fit(my_distribution_func, points, experimental_values, [1,1])

#     print(params)

#     points = np.linspace(1/8, 4-1/8, 400)
#     my_distribution = [my_distribution_func(point, *params) for point in points]
#     plt.plot(points, my_distribution)
#     plt.show()


# def plot_force():
#     plt.figure(figsize=(6, 6), dpi=80, facecolor='w', edgecolor='k')
#     plt.yscale('log')
#     points = np.linspace(1, 6, 100)
#     body_force = [force_func(distance, 2.5) for distance in points]
#     plt.plot(points, body_force)
#     flagella_force = [force_func(distance, 5) for distance in points]
#     plt.plot(points, flagella_force)
#     plt.show()


# def plot_radial_probability(filename):
#     plt.figure(figsize=(6, 6), dpi=80, facecolor='w', edgecolor='k')
#     radialProbability = np.loadtxt(filename, delimiter=',')
#     plt.plot(radialProbability[:, 0], radialProbability[:, 1])
#     plt.savefig(filename[:-4] + '.png', bbox_inches='tight')

def plot_diffusion(filename):
    plt.figure(figsize=(16, 16), dpi=80, facecolor='w', edgecolor='k')
    radialProbability = np.loadtxt(filename, delimiter=',')
    plt.scatter(radialProbability[:, 0], radialProbability[:, 1], s=[0.1]*len(radialProbability[:, 0]))
    plt.savefig(filename[:-4] + '.png', bbox_inches='tight')


def plot_radial_probability_all(filenames):
    fig = plt.figure(figsize=(6, 6), dpi=80, facecolor='w', edgecolor='k')
    ax = fig.add_subplot(111)
    plt.xlabel('distance r from the center')
    plt.ylabel('P(r) $[\mu m]$')
    xmax = 0
    for filename in filenames:
        xmax = max(xmax, np.loadtxt(filename, delimiter=',')[:, 0][-1])
    ax.set_xlim(left=5.0, right=xmax)
    plt.set_cmap(plt.get_cmap('winter'))
    plt.gca().set_prop_cycle(plt.cycler('color', plt.cm.jet(np.linspace(0.1, 0.9, len(filenames)))))
    for filename in filenames:
        radialProbability = np.loadtxt(filename, delimiter=',')
        plt.plot(radialProbability[:, 0], radialProbability[:, 1], alpha=0.7, label=filename[7:].split('*')[0])
        plt.legend()
    filename = filename.split(' = ')[0] + filename[:-4].split(' * ')[1] + '.png'
    filename = filename.replace('\\','')
    filename = filename.replace('$','')
    filename = filename.replace('{','')
    filename = filename.replace('}','')
    filename = filename.replace(' ','')
    plt.savefig(filename, bbox_inches='tight')


# def plot_displacement(filename):
#     fig = plt.figure(figsize=(6, 6), dpi=80, facecolor='w', edgecolor='k')
#     displacement = np.loadtxt(filename, delimiter=',')
#     ax = fig.add_subplot(111)
#     ax.set_xlim(left=0.01, right=displacement[:, 0][-1])
#     plt.xlabel('$t [s]$')
#     plt.ylabel('$< \Delta r^2 > [\mu m^2]$')
#     plt.yscale('log')
#     plt.xscale('log')
#     plt.plot(displacement[:, 0], displacement[:, 1])
#     plt.savefig(filename[:-4] + '.png', bbox_inches='tight')


def plot_displacement_all(filenames):
    fig = plt.figure(figsize=(6, 6), dpi=80, facecolor='w', edgecolor='k')
    ax = fig.add_subplot(111)
    displacement = np.loadtxt(filenames[0], delimiter=',')
    plt.set_cmap(plt.get_cmap('winter'))
    plt.gca().set_prop_cycle(plt.cycler('color', plt.cm.jet(np.linspace(0.1, 0.9, len(filenames)))))
    xmax = 0
    for filename in filenames:
        xmax = max(xmax, np.loadtxt(filename, delimiter=',')[:, 0][-1])
    ax.set_xlim(left=0.01, right=xmax)
    # ax.set_ylim(bottom=100, top=10000000)
    plt.xlabel('$t [s]$')
    plt.ylabel('$< \Delta r^2 > [\mu m^2]$')
    plt.yscale('log')
    plt.xscale('log')
    for filename in filenames:
        displacement = np.loadtxt(filename, delimiter=',')
        time = list(displacement[:, 0][:100])
        disp = list(displacement[:, 1][:100])
        i = 0
        while 101+int(pow(1.05, i)) < len(displacement[:, 0]):
            time.append(displacement[:, 0][101 + int(pow(1.05, i))])
            disp.append(sum(displacement[:, 1][100 + int(pow(1.05, (i-1))) : 101 + int(pow(1.05, i))])/(1 + int(pow(1.05, i)) - int(pow(1.05, (i-1)))))
            i += 1
        time.append(displacement[:, 0][-1])
        disp.append(sum(displacement[:, 1][100 + int(pow(1.05, (i-1))) :])/(len(displacement[:, 1]) - int(pow(1.05, (i-1)))))
        plt.plot(time, disp, alpha=0.7, label=filename[7:].split('*')[0])
        plt.legend()
    filename = filename.split(' = ')[0] + filename[:-4].split(' * ')[1] + '.png'
    filename = filename.replace('\\','')
    filename = filename.replace('$','')
    filename = filename.replace('{','')
    filename = filename.replace('}','')
    filename = filename.replace(' ','')
    plt.savefig(filename, bbox_inches='tight')


def plot_trajectory(filename):
    coord = np.loadtxt(filename, delimiter=',')
    minx = min(coord[:, 1])
    miny = min(coord[:, 2])
    maxx = max(coord[:, 1])
    maxy = max(coord[:, 2])
    size = max(maxx-minx,maxy-miny)
    fig = plt.figure(figsize=(8, 8), dpi=800, facecolor='w', edgecolor='k')
    ax = fig.add_subplot(111)
    ax.set_xlim(left=(maxx+minx-size)/2, right=(maxx+minx+size)/2)
    ax.set_ylim(bottom=(maxy+miny-size)/2, top=(maxy+miny+size)/2)
    ax.add_line(Line2D(coord[:, 1], coord[:, 2], linewidth=0.1))
    # ax.scatter(coord[:, 1], coord[:, 2], [0.01]*len(coord[:, 0]))
    plt.savefig(filename[:-4] + '.png', bbox_inches='tight')

def plot_density(filename):
    probabilityMap = np.loadtxt(filename, delimiter=',')
    if len(probabilityMap.shape) == 1:
        fig = plt.figure(figsize=(8, 3), dpi=80, facecolor='w', edgecolor='k')
        ax = fig.add_subplot(111)
        # plt.xlabel('$t [s]$')
        # plt.ylabel('$< \Delta r^2 > [\mu m^2]$')
        # plt.yscale('log')
        # plt.xscale('log')
        plt.plot(probabilityMap)
        plt.savefig(filename[:-4] + '.png', bbox_inches='tight')
    else:
        plt.imshow(probabilityMap, cmap='hot', interpolation='nearest')
        plt.savefig(filename[:-4] + '.png', bbox_inches='tight')

def plot_density_all(filenames):
    fig = plt.figure(figsize=(8, 4), dpi=80, facecolor='w', edgecolor='k')
    ax = fig.add_subplot(111)
    plt.xlabel('')
    plt.ylabel('')
    plt.set_cmap(plt.get_cmap('winter'))
    plt.gca().set_prop_cycle(plt.cycler('color', plt.cm.jet(np.linspace(0.1, 0.9, len(filenames)))))
    for filename in filenames:
        concentration = np.loadtxt(filename, delimiter=',')
        plt.plot(concentration, alpha=0.7, label=filename[7:].split('*')[0])
        plt.legend()
    filename = filename.split(' = ')[0] + filename[:-4].split(' * ')[1] + '.png'
    filename = filename.replace('\\','')
    filename = filename.replace('$','')
    filename = filename.replace('{','')
    filename = filename.replace('}','')
    filename = filename.replace(' ','')
    plt.savefig(filename, bbox_inches='tight')

def main():
    parser = argparse.ArgumentParser(description='Plots stuff')
    parser.add_argument('-m', '--mapFile', action='store', default='', help='plots map propability file')
    parser.add_argument('-ma', '--mapFileAll', nargs='+', default=[], help='plots map propability files')
    parser.add_argument('-r', '--radialFile', action='store', default='', help='plots radial probability file')
    parser.add_argument('-ra','--radialFileAll', nargs='+', default=[], help='plots all the radial probability in one figure')
    parser.add_argument('-d', '--displacementFile', action='store', default='', help='plots displacement probability file')
    parser.add_argument('-da', '--displacementFileAll', nargs='+', default=[], help='plots all the displacement probability files in one figure')
    parser.add_argument('-t', '--trajectoryFile', action='store', default='', help='plots trajectory file')
    parser.add_argument('-di', '--diffusionFile', action='store', default='', help='plots all the diffusion files')
    args = parser.parse_args()

    # plot_force()
    # plot_my_distribution()

    if(len(args.mapFile)>0):
        print('Creating probability map plot...')
        plot_density('output/' + args.mapFile)

    if(len(args.mapFileAll)>0):
        print('Creating probability map plots...')
        for index, value in enumerate(args.mapFileAll):
            args.mapFileAll[index] = 'output/' + value
        plot_density_all(args.mapFileAll)

    # if(len(args.radialFile)>0):
    #     print('Creating radial probability plot...')
    #     plot_radial_probability('output/' + args.radialFile)

    if(len(args.radialFileAll)>0):
        print('Creating radial probability plots...')
        for index, value in enumerate(args.radialFileAll):
            args.radialFileAll[index] = 'output/' + value
        plot_radial_probability_all(args.radialFileAll)

    # if(len(args.displacementFile)>0):
    #     print('Creating displacement probability plot...')
    #     plot_displacement('output/' + args.displacementFile)

    if(len(args.displacementFileAll)>0):
        print('Creating displacement probability plots...')
        for index, value in enumerate(args.displacementFileAll):
            args.displacementFileAll[index] = 'output/' + value
        plot_displacement_all(args.displacementFileAll)

    if(len(args.trajectoryFile)>0):
        print('Creating trajectory plot...')
        plot_trajectory('output/' + args.trajectoryFile)

    if(len(args.diffusionFile)>0):
        print('Creating diffusion plot...')
        plot_diffusion('output/' + args.diffusionFile)


if __name__ == '__main__':
    main()
