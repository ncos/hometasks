#!/usr/bin/python


import cv2
import numpy as np
from optparse import OptionParser
from multiprocessing import Pool
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
        self.imh, self.imw, channels = self.image.shape
        self.bbox = (bbox_.left(), bbox_.top(), bbox_.right(), bbox_.bottom())
        predicted = predictor(self.image, self.dlibbbox)
        self.landmarks = np.array([[p.x, p.y] for p in predicted.parts()])       
        self.old_landmarks = np.array([[p.x, p.y] for p in predicted.parts()])       
        self.kalmans = [self.init_kalman(p) for p in self.landmarks]        

    def init_kalman(self, p):
        kalman = cv2.KalmanFilter(4, 2)
        kalman.measurementMatrix = np.array([[1,0,0,0],[0,1,0,0]],np.float32)
        kalman.transitionMatrix = np.array([[1,0,1,0],[0,1,0,1],[0,0,1,0],[0,0,0,1]],np.float32)
        kalman.processNoiseCov = np.array([[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]],np.float32) * 0.003
        kalman.measurementNoiseCov = np.array([[1,0],[0,1]],np.float32) * 0.5
        return kalman 

    def update(self, image_, bbox_, predictor):
        self.image = image_
        self.dlibbbox = bbox_
        self.imh, self.imw, channels = self.image.shape
        self.bbox = (bbox_.left(), bbox_.top(), bbox_.right(), bbox_.bottom())
        predicted = predictor(self.image, self.dlibbbox)

        for i, p in enumerate(predicted.parts()):
            mp = np.array((2,1), np.float32)
            tp = np.zeros((2,1), np.float32)
            mp[0] = p.x - self.old_landmarks[i, 0]
            mp[1] = p.y - self.old_landmarks[i, 1]
            self.kalmans[i].correct(mp)
            tp = self.kalmans[i].predict() 
            self.landmarks[i] = self.old_landmarks[i] + np.array([int(tp[0]), int(tp[1])])
        self.old_landmarks = self.landmarks
        #self.landmarks = np.array([[p.x, p.y] for p in predicted.parts()])       


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
    return -r * np.ma.log(r).filled(0)

def tps(p, a0, ax, ay, w, landmarks):
    ret = a0 + ax * p[0] + ay * p[1]
    ret += np.sum(np.multiply(w.transpose(), np.array(Uvec(p, landmarks))))
    return ret

def tps_warp_worker(args):
    return tps_warp(args[0], args[1], args[2], args[3], args[4], args[5], args[6])

def tps_warp(params_x, params_y, landmarks, lu0, rd0, lu1, rd1):
    ax = params_x[-3:]
    wx = params_x[:-3]
    ay = params_y[-3:]
    wy = params_y[:-3]
 
    warp_img = np.zeros((rd1 - lu1 + 1, rd0 - lu0 + 1, 2))
    for i in range(lu0, rd0 + 1, 1):
        for j in range(lu1, rd1 + 1, 1):      
            p = np.array([i, j])
            x = tps(p, ax[2], ax[0], ax[1], wx, landmarks)
            y = tps(p, ay[2], ay[0], ay[1], wy, landmarks)
            warp_img[j - lu1, i - lu0] = [int(x), int(y)]
    return warp_img


