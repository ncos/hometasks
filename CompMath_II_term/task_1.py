#!/usr/bin/python
# y'' - 2y'-y = -2xe^x, 0 <= x <= 1, y'(0)-y(0)=1, y(1)=e(2cosh(sqrt(2)+1)


import numpy as np 
 
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

N = 5
a = 0.0
b = 1.0
h = (b-a)/float(N)
ya = 1.0 

X = [a+i*h for i in xrange(N+1)]


print ("N = "+str(N)+"; solving on ("+str(a)+", "+str(b)+"); h = "+str(h))
print X

def q(x):
    return 1.0

def p(x):
    return x

def f(x):
    return 0.0

b = [-2.0-h**2*q(X[i]) for i in range(1, N, 1)]
c = [1.0+p(X[i])*h/2.0 for i in range(1, N-1, 1)]
a = [0.0] + [1.0-p(X[i])*h/2.0 for i in range(2, N, 1)]
f = [h**2*f(X[i]) for i in range(1, N, 1)]
f[0] = f[0] - (1.0-p(X[1])*h/2.0)*ya

a += [-1.0/h]
b += [2.0+1.0/h]
c += [1.0+p(X[N-1])*h/2.0] + [0.0]
f += [0.0]



print b
print a
print c
print f

y = TDMA(a, b, c, f)
print "y=", y 

