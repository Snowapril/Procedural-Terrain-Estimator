import util
import numpy as np


def getfbM(_width, _height) :
    ret = np.zeros((_width, _height, 3))

    for i in range(0, _width) :
        for j in range(0, _height) :
            for k in range(0, 3) :
                ret[i][j][k] += util.fbm2(i / 100.0, j / 100.0)

    return ret

def getPerlin(_width, _height):
    ret = np.zeros((_width, _height, 3))
    for i in range(0, _width):
        for j in range(0, _height):
            for k in range(0,3):
                ret[i][j][k] = util.noise.snoise2(i / 100.0, j / 100.0, 2)
    return ret

def getSimplex(_width, _height):
    ret = np.zeros((_width, _height, 3))
    for i in range(0, _width):
        for j in range(0, _height):
            for k in range(0,3):
                ret[i][j][k] += util.noise.pnoise2(i / 100.0, j / 100.0, 2)
    return ret

def getVoronoi(_width, _height):
    ret = np.zeros((_width, _height, 3))
    for i in range(0, _width):
        for j in range(0, _height):
            for k in range(0,3):
                ret[i][j][k] += util.vnoise(i / 100.0, j / 100.0, 1, 1)
    return ret


def makeNoise(_width, _height):
    ret = np.zeros((_width, _height, 3))
    
    w_simplex = 0.25
    w_perlin = 0.25
    w_voronoi = 0.25
    w_fbm = 0.25

    p_arr = getPerlin(_width, _height)
    s_arr = getSimplex(_width, _height)
    v_arr = getVoronoi(_width, _height)
    f_arr = getfbM(_width, _height)
    return ret

def main():
    x = makeNoise(1024,1024)
    print(x)

if __name__ == "__main__":
    main()