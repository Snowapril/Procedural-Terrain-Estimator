import png

import numpy as np

import genNoise

from scipy.ndimage import gaussian_filter

def main():
    f = open('foo_voronoi.png', 'wb')      # binary mode is important
    w = png.Writer(1024, 1024, bitdepth=16)
    # x = genNoise.makeNoise(1024,1024)
    x = genNoise.getNoise(1024, 1024, num_thread=16, noise_func=genNoise.getVoronoi)
    # print(x)
    z = (65535*((x - x.min())/x.ptp())).astype(np.uint16)
    # print(z)
    z2list = z.reshape(-1, z.shape[1] * z.shape[2]).tolist()
    w.write(f, z2list)
    f.close()

if __name__ == "__main__":
    main()