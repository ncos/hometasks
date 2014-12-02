import matplotlib.pyplot as plt
from math import *

tn = [[1.04,  0.866],
      [6.80,  0.500],
      [7.33,  0.866],
      [7.85,  1.000],
      [13.61, 0.866],
      [14.14, 1.000],
      [19.37, 0.500],
      [20.42, 1.000],
      [25.92, 0.707]]

koefficient = 10

x_values = []
for i in xrange(len(tn) - 1):
    step = (tn[i + 1][0] - tn[i][0])/koefficient
    for j in xrange(koefficient):
        x_values.append(tn[i][0] + j*step)
    x_values.append(tn[len(tn)-1][0])

############################## NEWTON
def contdelta(k):
    summ = 0.0
    for j in xrange(k):
        divider = 1.0
        for r in xrange(k):
            if (r != j):
                divider = divider*(tn[j][0] - tn[r][0])
        summ += tn[j][1]/divider
    return summ

delta = []
for i in xrange(len(tn)):
    delta.append(contdelta(i + 1))

N = []
def dxx(x, n):
    d = 1.0
    for i in xrange(n):
        d = d*(x - tn[i][0])
    return d

def newton_f(x):
    N = 0.0
    for i in xrange(len(tn)):
        N += dxx(x, i)*delta[i]
    return N


################################ LAGRANGE
def l(x, i):
    mull = 1.0
    for j in xrange(len(tn)):
        if (j != i):
            mull = mull * (x - tn[j][0]) / (tn[i][0] - tn[j][0])
    return mull

def lagrange_f(x):
    L = 0.0
    for i in xrange(len(tn)):
        L += l(x, i) * tn[i][1]
    return L

################################ CUBIC

class spline_tuple:
    a = 0.0
    b = 0.0
    c = 0.0
    d = 0.0
    x = 0.0

splines = []
def build_spline():
    for i in xrange(len(tn)):
        sp = spline_tuple()
        sp.x = tn[i][0]
        sp.y = tn[i][1]
        sp.a = tn[i][1]
        splines.append(sp)
    splines[0].c = 0.0

    alpha = [0.0 for i in xrange(len(tn) - 1)]
    beta  = [0.0 for i in xrange(len(tn) - 1)]

    h_i = 0.0
    h_i1 = 0.0
    A = 0.0
    B = 0.0
    C = 0.0
    F = 0.0
    z = 0.0

    for i in range(1, len(tn) - 1, 1):
        h_i = splines[i].x - splines[i - 1].x
        h_i1 = splines[i + 1].x - splines[i].x
        A = h_i
        C = 2.0 * (h_i + h_i1)
        B = h_i1
        F = 6.0 * ((splines[i + 1].y - splines[i].y) / h_i1 - (splines[i].y - splines[i - 1].y) / h_i)
        z = (A * alpha[i - 1] + C)
        alpha[i] = -B / z
        beta[i] = (F - A * beta[i - 1]) / z

    splines[len(tn) - 1].c = (F - A * beta[len(tn) - 2]) / (C + A * alpha[len(tn) - 2])
 
    for i in range(len(tn) - 2, 0, -1):
        splines[i].c = alpha[i] * splines[i + 1].c + beta[i]
 
    for i in range(len(tn) - 1, 0, -1):
        h_i = splines[i].x - splines[i - 1].x
        splines[i].d = (splines[i].c - splines[i - 1].c) / h_i
        splines[i].b = h_i * (2.0 * splines[i].c + splines[i - 1].c) / 6.0 + (splines[i].y - splines[i - 1].y) / h_i

build_spline()

def cubic_f(x):
    s = spline_tuple()
    if (x <= splines[0].x):
        s = splines[0]

    if (x >= splines[len(tn) - 1].x):
        s = splines[len(tn) - 1]

    if (x > splines[0].x and x < splines[len(tn) - 1].x):
        i = 0
        j = len(tn) - 1
        while (i + 1 < j):
            k = i + (j - i) / 2
            if (x <= splines[k].x):
                j = k
            else:
                i = k
        s = splines[j]
 
    dx = (x - s.x);
    return s.a + (s.b + (s.c / 2.0 + s.d * dx / 6.0) * dx) * dx



################################ PLOTTING
plt.plot(zip(*tn)[0], zip(*tn)[1], 'o')
#plt.plot(x_values, [newton_f(x) for x in x_values], 'x')
#plt.plot(x_values, [lagrange_f(x) for x in x_values], 'go')
plt.plot(x_values, [cubic_f(x) for x in x_values], 'ro')
plt.show()
