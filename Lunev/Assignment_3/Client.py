#!/usr/bin/env python
# -*- coding: utf-8 -*-

from multiprocessing import Pool
import socket, sys, pickle, json, time, struct
#import argparse
import numpy as np

HEARTBEAT_PORT = 8881
CONNECTION_PORT = 8882
RECV_BUFFER = 8192






class Connection:
    def __init__(self):
        self.heartbeat_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.heartbeat_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        self.heartbeat_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.heartbeat_socket.bind(('', HEARTBEAT_PORT))

    def __enter__(self):
        return self
    
    def __exit__(self, type, value, traceback):
        self.heartbeat_socket.close()
        
    def wait_for_connection(self):
        print "Client: Waiting for connection..."
        message = ''
        while (not message == "SIA"):
            message, address =  self.heartbeat_socket.recvfrom(RECV_BUFFER)
        return address[0]


def establish_connection():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    succeed = False
    while (not succeed):
        with Connection() as connection_obj:
            SERVER_ADDR = connection_obj.wait_for_connection()
            print 'Found server at ' + SERVER_ADDR

            try :
                server_socket.connect((SERVER_ADDR, CONNECTION_PORT))
                succeed = True
            except :
                print 'Failed to connect to ' + SERVER_ADDR + ' at port ' + str(CONNECTION_PORT)
                succeed = False

    print 'Connected to a remote server (%s). Start receiving data' % SERVER_ADDR
    return server_socket


def req_new_task(server_socket, threads):
    server_socket.send(json.dumps(["MRQ", threads]) + 'a')



def get_a_terminated(sock):
    buf = ""
    data  = ""
    
    while not buf[-1:] == 'a': 
        try:
            buf = sock.recv(RECV_BUFFER)
        except:
            sock.close()
            return 0
        if not buf:
            sock.close()
            return 0
        data = data + buf
    return data[:-1]
   


def get_matrix(sock):
    print "Requesting matrix..."
    
    buf = ""
    data  = ""
    while not buf == 'a': 
        try:
            buf = sock.recv(1)
        except:
            sock.close()
            return 0
        if not buf:
            sock.close()
            return 0
        data = data + buf
    
    mat_size = int(data[:-1])
    print 'Read matrix size is:', mat_size
    
    bytes_to_read = 4 * mat_size * (mat_size + 1)
    
    data = ""
    bytes_read = 0
    while not bytes_read == bytes_to_read: 
        try:
            buf = sock.recv(RECV_BUFFER)
        except:
            sock.close()
            return 0
        if not buf:
            sock.close()
            return 0
        data = data + buf
        bytes_read = bytes_read + len(buf)
        
        
    mat_raw = struct.unpack('%sf' % (mat_size * (mat_size + 1)), data)
    
    mat = []
    for i in xrange(mat_size):
        str = []
        for j in xrange(mat_size + 1):
            str.append(mat_raw[j + i * (mat_size + 1)])
        mat.append(str)
        
    return mat






class TaskGenerator:
    def __init__(self, extended_matrix_):
        self.extended_matrix = extended_matrix_
        self.A = np.delete(self.extended_matrix, (self.extended_matrix.shape[0]), axis=1)
        self.b = np.swapaxes(self.extended_matrix[:,self.extended_matrix.shape[0]], 0, 1)
        print 'Done. The number of tasks is ' + str(extended_matrix.shape[1])

def worker(task):
    id = task[0] 
    A  = task[1]
    b  = task[2]
        
    if id == 0:
        return np.linalg.det(A)
        
    mat = np.insert(np.delete(A, id - 1, axis=1), id - 1, b, axis=1)
    return np.linalg.det(mat)




if __name__ == "__main__":
    #parser = argparse.ArgumentParser(description='Node for distributed linear system computation')
    #parser = argparse.ArgumentParser()
    #parser.add_argument("-t", "--threads", help="the number of threads", type= int)
    #args = parser.parse_args()
    
    if not len(sys.argv) == 2:
        sys.exit('Usage: %s number-of-threads' % sys.argv[0])
    
    if (int(sys.argv[1]) <= 0 or int(sys.argv[1]) > 100):
        print "Invalid number of threads: ", sys.argv[1]
        sys.exit(0)

    pool = Pool(processes = int(sys.argv[1]))  
    
    
    #tasks_to_request = 5 * args.threads
    tasks_to_request = 64
    
    while True:
        server_socket = establish_connection()
        
        tstart_g = time.time()
        mat_task = get_matrix(server_socket)
        if not mat_task:
            print 'Connection lost. Restarting.'
            continue
        
        print 'Received matrix. Time passed: %f s.' % (time.time() - tstart_g)
        
        print "Processing matrix..."
        
        tstart_l = time.time()
        extended_matrix = np.matrix(mat_task)
        print 'Generating matrix from string: %f s.' % (time.time() - tstart_l)
        
        print "Matrix shape is:", extended_matrix.shape
        if( extended_matrix.shape[0] != extended_matrix.shape[1] - 1):
            print "Invalid matrix!"
            server_socket.close()
            continue
        
        taskgen = TaskGenerator(extended_matrix)
        print 'Time passed: %f s.' % (time.time() - tstart_g)
        
        
        
        
        while True:
            try:
                req_new_task(server_socket, tasks_to_request)
            except:
                print 'Connection lost. Restarting.'
                server_socket.close()
                break
    

            data = get_a_terminated(server_socket)
            if not data:
                print 'Connection lost. Restarting.'
                break
            else:
                try:
                    task_id_list = json.loads(data)
                    answer_list = []
                    
                    
                    # ------------ Linear -----------------
                    #i = 0
                    #task_len = len(task_id_list)
                    #for id in task_id_list:
                    #    i = i + 1
                    #    #answer_list.append([worker(mlist[id]), id])
                    #    tstart_l = time.time()
                    #    #answer_list.append([worker(gen_task_data(A, b, id)), id])
                    #    answer_list.append([taskgen.worker(id), id])
                    #    tstop_l = time.time()
                    #    print 'Computation time of the task (%d/%d): %f s.' % (i, task_len, tstop_l - tstart_l)
                    #   
                    
                    # ------------ Threaded ----------------
                    tstart_l = time.time()
                    tasks = [[id, taskgen.A, taskgen.b] for id in task_id_list]                
                    thread_result = pool.map_async(worker, tasks).get(9999999)
                    answer_list = zip(thread_result, task_id_list)
                    print 'Computation time of %d tasks: %f s.' % (len(thread_result), time.time() - tstart_l)
                    server_socket.send(json.dumps(answer_list) + 'a')

                except (KeyboardInterrupt, SystemExit):
                    print 'Exiting...'
                    exit(0)
                    
                except:
                    print 'Internal exception/Connection lost. Restarting.'
                    server_socket.close()
                    break



