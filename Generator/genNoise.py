import util
import numpy as np
import time

def getfbM(_width, _height) :
    ret = np.zeros((_width, _height, 3))

    for i in range(0, _width) :
        for j in range(0, _height) :
            x = util.fbm2(i / 100.0, j / 100.0)
            for k in range(0, 3) :
                ret[i][j][k] += x

    return ret

def getPerlin(_width, _height):
    ret = np.zeros((_width, _height, 3))
    for i in range(0, _width):
        for j in range(0, _height):
            x = util.noise.snoise2(i / 100.0, j / 100.0, 2)
            for k in range(0,3):
                ret[i][j][k] = x
    return ret

def getSimplex(_width, _height):
    ret = np.zeros((_width, _height, 3))
    for i in range(0, _width):
        for j in range(0, _height):
            x = util.noise.pnoise2(i / 100.0, j / 100.0, 2)
            for k in range(0,3):
                ret[i][j][k] += x
    return ret

def getVoronoi(_width, _height):
    ret = np.zeros((_width, _height, 3))
    t0 = time.clock()
    for i in range(0, _width):
        # print(str(i))
        for j in range(0, _height):
            x = util.vnoise2(i/100.0, j/100.0)
            for k in range(0,3):
                ret[i][j][k] += x
    t1 = time.clock()
    print("%.8f" % (t1-t0))
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