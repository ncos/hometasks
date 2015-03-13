#!/usr/bin/env python
# -*- coding: utf-8 -*-

import socket, sys, pickle, json
import numpy as np

HEARTBEAT_PORT = 8881
CONNECTION_PORT = 8882
RECV_BUFFER = 8192


def worker(task):
    mat = task[0]
    id  = task[1]
    multiplier = mat[0, id]
    submatrix = np.delete(np.delete(mat, (0), axis=0), (id), axis=1)
    return multiplier*np.linalg.det(submatrix)



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


def gen_matrix_list(extended_matrix):
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

    return mlist


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
    return get_a_terminated(sock)




if __name__ == "__main__":
    while True:
        server_socket = establish_connection()
        
        str_matrix = get_matrix(server_socket)
        if not str_matrix:
            print 'Connection lost. Restarting.'
            continue
        
        
        print "Received %d bytes of matrix" % len(str_matrix)
        
        print "Generating task list..."
        mlist = gen_matrix_list(np.matrix(str_matrix))

        task_list = []
        for a in mlist:
            task_list = task_list + [[a, value] for value in xrange (a.shape[0])]
        print 'Done. The number of tasks is ' + str(len(task_list))
        
        
        
        while True:
            try:
                req_new_task(server_socket, 500)
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
                    
                    for id in task_id_list:
                        answer_list.append([worker(task_list[id]), id])
                       
                    print "Sending answer (%d bytes)" % server_socket.send(json.dumps(answer_list) + 'a')
                    #print "Sending answer (%d bytes)" % server_socket.send(json.dumps([answer_list[0]]))
                except:
                    print 'Internal exception/Connection lost. Restarting.'
                    server_socket.close()
                    break



