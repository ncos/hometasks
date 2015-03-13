#!/usr/bin/env python
# -*- coding: utf-8 -*-

import socket, sys, thread, select, json, time, struct
from time import sleep

HEARTBEAT_PORT = 8881
CONNECTION_PORT = 8882
RECV_BUFFER = 8192

class Heartbeat:
    def __init__(self):
        self.heartbeat_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.heartbeat_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    def __enter__(self):
        return self
    
    def __exit__(self, type, value, traceback):
        print "Server: Closing heartbeat socket..."
        self.heartbeat_socket.close()
        
        
    def start_sending(self):
        while True:
            try:
                self.heartbeat_socket.sendto("SIA", ('<broadcast>', HEARTBEAT_PORT))
            except:
                return
            sleep(0.5)

    def start(self):
        return thread.start_new_thread(self.start_sending, ())
    

 


class Server:
    def __init__(self):
        self.heartbeat_obj = Heartbeat()
        self.heartbeat_obj.start()
        # List to keep track of socket descriptors
        self.CONNECTION_LIST = []
        self.NAME_LIST = []
         
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server_socket.bind(("0.0.0.0", CONNECTION_PORT))
        self.server_socket.listen(10)
     
        # Add server socket to the list of readable connections
        self.CONNECTION_LIST.append(self.server_socket)
        self.NAME_LIST.append("myself")
        
        self.tasks_at_computation_map = {}
        self.tasks_at_computation = 0
        self.task_list_wid = []
        print "Computation server started on port " + str(CONNECTION_PORT)

    def __enter__(self):
        return self
    
    def __exit__(self, type, value, traceback):
        print "Server: Shutting down"
        self.heartbeat_obj.heartbeat_socket.close()
        self.server_socket.close()

    def close_connection(self, sock):
        sock_num = self.CONNECTION_LIST.index(sock)
        client_id = self.NAME_LIST.pop(sock_num)
        print "Client (%s) went offline" % client_id

        undone_tasks = self.tasks_at_computation_map[client_id]
        self.tasks_at_computation = self.tasks_at_computation - len(undone_tasks)
        self.task_list_wid.extend(undone_tasks)
        del self.tasks_at_computation_map[client_id]

        sock.close()
        self.CONNECTION_LIST.remove(sock)        


    def get_a_terminated(self, sock):
        buf = ""
        data  = ""
        
        while not buf[-1:] == 'a': 
            try:
                buf = sock.recv(RECV_BUFFER)
            except:
                return 0
            if not buf:
                return 0
            data = data + buf
        return data[:-1]


    def send_matrix(self, sock, mat_arr, mat_size):
        try:            
            sock.send(str(mat_size) + 'a')
            
            buf = struct.pack('%sf' % len(mat_arr), *mat_arr)
            #print buf
            
            sent = sock.send(buf)
            print "Sent %d bytes of the matrix from %d" % (sent, len(buf))
        except:
            print "Send matrix fail"
            self.close_connection(sock)


    def compute(self, mat_arr, matrix_size):
        server_result = []
        server_result.extend(xrange(matrix_size + 1))
        self.task_list_wid.extend(xrange(matrix_size + 1))

        while len(self.task_list_wid) + self.tasks_at_computation > 0:            
            read_sockets, write_sockets, error_sockets = select.select(self.CONNECTION_LIST, [], [])
     
            for sock in read_sockets:
                if sock == self.server_socket: # Than we got a new connection
                    sockfd, addr = self.server_socket.accept()
                    self.CONNECTION_LIST.append(sockfd)
                    self.NAME_LIST.append(addr[0])
                    self.tasks_at_computation_map[addr[0]] = []
                    print "Client (%s, %s) connected" % addr
                    
                    #tstart_l = time.time()
                    self.send_matrix(sockfd, mat_arr, matrix_size)
                    #tstop_l = time.time()
                    #print 'Matrix send time to (%s): %d s.' % (addr[0], tstop_l - tstart_l)
                    break
                else:
                    try:
                        #tstart_l = time.time()
                        data = self.get_a_terminated(sock)
                        #tstop_l = time.time()
                        #print 'Data receive time from (%s, %d bytes): %d s.' % (sock.getpeername()[0], len(data), tstop_l - tstart_l)
                        
                        if not data:
                            print "Not data"
                            self.close_connection(sock)
                            print 'Tasks remain: ' + str(len(self.task_list_wid))
                            break
                        
                        answer = json.loads(data)
                        if answer[0] == "MRQ": # New task is being requested
                            if len(self.task_list_wid) == 0:
                                continue
                            
                            n_of_tasks = answer[1]
                            #n_of_tasks = len(self.task_list_wid)/(len(self.CONNECTION_LIST) - 1)
                            #print 'Sending %d tasks to %s' % (n_of_tasks, sock.getpeername()[0])
                            task_id_list = []
                            
                            for i in xrange(n_of_tasks):
                                if len(self.task_list_wid) == 0:
                                    break;
                                task_id = self.task_list_wid.pop()
                                task_id_list.append(task_id)
                                self.tasks_at_computation_map[str(sock.getpeername()[0])].append(task_id)
                                self.tasks_at_computation = self.tasks_at_computation + 1
                            
                            #tstart_l = time.time()
                            tmp = sock.send(json.dumps(task_id_list) + 'a')
                            #tstop_l = time.time()
                            #print 'Task send time to (%s, %d bytes): %d s.' % (sock.getpeername()[0], tmp, tstop_l - tstart_l)
                            print 'Sending %d tasks to %s. Tasks remain: %d' % (n_of_tasks, sock.getpeername()[0], len(self.task_list_wid))
                        else:
                            for answer_pair in answer:
                                self.tasks_at_computation_map[str(sock.getpeername()[0])].remove(answer_pair[1])
                                self.tasks_at_computation = self.tasks_at_computation - 1
                                server_result[answer_pair[1]] = answer_pair[0]
                    
                    except:
                        print "Unexpected error:", sys.exc_info(), " "
                        self.close_connection(sock)
                        break

        return server_result





