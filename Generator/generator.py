import png

import numpy as np

import simplexNoise

from scipy.ndimage import gaussian_filter

def main():
    f = open('foo.png', 'wb')      # binary mode is important
    w = png.Writer(1024, 1024, greyscale=True)
    np.random.seed(12345)
    x = np.random.randint(0,256,(1024,1024))
    y = gaussian_filter(x, (8, 8))
    w.write(f, y)
    f.close()