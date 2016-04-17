#!/usr/bin/python

import matplotlib.pyplot as plt

f = open('res2.csv', 'r')
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
    if 'a' in line:
        if ('a' in current_name):
            graphs.append([current_name, current_x, current_y])
        current_name = line
        current_x = []
        current_y = []
        continue
    xy = line.split("; ")

    if float(xy[0]) > 0.98:
        current_x.append(float(xy[0]))    
        current_y.append(float(xy[1]))    

graphs.append([current_name, current_x, current_y])



print "Haz " + str(len(graphs)) + " graphs\n"



color = ['r', 'b', 'g', 'k', 'm']



for i, graph in enumerate(graphs):
    plt.plot(graph[1], graph[2], color=color[i%5], label=graph[0])
plt.legend(loc=2)

#plt.axis([0, 6, 0, 20])
plt.show()