class DaSwap:
    def __init__(self, arg_parser):
        (self.options, self.args) = arg_parser.parse_args()
        if (self.options.predictor_path == 'None'):
            arg_parser.print_help()
            exit(1)

        self.debug_level = self.options.debug_level
        self.detector = dlib.get_frontal_face_detector()
        self.predictor = dlib.shape_predictor(self.options.predictor_path)

        self.src_face = None
        self.dst_face = None
        self.target_video = None
        self.face_swap = False
        if (self.options.source_img != ""):
            self.src_face = self.safe_open_image(self.options.source_img)
        
        if (self.options.target_img != ""):
            self.dst_face = self.safe_open_image(self.options.target_img)
        
        if (self.options.target_video_path != ""):
            self.target_video = cv2.VideoCapture(self.options.target_video_path)
       
        print okb("Initialization done")


    def safe_open_image(self, path, id_=0):
         image = cv2.imread(path, cv2.IMREAD_COLOR)
         dets = self.detector(image, 1)
         if (len(dets) < id_ + 1):
            print err("Error! face not found on " + path + " with id " + str(id_))
            return None
         return Face(image, dets[id_], self.predictor)


    def do_blend(self, face, face_image, input_img=None):
        if (input_img is None):
            input_img = face.image

        mask = np.zeros(face.image.shape, dtype = face.image.dtype)  
        hull = cv2.convexHull(face.landmarks, returnPoints = True)
        cv2.fillConvexPoly(mask, hull, (255, 255, 255))

        r = cv2.boundingRect(hull)    
        center = ((r[0]+int(r[2]/2), r[1]+int(r[3]/2)))
        return cv2.seamlessClone(face_image, input_img, mask, center, cv2.NORMAL_CLONE) 


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
                        break

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
        self.tri_mapping = Delaunay.get((0, 0, max(self.dst_face.imw, self.src_face.imw), 
                                               max(self.dst_face.imh, self.src_face.imh)), landmarks)
        print okb("Delaunay done")

        (lu, rd, tri_map) = self.get_trimap(self.dst_face, self.tri_mapping)
        print okb("Trimap done")
 
        matrices = []
        for k, t in enumerate(self.tri_mapping):
            matrices.append(self.get_tritrans(k, self.tri_mapping, self.src_face.landmarks, self.dst_face.landmarks))

        print okb("Matrices done")

        face_img = np.copy(self.dst_face.image)
        for i in range(lu[0], rd[0] + 1, 1):
            for j in range(lu[1], rd[1] + 1, 1):        
                m_id = tri_map[i - lu[0], j - lu[1]] - 1
                if (m_id < 0):
                    continue
                dst_pt = np.matrix([[i],[j],[1.0]])
                src_pt = matrices[m_id] * dst_pt
                x = int(src_pt[0] / src_pt[2])
                y = int(src_pt[1] / src_pt[2])
                if ((y < self.src_face.imh) and (x < self.src_face.imw)):
                    face_img[j, i] = self.src_face.image[y, x]

        if (self.debug_level > 0):               
            self.draw_delaunay(self.src_face.image, self.src_face.landmarks, self.tri_mapping)
            self.draw_delaunay(self.dst_face.image, self.dst_face.landmarks, self.tri_mapping)
            cv2.imwrite("./triangulation_src_triang.png", self.src_face.image)
            cv2.imwrite("./triangulation_dst_triang.png", self.dst_face.image)
            self.draw_delunay_color(self.src_face.image, self.src_face, self.tri_mapping)
            self.draw_delunay_color(self.dst_face.image, self.dst_face, self.tri_mapping)
            cv2.imwrite("./triangulation_src_triang_color.png", self.src_face.image)
            cv2.imwrite("./triangulation_dst_triang_color.png", self.dst_face.image)

        blend_img = self.do_blend(self.dst_face, face_img)
        return blend_img


    def U(self, p0, p1):
        diff = p0 - p1
        r = diff.dot(diff)
        if (r < 0.000001):
            return 0
        return -r * math.log(r)


    def Uvec(self, p0, p_vec):
        diff = p_vec - p0
        r = np.einsum('ij,ij->i', diff, diff)
        return -r * np.ma.log(r).filled(0)


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

        
        # Threading
        len_size = rd[0] + 1 - lu[0]
        step = len_size/10
        rngs = range(0, len_size, step) + lu[0]

        thread_args = []
        thread_result = []
        for i in xrange(len(rngs)):
            lu0 = rngs[i]
            rd0 = min(lu0 + step - 1, rd[0])
            thread_args.append([params_x, params_y, self.dst_face.landmarks, lu0, rd0, lu[1], rd[1]])
        
        thread_pool = Pool(processes=len(thread_args))
        thread_result = thread_pool.map(tps_warp_worker, thread_args)
        thread_pool.close()
        warp_img = np.hstack(thread_result)
        #warp_img = tps_warp(params_x, params_y, self.dst_face.landmarks, lu[0], rd[0], lu[1], rd[1])

        for i in range(lu[0], rd[0] + 1, 1):
            for j in range(lu[1], rd[1] + 1, 1):      
                x = warp_img[j - lu[1], i - lu[0], 0]
                y = warp_img[j - lu[1], i - lu[0], 1]
                if ((int(y) < self.src_face.imh) and (int(x) < self.src_face.imw)):
                    face_img[j, i] = self.src_face.image[int(y), int(x)]

        face_img = self.do_blend(self.dst_face, face_img)
        if (not self.face_swap):
            return face_img

        self.src_face.image = np.copy(face_img)
        (params_x, params_y) = self.get_parameters(self.dst_face.landmarks, self.src_face.landmarks)

        ax = params_x[-3:]
        wx = params_x[:-3]
        ay = params_y[-3:]
        wy = params_y[:-3]
  
        lu = np.min(self.src_face.landmarks, axis=0)
        rd = np.max(self.src_face.landmarks, axis=0)
        face_img = np.copy(self.src_face.image)
        
        # Threading
        len_size = rd[0] + 1 - lu[0]
        step = len_size/10
        rngs = range(0, len_size, step) + lu[0]

        thread_args = []
        thread_result = []
        for i in xrange(len(rngs)):
            lu0 = rngs[i]
            rd0 = min(lu0 + step - 1, rd[0])
            thread_args.append([params_x, params_y, self.src_face.landmarks, lu0, rd0, lu[1], rd[1]])
        
        thread_pool = Pool(processes=len(thread_args))
        thread_result = thread_pool.map(tps_warp_worker, thread_args)
        thread_pool.close()
        warp_img = np.hstack(thread_result)

        for i in range(lu[0], rd[0] + 1, 1):
            for j in range(lu[1], rd[1] + 1, 1):      
                x = warp_img[j - lu[1], i - lu[0], 0]
                y = warp_img[j - lu[1], i - lu[0], 1]
                if ((y < self.dst_face.imh) and (x < self.dst_face.imw)):
                    face_img[j, i] = self.dst_face.image[int(y), int(x)]

        face_img = self.do_blend(self.src_face, face_img)                
        return face_img


    def swap(self):
        if (self.dst_face != None):
            out_image_name = os.path.splitext(self.options.target_img)[0] + "Out" + os.path.splitext(self.options.target_img)[1]
            print okb("Going to write to: " + out_image_name)

            if (self.src_face == None):
                print okb("Ok, going into the face swap mode...")
                self.src_face = self.safe_open_image(self.options.target_img, 1)
                self.face_swap = True

            if (self.options.method == 'TRI'):
                cv2.imwrite(out_image_name, self.triang_swap())
            
            if (self.options.method == 'TPS'):
                cv2.imwrite(out_image_name, self.tps_swap())


        if (self.target_video != None):
            out_video_name =  os.path.splitext(self.options.target_video_path)[0] + "Out.avi"
            print okb("Going to write to: " + out_video_name)
            ret, frame = self.target_video.read()
            (h, w, c) = frame.shape
            fourcc = cv2.VideoWriter_fourcc(*'MJPG')
            wwriter = cv2.VideoWriter(out_video_name, fourcc, 30.0, (w, h))

            if (self.src_face == None):
                print okb("Ok, going into the face swap mode...")
                self.face_swap = True

            dst_dets = []
            while len(dst_dets) == 0:
                ret, frame = self.target_video.read()
                dst_dets = self.detector(frame, 1)
            
            self.dst_face = Face(frame, dst_dets[0], self.predictor)
            
            if (self.face_swap):
                while len(dst_dets) == 1:
                    ret, frame = self.target_video.read()
                    dst_dets = self.detector(frame, 1)
            
                self.src_face = Face(frame, dst_dets[1], self.predictor)
            
            while(self.target_video.isOpened()):
                ret, result_frame = self.target_video.read()
                if (ret == False):
                    break

                dst_dets = self.detector(result_frame, 1)
                if (len(dst_dets) == 0):
                    print err("No face detected!")
                elif ((len(dst_dets) == 1) and self.face_swap):
                    print err("Swapping faces, but only one face on the image!")
                else:
                    self.dst_face.update(result_frame, dst_dets[0], self.predictor)
                    if (self.face_swap):
                        self.src_face.update(result_frame, dst_dets[1], self.predictor)

                    if (self.options.method == 'TRI'):
                        result_frame = self.triang_swap()
                    
                    if (self.options.method == 'TPS'):
                        result_frame = self.tps_swap()

                    if (self.debug_level > 0):               
                        self.draw_landmarks(result_frame, self.dst_face.landmarks) 
                        if (self.face_swap):
                            self.draw_landmarks(result_frame, self.src_face.landmarks) 
                         
                wwriter.write(result_frame)
                cv2.imshow('frame', result_frame)
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break

            self.target_video.release()
            wwriter.release()
            cv2.destroyAllWindows()


    def draw_point(self, img, p, color = (255,0,0)):
        cv2.circle(img, (p[0], p[1]), 2, color, thickness=1, lineType=8, shift=0)
    
    def draw_landmarks(self, img, landmarks, color = (255,0,0)):
        for p in landmarks:
            self.draw_point(img, p, color)
 
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

    def draw_delunay_color(self, img, face, mapping):
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
                        self.draw_point(img, (i,j), colors[k % len(colors)])
                        break
        return (lu, rd, tri_map)



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
    parser.add_option('-s', '--source', dest='source_img',
        help='specify path to source picture', default="")
    parser.add_option('-t', '--target',  dest='target_img',
        help='specify path to target image', default="")
    parser.add_option('-v', '--target_video',  dest='target_video_path',
        help='specify path to target video', default="")
    parser.add_option('--predictor',  dest='predictor_path',
        help='specify path to predictor (default: ./FaceDetectorCodes/DLib/python_examples/shape_predictor_68_face_landmarks.dat)', 
                                         default="./FaceDetectorCodes/DLib/python_examples/shape_predictor_68_face_landmarks.dat")
    parser.add_option('--method', dest='method',
            help="specify swap method: 'TRI' for triangulation based one or 'TPS' for " +
                 "Thin Plate Spline method", default='TPS', choices=['TRI', 'TPS'])
    parser.add_option('--debug-level', dest='debug_level',
            help='debug message (and other stuff) level - the higher the better', type="int", default=0)


    da_swap = DaSwap(parser)
    da_swap.swap()

