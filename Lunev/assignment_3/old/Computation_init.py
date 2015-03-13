from multiprocessing import Pool
import sys, time, random, argparse
import numpy as np
from Server import Server

def gen_matrix(file_path, size):
    try:
        f = open(file_path, 'w')
    except IOError:
        print "Could not open file:", file_path
        exit(0)

    for i in xrange(size):
        for j in xrange(size + 1):
            f.write(str(random.uniform(-10, 10)) + ' ')
        f.write("\n")
    f.close()


def read_matrix(file_path):
    tstart = time.time()
    try:
        f = open(file_path, 'r')
    except IOError:
        print "Could not open file:", file_path
        exit(0)
	
    lines = [line.strip() for line in f]
    f.close()

    str_mat=""
    for line in lines:
        str_mat = str_mat + line + ';'
    str_mat = str_mat[:-1]

    tstop =  time.time()
    print "read_matrix time: ", tstop - tstart
    return str_mat


def gen_matrix_list(extended_matrix):
    tstart = time.time()
    print "Matrix shape is:", extended_matrix.shape
    if( extended_matrix.shape[0] != extended_matrix.shape[1] - 1):
        print "Invalid matrix!"
        sys.exit(0)
    mlist = []
    
    A = np.delete(extended_matrix, (extended_matrix.shape[0]), axis=1)
    b = np.swapaxes(extended_matrix[:,extended_matrix.shape[0]], 0, 1)
    #print A

    mlist.append(A)
    for i in xrange(A.shape[0]):
        mlist.append( np.insert(np.delete(A, i, axis=1), i, b, axis=1)  )

    tstop =  time.time()
    print "gen_matrix_list time: ", tstop - tstart
    return mlist




if __name__ == '__main__':
    tstart = time.time()

    parser = argparse.ArgumentParser(description='Solve linear equation system')
    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--file",    help="the name of the file")
    parser.add_argument("-r", "--random",  help="generate random matrix and exit", type = int)
    args = parser.parse_args()
        
    if (args.random > 0):
        if (args.random < 2):
            print "Invalid number of variables: ", args.random
            sys.exit(0)

        gen_matrix("__random.dat", args.random)
        sys.exit(0)

    if (args.file == None):
        print "You must specify the location of your linear equation system description file"
        sys.exit(0)

    str_matrix = read_matrix(args.file)


    matrix_size = np.matrix(str_matrix).shape[0]
    print matrix_size


    tstart_l = time.time()
    with Server() as server_obj:
        server_result = server_obj.compute(str_matrix, matrix_size)
    tstop_l = time.time()
    print "Computation time: ", tstop_l - tstart_l

    determinants = []
    for i in xrange(matrix_size + 1):
        determinants.append(sum(server_result[i*matrix_size: (i+1)*matrix_size]))

    if(determinants[0] == 0):
        print "This matrix rank is insufficient"
    else:
        for i in xrange(matrix_size):
            print "x%d = %5f" % (i, determinants[i+1]/determinants[0])

    tstop =  time.time()
    print "Total elapsed:", tstop - tstart

