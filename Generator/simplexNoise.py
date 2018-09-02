import numpy as np

def makeNoise(_width, _height):
    # https://github.com/bradykieffer/SimplexNoise
    ret = np.zeros((_width, _height, 3))
    print(ret)
    return ret

def main():
    x = makeNoise(3,5)
    print(x)

if __name__ == "__main__":
    main()