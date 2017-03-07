#!/usr/bin/python


import cv2
import numpy as np
from optparse import OptionParser
from multiprocessing import Process, Array
from os.path import isfile, join
import sys, os, math
import dlib


# Color print
class bcolors:
    HEADER = '\033[95m'
    PLAIN = '\033[37m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def offset(str_, p_offset):
    for i in xrange(p_offset):
        str_ = '...' + str_
    return str_

def hdr(str_, p_offset=0):
    return offset(bcolors.HEADER + str_ + bcolors.ENDC, p_offset)

def wht(str_, p_offset=0):
    return offset(bcolors.PLAIN + str_ + bcolors.ENDC, p_offset)

def okb(str_, p_offset=0):
    return offset(bcolors.OKBLUE + str_ + bcolors.ENDC, p_offset)

def okg(str_, p_offset=0):
    return offset(bcolors.OKGREEN + str_ + bcolors.ENDC, p_offset)

def wrn(str_, p_offset=0):
    return offset(bcolors.WARNING + str_ + bcolors.ENDC, p_offset)

def err(str_, p_offset=0):
    return offset(bcolors.FAIL + str_ + bcolors.ENDC, p_offset)

def bld(str_, p_offset=0):
    return offset(bcolors.BOLD + str_ + bcolors.ENDC, p_offset)
    
colors = [(255,0,0),(0,255,0),(0,0,255),(127,127,0),(127,0,127),(0,127,127),(255,255,255),(255,255,0),
          (255,0,255),(0,255,255),(127,127,127)]


# Core
class Face:
    def __init__(self, image_, bbox_, predictor):
        self.image = image_
        self.dlibbbox = bbox_
        self.bbox = (bbox_.left(), bbox_.top(), bbox_.right(), bbox_.bottom())
        predicted = predictor(self.image, self.dlibbbox)
        self.landmarks = np.array([[p.x, p.y] for p in predicted.parts()])
        self.imh, self.imw, channels = self.image.shape


class Delaunay:
    @staticmethod
    def rectContains(rect, point):
        if point[0] < rect[0]:
            return False
        elif point[1] < rect[1]:
            return False
        elif point[0] > rect[0] + rect[2]:
            return False
        elif point[1] > rect[1] + rect[3]:
            return False
        return True
 

    @staticmethod
    def get(rect, points):
        subdiv = cv2.Subdiv2D(rect)

        for p in points:
            subdiv.insert((p[0], p[1]))

        triangleList = subdiv.getTriangleList()    
        delaunayTri = []
   
        pt = []       
        count= 0    
        for t in triangleList:        
            pt.append((t[0], t[1]))
            pt.append((t[2], t[3]))
            pt.append((t[4], t[5]))

            pt1 = (t[0], t[1])
            pt2 = (t[2], t[3])
            pt3 = (t[4], t[5])        
        
            if Delaunay.rectContains(rect, pt1) and Delaunay.rectContains(rect, pt2) and Delaunay.rectContains(rect, pt3):
                count = count + 1 
                ind = []
                for j in xrange(0, 3):
                    for k in xrange(0, len(points)):                    
                        if(abs(pt[j][0] - points[k][0]) < 1.0 and abs(pt[j][1] - points[k][1]) < 1.0):
                            ind.append(k)                            
                if len(ind) == 3:                                                
                    delaunayTri.append((ind[0], ind[1], ind[2]))
        
            pt = []        
             
        return delaunayTri
       
# Multithreading workaround
def Uvec(p0, p_vec):
    diff = p_vec - p0
    r = np.einsum('ij,ij->i', diff, diff)
    return r * np.ma.log(r).filled(0)

def tps(p, a0, ax, ay, w, landmarks):
    ret = a0 + ax * p[0] + ay * p[1]
    ret += np.sum(np.multiply(w.transpose(), np.array(Uvec(p, landmarks))))
    return ret

