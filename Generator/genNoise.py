import numpy as np

import math

import noise

def makeNoise(_width, _height):
    ret = np.zeros((_width, _height, 3))
    
    for i in range(0, _width):
        for j in range(0, _height):
            for k in range(0,3):
                ret[i][j][k] = noise.snoise2(i / 100.0, j / 100.0, 2)
    return ret

def main():
    x = makeNoise(1024,1024)
    print(x)

if __name__ == "__main__":
    main()