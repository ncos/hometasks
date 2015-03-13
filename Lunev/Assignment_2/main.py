from multiprocessing import Pool
import argparse
import time
import numpy as np
import sys




def worker(task):
    mat = task[0]
    id  = task[1]
    multiplier = mat[0, id]
    submatrix = np.delete(np.delete(mat, (0), axis=0), (id), axis=1)
    return multiplier*np.linalg.det(submatrix)


def read_matrix(file_path):
    tstart = time.time()
    try:
        f = open(args.file, 'r')
    except IOError:
        print "Could not open file:", args.file
        exit(0)
	
    lines = [line.strip() for line in f]
    f.close()

    str_mat=""
    for line in lines:
        str_mat = str_mat + line + ';'
    str_mat = str_mat[:-1]

    tstop =  time.time()
    print "read_matrix time: ", tstop - tstart
    return np.matrix(str_mat)


def gen_matrix_list(extended_matrix):
    tstart = time.time()
    print "Matrix shape is:", extended_matrix.shape
    if( extended_matrix.shape[0] != extended_matrix.shape[1] - 1):
        print "Invalid matrix!"
        sys.exit(0)
    mlist = []
    
    A = np.delete(extended_matrix, (extended_matrix.shape[0]), axis=1)
    b = np.swapaxes(extended_matrix[:,extended_matrix.shape[0]], 0, 1)
    print A

    mlist.append(A)
    for i in xrange(A.shape[0]):
        mlist.append( np.insert(np.delete(A, i, axis=1), i, b, axis=1)  )

    tstop =  time.time()
    print "gen_matrix_list time: ", tstop - tstart
    return mlist

if __name__ == '__main__':
    tstart = time.time()

    parser = argparse.ArgumentParser(description='Solv linear equation system using threads')
    parser = argparse.ArgumentParser()
    parser.add_argument("-t", "--threads", help="the number of threads", type= int)
    parser.add_argument("-f", "--file",    help="the name of the file")
    args = parser.parse_args()
        
    
    if (args.threads <= 0 or args.threads > 100):
        print "Invalid number of threads: ", args.threads
        sys.exit(0)
        
    if (args.file == None):
        print "You must specify the location of your linear equation system description file"
        sys.exit(0)

    pool = Pool(processes=args.threads)             

    tstop =  time.time()
    print "Argument parsing and init time:", tstop - tstart

    extended_matrix = read_matrix(args.file)
    mlist = gen_matrix_list(extended_matrix)

    
    task_list = []
    for a in mlist:
        task_list = task_list + [[a, value] for value in xrange (a.shape[0])]

    tstart_l = time.time()
    thread_result = pool.map(worker, task_list)
    tstop_l = time.time()
    print "Thread computation time: ", tstop_l - tstart_l

    matrix_size = mlist[0].shape[0]
    determinants = []
    for i in xrange(matrix_size + 1):
        #print thread_result[i*matrix_size: (i+1)*matrix_size], sum(thread_result[i*matrix_size: (i+1)*matrix_size])
        determinants.append(sum(thread_result[i*matrix_size: (i+1)*matrix_size]))

    if(determinants[0] == 0):
        print "This matrix rank is too small to solve"
    else:
        for i in xrange(matrix_size):
            print "x%d = %5f" % (i, determinants[i+1]/determinants[0])

    tstop =  time.time()
    print "Total elapsed:", tstop - tstart

    #print result