def imtransfer(f_img, src_img, params_x, params_y, landmarks, lu0, rd0, lu1, rd1):
    src_imh, src_imw, channels = src_img.shape
    ax = params_x[-3:]
    wx = params_x[:-3]
    ay = params_y[-3:]
    wy = params_y[:-3]
 
    for i in range(lu0, rd0 + 1, 1):
        for j in range(lu1, rd1 + 1, 1):      
            p = np.array([i, j])
            x = tps(p, ax[2], ax[0], ax[1], wx, landmarks)
            y = tps(p, ay[2], ay[0], ay[1], wy, landmarks)
            if ((y < src_imh) and (x < src_imw)):
                f_img[j, i] = src_img[int(y), int(x)]


class DaSwap:
    def __init__(self, arg_parser):
        (self.options, self.args) = arg_parser.parse_args()
        if (self.options.predictor_path == 'None'):
            arg_parser.print_help()
            exit(1)

        self.detector = dlib.get_frontal_face_detector()
        self.predictor = dlib.shape_predictor(self.options.predictor_path)

        # Src face
        src_image = cv2.imread(self.options.target_path, cv2.IMREAD_COLOR)
        src_dets = self.detector(src_image, 1)
        self.src_face = Face(src_image, src_dets[0], self.predictor)

        if (self.options.target_video_path != ""):
            self.target_video = cv2.VideoCapture(self.options.target_video_path)
        else:
            # Dst face
            dst_image = cv2.imread(self.options.celebrity_img, cv2.IMREAD_COLOR)
            dst_dets = self.detector(dst_image, 1)
            self.dst_face = Face(dst_image, dst_dets[0], self.predictor)
        print okb("Initialization done")

    def do_blend(self, face, face_image):
        mask = np.zeros(face.image.shape, dtype = face.image.dtype)  
        hull = cv2.convexHull(face.landmarks, returnPoints = True)
        cv2.fillConvexPoly(mask, hull, (255, 255, 255))

        r = cv2.boundingRect(hull)    
        center = ((r[0]+int(r[2]/2), r[1]+int(r[3]/2)))        
        return cv2.seamlessClone(face_image, face.image, mask, center, cv2.NORMAL_CLONE) 


    def get_trimap(self, face, mapping):
        lu = np.min(face.landmarks, axis=0)
        rd = np.max(face.landmarks, axis=0)
        tri_map = np.zeros((rd[0] - lu[0] + 1, rd[1] - lu[1] + 1), dtype=np.int)
        points = face.landmarks
        for i in range(lu[0], rd[0] + 1, 1):
            for j in range(lu[1], rd[1] + 1, 1):               
                for k, t in enumerate(mapping):
                    pt1 = (points[t[0], 0], points[t[0], 1])
                    pt2 = (points[t[1], 0], points[t[1], 1])
                    pt3 = (points[t[2], 0], points[t[2], 1])
                    if(cv2.pointPolygonTest(np.array([pt1, pt2, pt3]), (i, j), False) != -1):
                        tri_map[i - lu[0], j - lu[1]] = k + 1
                        #self.draw_point(face.image, (i,j), colors[k % len(colors)])
                        break;
        return (lu, rd, tri_map)


    def get_tritrans(self, tri_id, mapping, lnd_src, lnd_dst):
        t = mapping[tri_id]
        m_src = np.matrix((np.append(lnd_src[t[0]], 1.0), np.append(lnd_src[t[1]], 1.0), np.append(lnd_src[t[2]], 1.0))).transpose()
        m_dst = np.matrix((np.append(lnd_dst[t[0]], 1.0), np.append(lnd_dst[t[1]], 1.0), np.append(lnd_dst[t[2]], 1.0))).transpose()

        try:
            return m_src * np.linalg.inv(m_dst)
        except:
            return m_src
        

    def triang_swap(self):
        landmarks = (self.src_face.landmarks + self.dst_face.landmarks) / 2.0
        mapping = Delaunay.get((0, 0, max(self.dst_face.imw, self.src_face.imw), 
                                      max(self.dst_face.imh, self.src_face.imh)), landmarks)
        print okb("Delaunay done")

        (lu, rd, tri_map) = self.get_trimap(self.dst_face, mapping)
        print okb("Trimap done")
 
        matrices = []
        for k, t in enumerate(mapping):
            matrices.append(self.get_tritrans(k, mapping, self.src_face.landmarks, self.dst_face.landmarks))

        print okb("Matrices done")

        face_img = np.copy(self.dst_face.image)
        for i in range(lu[0], rd[0] + 1, 1):
            for j in range(lu[1], rd[1] + 1, 1):        
                m_id = tri_map[i - lu[0], j - lu[1]] - 1;
                if (m_id < 0):
                    continue
                dst_pt = np.matrix([[i],[j],[1.0]])
                src_pt = matrices[m_id] * dst_pt
                x = int(src_pt[0] / src_pt[2])
                y = int(src_pt[1] / src_pt[2])
                if ((y < self.src_face.imh) and (x < self.src_face.imw)):
                    face_img[j, i] = self.src_face.image[y, x]

        blend_img = self.do_blend(self.dst_face, face_img)
        # self.draw_delaunay(self.src_face.image, self.src_face.landmarks, mapping)
        # self.draw_delaunay(self.dst_face.image, self.dst_face.landmarks, mapping)
        # cv2.imwrite("./triangulation_src.png", self.src_face.image)
        # cv2.imwrite("./triangulation_src_map.png", self.src_face.image)
        cv2.imwrite("./triangulation_result.png", blend_img)


    def U(self, p0, p1):
        diff = p0 - p1
        r = diff.dot(diff)
        if (r < 0.000001):
            return 0
        return r * math.log(r)


    def Uvec(self, p0, p_vec):
        diff = p_vec - p0
        r = np.einsum('ij,ij->i', diff, diff)
        return r * np.ma.log(r).filled(0)


    def tps(self, p, a0, ax, ay, w, landmarks):
        ret = a0 + ax * p[0] + ay * p[1]
        ret += np.sum(np.multiply(w.transpose(), np.array(self.Uvec(p, landmarks))))
        return ret


    def get_parameters(self, lnd_src, lnd_dst):
        K = np.zeros((len(lnd_src), len(lnd_dst)), dtype = np.float32)
        P = np.zeros((len(lnd_dst), 3), dtype = np.float32)
        for i, pi in enumerate(lnd_dst):
            P[i][0] = pi[0]
            P[i][1] = pi[1]
            P[i][2] = 1.0
            for j, pj in enumerate(lnd_dst):
                K[j, i] = self.U(pi, pj)
        K = np.matrix(K)
        P = np.matrix(P)
        Z = np.matrix(np.zeros((3,3), dtype = np.float32)) 
        I = np.identity(len(lnd_dst) + 3)
        M = np.concatenate((np.concatenate((K, P), axis=1), np.concatenate((P.transpose(), Z), axis=1)), axis=0) + 0.001 * I
         
        Vx = np.matrix(np.concatenate((lnd_src[:,0], np.array([0.0,0.0,0.0])), axis=0)).transpose()
        Vy = np.matrix(np.concatenate((lnd_src[:,1], np.array([0.0,0.0,0.0])), axis=0)).transpose()
      
        M_inv = np.linalg.inv(M)
        params_x = M_inv * Vx
        params_y = M_inv * Vy

        return (params_x, params_y)


    def tps_swap(self):
        (params_x, params_y) = self.get_parameters(self.src_face.landmarks, self.dst_face.landmarks)

        ax = params_x[-3:]
        wx = params_x[:-3]
        ay = params_y[-3:]
        wy = params_y[:-3]
  
        lu = np.min(self.dst_face.landmarks, axis=0)
        rd = np.max(self.dst_face.landmarks, axis=0)
        face_img = np.copy(self.dst_face.image)

        len_size = rd[0] + 1 - lu[0]
        step = len_size/10

        #print "Size: ", len_size, lu[0], rd[0]
        #rngs = range(0, len_size, step) + lu[0]
        #print rngs


        #face_img_shared = Array('d', face_img.reshape(-1,1))

        #p_handlers = []
        #for r in rngs:
        #    lu0 = r
        #    rd0 = min(r + step - 1, rd[0])
            
        #    p = Process(target=imtransfer, args=(face_img_shared, self.src_face.image, params_x, params_y, self.dst_face.landmarks, lu0, rd0, lu[1], rd[1],))
        #    p_handlers.append(p)
        #    p.start()
            #imtransfer(face_img, self.src_face.image, params_x, params_y, self.dst_face.landmarks, lu0, rd0, lu[1], rd[1])
        
        #for p in p_handlers:
        #    p.join()



        for i in range(lu[0], rd[0] + 1, 1):
            for j in range(lu[1], rd[1] + 1, 1):      
                p = np.array([i, j])
                x = self.tps(p, ax[2], ax[0], ax[1], wx, self.dst_face.landmarks)
                y = self.tps(p, ay[2], ay[0], ay[1], wy, self.dst_face.landmarks)
        
                if ((y < self.src_face.imh) and (x < self.src_face.imw)):
                    face_img[j, i] = self.src_face.image[int(y), int(x)]
       

        blend_img = self.do_blend(self.dst_face, face_img)
        return blend_img
        

    def swap(self):
        if (self.options.method == 'TRI'):
            self.triang_swap()    
            exit(0)

        if (self.options.method == 'TPS'):
            if (self.options.target_video_path != ""):
                fourcc = cv2.VideoWriter_fourcc(*'XVID')
                wwriter = cv2.VideoWriter('output.avi', fourcc, 20.0, (self.src_face.imw, self.src_face.imh))            

                while(self.target_video.isOpened()):
                    ret, frame = self.target_video.read()
                    dst_dets = self.detector(frame, 1)
                    self.dst_face = Face(frame, dst_dets[0], self.predictor)
                    result_frame = self.tps_swap()
                    wwriter.write(result_frame)
                
            else:
                cv2.imwrite("./tps_result.png", self.tps_swap())
                exit(0)


    def draw_point(self, img, p, color = (255,0,0)):
        cv2.circle(img, (p[0], p[1]), 2, color, thickness=1, lineType=8, shift=0)
 
 
    def draw_delaunay(self, img, points, mapping):
        delaunay_color = (255,255,255)
        for t in mapping:
            pt1 = (points[t[0], 0], points[t[0], 1])
            pt2 = (points[t[1], 0], points[t[1], 1])
            pt3 = (points[t[2], 0], points[t[2], 1])

            cv2.line(img, pt1, pt2, delaunay_color, 1, 8, 0)
            cv2.line(img, pt1, pt3, delaunay_color, 1, 8, 0)
            cv2.line(img, pt2, pt3, delaunay_color, 1, 8, 0)
            cv2.circle(img, pt1, 2, (0,0,255), thickness=1, lineType=8, shift=0)
            cv2.circle(img, pt2, 2, (0,0,255), thickness=1, lineType=8, shift=0)
            cv2.circle(img, pt3, 2, (0,0,255), thickness=1, lineType=8, shift=0)


