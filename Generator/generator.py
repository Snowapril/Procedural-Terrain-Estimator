import png

import numpy as np

import simplexNoise

# The following import is just for creating an interesting array
# of data.  It is not necessary for writing a PNG file with PyPNG.
from scipy.ndimage import gaussian_filter

def get_colorscale(z):
    # Use pypng to write z as a color PNG.
    with open('foo_color.png', 'wb') as f:
        writer = png.Writer(width=z.shape[1], height=z.shape[0], bitdepth=16)
        # Convert z to the Python list of lists expected by
        # the png writer.
        z2list = z.reshape(-1, z.shape[1]*z.shape[2]).tolist()
        writer.write(f, z2list)


def get_grayscale(z):
    # Here's a grayscale example.
    zgray = z[:, :, 0]
    # Use pypng to write zgray as a grayscale PNG.
    with open('foo_gray.png', 'wb') as f:
        writer = png.Writer(width=z.shape[1], height=z.shape[0], bitdepth=16, greyscale=True)
        zgray2list = zgray.tolist()
        writer.write(f, zgray2list)


def main():
    # Make an image in a numpy array for this demonstration.
    nrows = 1024
    ncols = 1024
    np.random.seed(12345)
    # x = simplexNoise.makeNoise(nrows,ncols)
    x = np.random.randn(nrows, ncols, 3)

    # y is our floating point demonstration data.
    # y = gaussian_filter(x, (16, 16, 0))

    # Convert y to 16 bit unsigned integers.
    # z = (65535*((y - y.min())/y.ptp())).astype(np.uint16)
    z = (65535*((x - x.min())/x.ptp())).astype(np.uint16)

    get_colorscale(z)
    get_grayscale(z)


if __name__ == "__main__":
    main()    

# reference : https://stackoverflow.com/questions/25696615/can-i-save-a-numpy-array-as-a-16-bit-image-using-normal-enthought-python/25814423#25814423