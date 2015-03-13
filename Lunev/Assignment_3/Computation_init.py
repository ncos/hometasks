import sys, time, random, json
#import argparse
import numpy as np
from Server import Server

def gen_matrix_(file_path, size):
    try:
        f = open(file_path, 'w')
    except IOError:
        print "Could not open file:", file_path
        exit(0)
        
    f.write("[")
    f.write(str(size)+",\n[\n")

    for i in xrange(size):
        for j in xrange(size + 1):
            f.write(str(random.uniform(-0.1, 0.1)))
            if not j == size:
                f.write(",")
        
        if not i == size - 1:
            f.write(",\n")
        else:
            f.write("\n")
    f.write("]]")
    f.close()



def gen_matrix(file_path, size):
    try:
        f = open(file_path, 'w')
    except IOError:
        print "Could not open file:", file_path
        exit(0)
        
    f.write(str(size)+",\n")

    for i in xrange(size):
        for j in xrange(size + 1):
            f.write(str(random.uniform(-0.1, 0.1)))
            if not j == size:
                f.write(",")
        
        if not i == size - 1:
            f.write(",\n")
        else:
            f.write("\n")
    f.close()




def read_matrix(file_path):
    tstart = time.time()
    try:
        f = open(file_path, 'r')
    except IOError:
        print "Could not open file:", file_path
        exit(0)
        
    content = f.read().split(',')
    f.close()
    mat = []
    
    print "Split time: ", time.time() - tstart
    #print content
    
    for elem in content:
        mat.append(float(elem))

    print "Overall read_matrix time: ", time.time() - tstart
    
    #print mat
    return [int(mat[0]), mat[1:]]





if __name__ == '__main__':
    tstart = time.time()

    #parser = argparse.ArgumentParser(description='Solve linear equation system')
    #parser = argparse.ArgumentParser()
    #parser.add_argument("-f", "--file",    help="the name of the file")
    #parser.add_argument("-r", "--random",  help="generate random matrix and exit", type = int)
    #args = parser.parse_args()
        
    #if (args.random > 0):
    #    if (args.random < 2):
    #        print "Invalid number of variables: ", args.random
    #        sys.exit(0)

    #    gen_matrix("__random.dat", args.random)
    #    sys.exit(0)

    #if (args.file == None):
    #    print "You must specify the location of your linear equation system description file"
    #    sys.exit(0)

    if len(sys.argv) == 3:
        gen_matrix("__random.dat", int(sys.argv[2]))
        sys.exit(0)

    if len(sys.argv) < 2:
        sys.exit('Usage: %s file-name' % sys.argv[0])


    matrix = read_matrix(sys.argv[1])
    matrix_size = matrix[0]

    tstart_l = time.time()
    with Server() as server_obj:
        server_result = server_obj.compute(matrix[1], matrix_size)
    tstop_l = time.time()
    print "Computation time: ", tstop_l - tstart_l


    if(server_result[0] == 0):
        print "This matrix rank is insufficient"
    else:
        for i in xrange(matrix_size):
            print "x%d = %5f" % (i, server_result[i+1]/server_result[0])
            pass

    tstop =  time.time()
    print "Total elapsed:", tstop - tstart

