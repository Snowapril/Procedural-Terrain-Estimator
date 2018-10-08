import numpy as np
import matplotlib.pyplot as plt

def read_integers(filename):
    with open(filename) as f:
        return [int(x) for l in f for x in l.split()]

array = read_integers('outHeight2.txt')
array = np.asarray(array).reshape(2048, 2048)

plt.imshow(array, cmap='gray')
plt.show()