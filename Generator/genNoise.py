import util
import numpy as np
import time
import math
from concurrent.futures import ThreadPoolExecutor, wait, ALL_COMPLETED

def getfbM(_x, _y, _width, _height) :
    ret = np.zeros((_width, _height, 3))

    for i in range(0, _width) :
        for j in range(0, _height) :
            x = util.fbm2((_x + i) / 100.0, (_y + j) / 100.0)
            for k in range(0, 3) :
                ret[i][j][k] += x

    return ret

def getPerlin(_x, _y, _width, _height):
    ret = np.zeros((_width, _height, 3))
    for i in range(0, _width):
        for j in range(0, _height):
            x = util.noise.snoise2((_x + i) / 100.0, (_y + j) / 100.0, 2)
            for k in range(0,3):
                ret[i][j][k] = x

    return ret

def getSimplex(_x, _y, _width, _height):
    ret = np.zeros((_width, _height, 3))
    for i in range(0, _width):
        for j in range(0, _height):
            x = util.noise.pnoise2((i + _x) / 100.0, (j + _y) / 100.0, 2)
            for k in range(0,3):
                ret[i][j][k] += x
    return ret

def getVoronoi(_x, _y, _width, _height):
    ret = np.zeros((_width, _height, 3))
    for i in range(0, _width):
        for j in range(0, _height):
            x = util.vnoise2((_x + i) / 100.0, (_y + j) / 100.0)
            ret[i][j][0:3] = [x, x, x]

    print (ret)
    return ret

def getNoise(_width, _height, num_thread, noise_func) :
    bandwidth = math.sqrt(num_thread)

    if math.isclose(bandwidth, int(bandwidth)) == False :
        raise Exception("num thread argument must be square of two")

    startTime = time.clock()
    result = None

    with ThreadPoolExecutor(max_workers=num_thread) as pool :
        fs = []

        bandwidth = int(bandwidth)

        workerWidth = int(_width / bandwidth)
        workerHeight = int(_height / bandwidth)

        for i in range(0, bandwidth) :
            for j in range(0, bandwidth) :
                fs.append(pool.submit(noise_func, j * workerWidth, i * workerHeight, workerWidth, workerHeight))

        pool.shutdown()

        first = False
        for i in range(0, bandwidth) :
            oneRow = fs[i * bandwidth].result()
            for j in range(1, bandwidth):
                rowTuple = (oneRow, fs[i * bandwidth + j].result())
                oneRow = np.concatenate(rowTuple)

            if first == False:
                result = oneRow
                first = True
            else :
                colTuple = (result, oneRow)
                result = np.concatenate(colTuple, axis=1)

    endTime = time.clock()
    print (endTime - startTime, "second")

    return result


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
    x = getNoise(1024, 1024, 16, getVoronoi)
    print(x)

if __name__ == "__main__":
    main()