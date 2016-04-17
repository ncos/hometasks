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
    xy = line.split("; ")
    current_x.append(float(xy[0]))    
    current_y.append(float(xy[1]))    


plt.plot(current_x, current_y, 'ro')
#plt.axis([0, 6, 0, 20])
plt.show()
