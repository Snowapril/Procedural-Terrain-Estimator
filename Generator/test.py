import numpy as np

def change(x):
    x[0][0] = 1
    x[1][0] = 0
    x[0][1] = 0
    x[1][1] = 1

def main():
    Q = np.array([2,2])
    Q = [[0,1],[1,0]]
    print(Q)
    change(Q)
    print(Q)

main()
    