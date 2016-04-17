#!/usr/bin/python

import numpy as np 
import matplotlib.pyplot as plt
from math import *

## Various Helper Function to Calculate Error
def list_diff(l1, l2):
    '''
    Returns l1 - l2. [1, 2, 3] - [3, 4, 5] = [-2, -2, -2]
    '''
    return [l1[i] - l2[i] for i in xrange(len(l1))] 

def list_norm(l):
    s = 0.0
    for a in l:
        s+= a**2
    return sqrt(s)

## Tri Diagonal Matrix Algorithm(a.k.a Thomas algorithm) solver
def TDMA(a, b, c, d):
    '''
    TDMA solver, a b c d can be NumPy array type or Python list type.
    refer to http://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm
    '''
    nf = len(a)     # number of equations
    ac, bc, cc, dc = map(np.array, (a, b, c, d))     # copy the array
    for it in xrange(1, nf):
        mc = ac[it]/bc[it-1]
        bc[it] = bc[it] - mc*cc[it-1] 
        dc[it] = dc[it] - mc*dc[it-1]
 
    xc = ac
    xc[-1] = dc[-1]/bc[-1]
 
    for il in xrange(nf-2, -1, -1):
        xc[il] = (dc[il]-cc[il]*xc[il+1])/bc[il]
 
    del bc, cc, dc  # delete variables from memory
    return xc 

'''
# Example:
M = [[1,2,0,0],
     [5,3,2,0],
     [0,9,3,5],
     [0,0,3,5]]

a = [0.0, 5.0, 9.0, 3.0]
c = [2.0, 2.0, 5.0, 0.0]
b = [1.0, 3.0, 3.0, 5.0]
f = [1.0, 2.0, 3.0, 4.0]

x = TDMA(a, b, c, f)
print x
'''

# Start
# y'' - 2y'-y = -2xe^x, 0 <= x <= 1, y'(0)-y(0)=1, y(1)=e(2cosh(sqrt(2)+1)
N = 50000
a = 0.0
b = 1.0
h = (b-a)/float(N)
yb = e*(2.0*cosh(sqrt(2.0)) + 1.0) # Left side type (fixed on right)
X = [a+i*h for i in xrange(N+1)]

# Ideal solution (got from Wolfram)
def _f(x):
    return exp(-sqrt(2.0)*x)*(x*exp(x*(1.0+sqrt(2.0)))+exp(x)+exp(2.0*sqrt(2.0)*x+x)) 

print ("N = "+str(N)+"; solving on ("+str(a)+", "+str(b)+"); h = "+str(h))

# y'' + py' + qy = f
def q(x):
    return -1.0

def p(x):
    return -2.0

def f(x):
    return -2.0*x*exp(x)

b = [-2.0+h**2*q(X[i]) for i in range(1, N, 1)]
c = [1.0+p(X[i])*h/2.0 for i in range(1, N-1, 1)]
a = [1.0-p(X[i])*h/2.0 for i in range(2, N, 1)]
f = [h**2*f(X[i]) for i in range(1, N, 1)]
#f[0] = f[0] - (1.0-p(X[1])*h/2.0)*ya # Uncomment if have left value
f[N-2] = f[N-2] - (1.0+p(X[N-1])*h/2.0)*yb # Uncomment if have right value

# Right side
#a += []
#b += []
#c += []
#f += []

# Left side
# y'(0)-y(0)=1 --> (Y[1]-Y[0])/h-Y[0]=1
a = [1.0-p(X[1])*h/2.0] + a
b = [-(1+1/h)] + b
c = [1/h] + c
f = [1.0] + f

# Bring a, b, c and f to the same size
a = [0.0] + a
c += [0.0]

# Calculate/plot ideal and approximated functions
Y = TDMA(a, b, c, f).tolist() + [yb]
Y_ = [_f(x) for x in X]
#print "y  =", Y
#print "y_ =", Y_
print "Error: ", list_norm(list_diff(Y, Y_))

plt.plot(X, Y, 'ro')
plt.plot(X, Y_, 'g')
plt.show()
