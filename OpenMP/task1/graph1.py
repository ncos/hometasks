#!/usr/bin/python

import matplotlib.pyplot as plt

f = open('res1.csv', 'r')
lines = f.readlines()
f.close()

graphs = []

current_x = []
current_y = []
current_name = ""

for line in lines:
    if len(line) < 3:
        continue
    if 'X' in line:
        continue
    if 'Time' in line:
        continue
    if 'b' in line:
        if ('b' in current_name):
            graphs.append([current_name, current_x, current_y])
        current_name = line
        current_x = []
        current_y = []
        continue
    xy = line.split("; ")
    current_x.append(float(xy[0]))    
    current_y.append(float(xy[1]))    


print "Haz " + str(len(graphs)) + " graphs\n"

for graph in graphs:
    plt.plot(graph[1], graph[2], 'ro')
#plt.axis([0, 6, 0, 20])
plt.show()
