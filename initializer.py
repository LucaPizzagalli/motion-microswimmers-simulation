#!/usr/bin/env python3

import os
import subprocess
import json
from pprint import pprint

print("--- Creating folders if needed:")
if not os.path.exists('./bin'):
    os.makedirs('./bin')
if not os.path.exists('./input'):
    os.makedirs('./input')
if not os.path.exists('./output'):
    os.makedirs('./output')

print("--- Compiling code:")
outcome = subprocess.run('make')
print("--- Outcome: " + str(outcome))

with open('./param/full_physics_parameters.json') as f:
    data = json.load(f)

# pprint(data)

print("--- Running simulation program:")
outcome = subprocess.run('bin/simulation')
print("--- Outcome: " + str(outcome))

print("--- Running plotter script:")
outcome = subprocess.run('./plotter.py')
print("--- Outcome: " + str(outcome))