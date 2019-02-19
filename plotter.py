#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np
import math
from scipy.optimize import curve_fit
import glob
import argparse


def my_distribution_func(x, alpha, gamma=1):
    return alpha**(gamma+1)/math.gamma(gamma+1)*x**gamma*math.e**(-alpha*x)

def force_func(distance, radius):
    if distance < radius*pow(2, 1/6):
        return 24*10*250*(2*(radius**12)/(distance**13)-(radius**6)/(distance**7))
    else:
        return 0


def plot_my_distribution():
    points = np.linspace(1/8, 4-1/8, 16)
    experimental_values = [2.5, 40, 185, 132.5, 62.5, 25, 17.5, 7.5, 7.5, 7.5, 5, 5, 5, 5, 2.5, 0]
    experimental_values = [num/(sum(experimental_values)*0.25) for num in experimental_values]
    plt.figure(figsize=(8, 8), dpi=80, facecolor='w', edgecolor='k')
    plt.plot(points, experimental_values)

    params, _ = curve_fit(my_distribution_func, points, experimental_values, [1,1])

    print(params)

    points = np.linspace(1/8, 4-1/8, 400)
    my_distribution = [my_distribution_func(point, *params) for point in points]
    plt.plot(points, my_distribution)
    plt.show()


def plot_force():
    plt.figure(figsize=(8, 8), dpi=80, facecolor='w', edgecolor='k')
    plt.yscale('log')
    points = np.linspace(1, 6, 100)
    body_force = [force_func(distance, 2.5) for distance in points]
    plt.plot(points, body_force)
    flagella_force = [force_func(distance, 5) for distance in points]
    plt.plot(points, flagella_force)
    plt.show()


def plot_radial_probability(filename):
    plt.figure(figsize=(8, 8), dpi=80, facecolor='w', edgecolor='k')
    radialProbability = np.loadtxt(filename, delimiter=',')
    plt.plot(radialProbability[:, 0], radialProbability[:, 1])
    plt.savefig(filename[:-4] + '.png', bbox_inches='tight')


def plot_displacement(filename):
    plt.figure(figsize=(8, 8), dpi=80, facecolor='w', edgecolor='k')
    displacement = np.loadtxt(filename, delimiter=',')
    plt.plot(displacement[:, 0], displacement[:, 1])
    plt.savefig(filename[:-4] + '.png', bbox_inches='tight')


def plot_density(filename):
    probabilityMap = np.loadtxt(filename, delimiter=',')
    plt.imshow(probabilityMap, cmap='hot', interpolation='nearest')
    plt.savefig(filename[:-4] + '.png', bbox_inches='tight')


def plot_traj(pos_x, pos_y):
    minx = min(pos_x)
    miny = min(pos_y)
    maxx = max(pos_x)
    maxy = max(pos_y)
    size = max(maxx-minx,maxy-miny)
    fig = plt.figure(figsize=(8, 8), dpi=80, facecolor='w', edgecolor='k')
    ax = fig.add_subplot(111)
    ax.set_xlim(left=(maxx+minx-size)/2, right=(maxx+minx+size)/2)
    ax.set_ylim(bottom=(maxy+miny-size)/2, top=(maxy+miny+size)/2)
    ax.scatter(pos_x, pos_y, [0.01]*len(pos_x))


def main():
    parser = argparse.ArgumentParser(description='Plots stuff')
    parser.add_argument('-m', '--mapFile', action='store', default='', help='map propability file')
    parser.add_argument('-r', '--radialFile', action='store', default='', help='radial probability file')
    parser.add_argument('-d', '--displacementFile', action='store', default='', help='displacement probability file')
    args = parser.parse_args()

    # plot_force()
    # plot_my_distribution()

    if(len(args.mapFile)>0):
        print('Creating probability map plot...')
        plot_density('output/' + args.mapFile)

    if(len(args.radialFile)>0):
        print('Creating radial probability plot...')
        plot_radial_probability('output/' + args.radialFile)

    if(len(args.displacementFile)>0):
        print('Creating displacement probability plot...')
        plot_displacement('output/' + args.displacementFile)


if __name__ == '__main__':
    main()
