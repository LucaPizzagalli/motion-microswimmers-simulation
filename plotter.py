import matplotlib.pyplot as plt
import numpy as np

probabilityMap = np.genfromtxt('probability_map.csv', delimiter=',')
plt.imshow(probabilityMap, cmap='hot', interpolation='nearest')
plt.show()