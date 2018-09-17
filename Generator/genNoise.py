import util
import numpy as np
import time
import math
from multiprocessing import Process

def getfbM(ret,_x, _y, _width, _height) :
    ret = np.zeros((_width, _height, 3))

    for i in range(0, _width) :
        for j in range(0, _height) :
            x = util.fbm2((_x + i) / 100.0, (_y + j) / 100.0)
            ret[_x+i][_y+j][0:3] = [x, x, x]

    return ret

def getPerlin(ret,_x, _y, _width, _height):
    ret = np.zeros((_width, _height, 3))
    for i in range(0, _width):
        for j in range(0, _height):
            x = util.noise.snoise2((_x + i) / 100.0, (_y + j) / 100.0, 2)
            ret[_x+i][_y+j][0:3] = [x, x, x]

    return ret

def getSimplex(ret,_x, _y, _width, _height):
    ret = np.zeros((_width, _height, 3))
    for i in range(0, _width):
        for j in range(0, _height):
            x = util.noise.pnoise2((i + _x) / 100.0, (j + _y) / 100.0, 2)
            ret[_x+i][_y+j][0:3] = [x, x, x]

def getVoronoi(ret,_x, _y, _width, _height):
    for i in range(0, _width):
        for j in range(0, _height):
            x = util.vnoise2((_x + i) / 100.0, (_y + j) / 100.0)
            ret[_x+i][_y+j][0:3] = [x, x, x]
    print (ret)

def getNoise(_width, _height, num_thread, noise_func) :
    ret = np.zeros((_width, _height, 3))
    bandwidth = int(math.sqrt(num_thread))

    if bandwidth % 2 != 0 :
        raise Exception("num processes argument must be square of even number")

    startTime = time.clock()

    bandwidth = int(bandwidth)

    workerWidth = int(_width / bandwidth)
    workerHeight = int(_height / bandwidth)

    processes = []
    for i in range(0, bandwidth):
        for j in range(0, bandwidth):
            process = Process(target=noise_func, args=(ret, j * workerWidth, i * workerHeight, workerWidth, workerHeight, ))
            process.start()
            processes.append(process)

    for process in processes:
        process.join()

    endTime = time.clock()
    print (endTime - startTime, "second")

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
    x = getNoise(1024, 1024, 4, getVoronoi)
    print(x)

if __name__ == "__main__":
    main()