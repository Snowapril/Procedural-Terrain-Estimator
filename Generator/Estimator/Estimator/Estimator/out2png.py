import png
import numpy as np

a = np.array((600,600,3))

with open("outHeight.txt","r") as f:
    lines = f.read()
    for line in lines:
        elements = line.split(" ")
        