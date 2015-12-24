#!/usr/bin/python

import matplotlib.pyplot as plt
from math import *
from scipy import special
from optparse import OptionParser

def psi(n, x):
    a = sqrt(2.0 * n + 1.0)
    A = sqrt(2.0 / pi)
    g = x/a
    p = sqrt(fabs(2.0 * n + 1.0 - x**2))

    if (x == a) or (x == -a):
        return 0

    if (x < -a):
        z1 = (n + 0.5) * (-g * sqrt(g**2 - 1.0) - log(- g + sqrt(g**2 - 1.0)))
        return A * pow(-1, n) * exp(-z1) / (2 * sqrt(p))

    if (x > a):
        z4 = (n + 0.5) * ( g * sqrt(g**2 - 1.0) - log(  g + sqrt(g**2 - 1.0)))
        return A * exp(-z4) / (2 * sqrt(p))

    z2 = (n + 0.5) * (-g * sqrt(1.0 - g**2) + pi/2.0 - asin(g))
    return A * sin(z2 + pi/4.0) / sqrt(p)

def psi_ref(n, x, f):
    return f(x)*exp(-x**2/2.0)/sqrt(factorial(n)*pow(2.0, n)*sqrt(pi))

def do_graph(n):
    a = sqrt(2.0 * n + 1.0)
    label = "n = " + str(n) + "\na=sqrt(" + str(2*n+1) + ")"
    x_min = -a * 2.0
    x_max =  a * 2.0
    dx = 0.001

    x = x_min
    X = []
    Y = []
    while (x <= x_max):
        y = psi(n, x)
        #if y > 3: 
        #    x += dx
        #    continue
        X.append(x)
        Y.append(y)
        x += dx
        
    return [label, X, Y]

def do_graph_ref(n):
    a = sqrt(2.0 * n + 1.0)
    label = "n = " + str(n) + "\na=sqrt(" + str(2*n+1) + ")"
    x_min = -a * 2.0
    x_max =  a * 2.0
    dx = 0.001
    f = special.hermite(n) 

    x = x_min
    X = []
    Y = []
    while (x <= x_max):
        y = psi_ref(n, x, f)
        #if y > 3: 
        #    x += dx
        #    continue
        X.append(x)
        Y.append(y)
        x += dx
        
    return [label, X, Y]



color = ['r', 'b', 'g', 'k', 'm']

def getticks(X, n, a):
    r = len(X)/n
    ret = []
    for i in range(0, len(X), r):
        ret.append(X[i])
    #ret.append(a)
    #ret.append(-a)
    return ret

def getticks2(a, dx):
    x = 0
    ret = []
    while x < 2.0*a:
        ret.append( x)
        ret.append(-x)
        x += dx
    return ret

def plot(n):
    a = sqrt(2.0 * n + 1.0)
    graph = do_graph(n)
    plt.plot(graph[1], graph[2], color='r', label=graph[0])
    graph = do_graph_ref(n)
    plt.plot(graph[1], graph[2], 'k--')
    plt.legend(loc=2)
    plt.axis([-2.0*a, 2.0*a, -3, 3])

    ymin = 0.5
    ymax = 1.0
    if (n%2 != 0):
        ymin = 0.0
        ymax = 0.5

    plt.axvline(x=-a, ymin = ymin, ymax = ymax, linewidth=1, color='k', ls='dashed', label="sdfgsfgsfg")
    plt.axvline(x= a, ymin = 0.5, ymax = 1, linewidth=1, color='k', ls='dashed', label=str(-a))
    plt.axvline(x= 0, linewidth=1, color='k')
    plt.axhline(y= 0, linewidth=1, color='k')
    #plt.xticks(getticks(graph[1], 70, a), rotation='vertical')
    plt.xticks(getticks2(a, 0.1), rotation='vertical')
    plt.yticks(getticks2(1.5, 0.5))
    #plt.xaxis.tick_top()
    plt.grid(True, which='major', axis='both')
    plt.show()

parser = OptionParser()
(options, args) = parser.parse_args()
n = int(args[0])

plot(n)
