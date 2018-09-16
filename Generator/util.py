import numpy as np
import math
import noise
import time

def fract1(x) :
    return x - int(x)

def fract2(x) :
    return x - x.astype(np.int)

def rand1(n) :
    return fract1(math.sin(n) * 43758.5453123)

def rand2(n) :
    return fract1(math.sin(np.dot(n, np.array([12.9898, 4.1414]))) * 43758.5453)

def mix(x, y, a) :
    return x * (1 - a) + y * a

def noise2(x, y) :
    n = np.array([x, y], dtype=np.float32)
    d = np.array([0.0, 1.0], dtype=np.float32)

    b = n.astype(np.int)
    f = smoothstep(np.array([0.0, 0.0]), np.array([1.0, 1.0]), fract2(n))

    return mix(mix(rand2(b), rand2(b + np.array([d[1], d[0]])), f[0]),
               mix(rand2(b + np.array([d[0], d[1]])), rand2(b + np.array([d[1], d[1]])), f[0]), f[1])

def fbm2(x, y, num_octaves = 5) :
    n = np.array([x, y])

    v = 0.0
    a = 0.5

    coscos = math.cos(0.5)
    sinsin = math.sin(0.5)

    shift = np.array([100, 100])
    rot = np.array([[coscos,sinsin ], [-sinsin, coscos]])

    for i in range(num_octaves) :
        v += a * noise2(n[0], n[1])
        n = np.matmul(rot, n) * 2.0 + shift
        a *= 0.5

    return v

def fract(n):
    return n-np.floor(n)

def vnoise2(x, y) :
    t0 = time.clock()
    n = np.array([x, y]).astype(np.float32)

    F1 = 8.0

    for j in range(-1, 2) :
        for i in range(-1, 2) :
            g = np.array([i, j]).astype(np.float32)
            p = n+g
            o = fract2(np.sin(np.array([np.dot(p, np.array([127.1, 311.7])), np.dot(p, np.array([269.5, 183.3]))])) * 43758.5453)
            o = 0.5 + 0.41 * np.sin(6.2831 * o)
            r = g + o

            d = np.dot(r, r)

            if (d < F1) :
                F1 = d

    c = F1

    t1 = time.clock()
    # print("%.8f" % (t1-t0))

    return c