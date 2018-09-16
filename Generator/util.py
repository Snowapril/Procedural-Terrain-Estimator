import numpy as np
import math
import noise

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

def smoothstep(edge0, edge1, x):
    t = np.clip(((x - edge0) / (edge1 - edge0)), 0.0, 1.0)
    return t * t * (3.0 - 2.0 * t)

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

    shift = np.array([100, 100])
    rot = np.array([[math.cos(0.5), math.sin(0.5)], [-math.sin(0.5), math.cos(0.5)]])

    for i in range(num_octaves) :
        v += a * noise2(n[0], n[1])
        n = np.matmul(rot, n) * 2.0 + shift
        a *= 0.5

    return v

def fract(n):
    return n-np.floor(n)

def hash3(n):
    q = np.array([np.dot(n,np.array([127.1,311.7])), 
				  np.dot(n,np.array([269.5,183.3])), 
				  np.dot(n,np.array([419.2,371.9]))])
    return fract(np.sin(q)*43758.5453)

def vnoise(y, x, u, v):
    p = np.array([y,x])
    k = 1.0 + 63.0*pow(1.0-v,4.0)
    va = 0.0
    wt = 0.0
    for j in range(-2,3):
        for i in range(-2,3):
            g = np.array([i,j])
            o = hash3(p+g)*np.array([u,u,1.0])
            r = g + np.array([o[0],o[1]])
            d = np.dot(r,r)
            w = math.pow(1.0-smoothstep(0.0,1.414,math.sqrt(d)),k)
            va += w*o[2]
            wt += w
    return va/wt