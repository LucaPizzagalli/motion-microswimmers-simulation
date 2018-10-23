#!/usr/bin/env python3.6
import matplotlib.pyplot as plt
import numpy as np
import math

def force_func(distance, radius):
    if distance < radius*pow(2, 1/6):
        return 24*10*250*(2*(radius**12)/(distance**13)-(radius**6)/(distance**7))
    else:
        return 0


def plot_force():
    plt.figure(figsize=(8, 8), dpi=80, facecolor='w', edgecolor='k')
    ax = plt.gca()
    # ax.set_ylim(bottom=1, top=10)
    plt.yscale('log')
    points = np.linspace(1,6,100)
    body_force = [force_func(distance, 2.5) for distance in points]
    plt.plot(points, body_force)
    flagella_force = [force_func(distance, 5) for distance in points]
    plt.plot(points, flagella_force)
    plt.show()
    #plt.savefig('images/'+ name + filename[13:-4] + '.png', bbox_inches='tight')

def plot_density():
    probabilityMap = np.genfromtxt('probability_map.csv', delimiter=',')
    plt.imshow(probabilityMap, cmap='hot', interpolation='nearest')
    plt.show()
    

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
    plot_force()

if __name__ == '__main__':
    main()
