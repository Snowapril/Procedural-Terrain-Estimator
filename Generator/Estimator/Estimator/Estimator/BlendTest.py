import numpy as np
import matplotlib.pyplot as plt

def read_integers(filename):
    with open(filename) as f:
        return [int(x) for l in f for x in l.split()]

array = read_integers('outBlend.txt')
array = np.asarray(array).reshape(2048, 2048, 4)

plt.imshow(array)
plt.show()