# Dispatcher
if __name__ == '__main__':
    (major_ver, minor_ver, subminor_ver) = (cv2.__version__).split('.')

    if int(major_ver) < 3 :
        print >> sys.stderr, 'ERROR: Script needs OpenCV 3.0 or higher'
        sys.exit(1)

    class MyParser(OptionParser):
        def format_epilog(self, formatter):
            return self.epilog

    examples = ("")
    parser = MyParser(usage="Usage: " + sys.argv[0] + " <options>", epilog=examples)
    parser.add_option('-c', '--celebrity', dest='celebrity_img',
        help='specify path to celebrity picture (default: ../Data/rambo.jpg)', default="../Data/rambo.jpg")
    parser.add_option('-t', '--target',  dest='target_path',
        help='specify path to target image (default: ../Data/me.jpg)', default="../Data/me.jpg")
    parser.add_option('-t', '--target_video',  dest='target_video_path',
        help='specify path to target video (default: ../Data/video.avi)', default="../Data/video.avi")
    parser.add_option('--predictor',  dest='predictor_path',
        help='specify path to predictor (default: ./FaceDetectorCodes/DLib/python_examples/shape_predictor_68_face_landmarks.dat)', 
                                         default="./FaceDetectorCodes/DLib/python_examples/shape_predictor_68_face_landmarks.dat")
    parser.add_option('--method', dest='method',
            help="specify swap method: 'TRI' for triangulation based one or 'TPS' for " +
                 "Thin Plate Spline method", default='TPS', choices=['TRI', 'TPS'])

    da_swap = DaSwap(parser)
    da_swap.swap()

