#!/usr/bin/python3

from numpy import *
import os
import sys
import math


# 
#   Solves this nonlienar system:
# 
#    +-          --+   +-- --+     +--     --+
#    |             |   |     |     |         |
#    |  3.    4.   |   | x0  |     | sin(x0) |
#    |             |   |     | =   |         |
#    |  1.    2.   |   | x1  |     | cos(x1) |
#    |             |   |     |     |         |
#    +-          --+   +-- --+     +--     --+
# 
# 

# --------------------------
# Evaluate nonlinear system
# --------------------------

def eval_f( A ,  x , f):

    f[0] = A[0][0] * x[0] + A[0][1] * x[1] - ( 1.)*math.sin(x[0])
    f[1] = A[1][0] * x[0] + A[1][1] * x[1] - ( 1.)*math.cos(x[1])
    
    return 

# --------------------------
# Update Jacobian
# --------------------------

def updateJacobian( A , x , J):

    J[0][0] = A[0][0] - ( 1.)*math.cos(x[0])
    J[0][1] = A[0][1] - 0.
    J[1][0] = A[1][0] - (-1.)*math.sin(x[1])
    J[1][1] = A[1][1] - 0.

    return 

# --------------------------
# Solve 2x2 system
# --------------------------

def solve2x2( J , b ):

    detA = J[0][0]*J[1][1] - J[0][1]*J[1][0]

    if abs(detA) < 0.00001:
        print("determinant is too small")
        print("J = ",J)
        exit(0)

    x0 = b[0]/detA
    x1 = b[1]/detA
    
    return array( [ x0 , x1] )


# ==
# ||
# || Main Program
# ||
# ==

def showPlot(argv):
    os.system('clear')

    # -------------------------------------------------
    # Set up Perspective Matrices
    # -------------------------------------------------

    A = array( [
        [ 30., 40.],
        [ 10., 20.] ] )
    
    x = array( [ .1 , .2] )
    f = array( [ 0., 0.] )
    
    J = array( [
        [ 0., 0.],
        [ 0., 0.] ] )
    
    # -------------------------------------------------
    # Newton-Raphson Iterations
    # -------------------------------------------------

    maxIter = 1000
    iter = 0
    tol = 0.0001
    r = 1.

    fFile = open('f.plt','w')
    iFile = open('iter.plt','w')

    converged = 0
    while converged == 0 and iter < maxIter:

        iter += 1

        updateJacobian(A,x,J)
        eval_f(A,x,f)
        delta = solve2x2(J,-f)

        x += r*delta
        print('iter ' ,iter, ' has x = ',x , ' and delta = ',delta,  ' f = ',f)

        if abs(delta[0]) < tol and abs(delta[1]) < tol:
            converged = 1
            print("Converged in " , iter , " iterations")
        else:
            converged = 0

        print(x[0] , ' ' , x[1] , ' ' ,f[0] , ' ' , f[1] , file = fFile)
        print(iter , ' ' , delta[0] , ' ' ,delta[1] , file = iFile)
            

    fFile.close()
    iFile.close()
    
    return



if __name__ == "__main__":
    showPlot(sys.argv[1:])

