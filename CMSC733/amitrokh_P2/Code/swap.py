#!/usr/bin/python


import cv2
import numpy as np
from optparse import OptionParser
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
    



# Core
class Face:
    def __init__(self, image_, bbox_, predictor):
        self.image = image_
        self.dlibbbox = bbox_
        self.bbox = (bbox_.left(), bbox_.top(), bbox_.right(), bbox_.bottom())
        predicted = predictor(self.image, self.dlibbbox)
        self.landmarks = np.array([[p.x, p.y] for p in predicted.parts()])
        self.imh, self.imw, channels = self.image.shape

        print "\n\n-----------"
        print self.landmarks

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
        # Dst face
        dst_image = cv2.imread(self.options.celebrity_img, cv2.IMREAD_COLOR)
        dst_dets = self.detector(dst_image, 1)
        self.dst_face = Face(dst_image, dst_dets[0], self.predictor)


    def swap(self):
        subdiv = cv2.Subdiv2D((0, 0, self.dst_face.imw, self.dst_face.imh))
        for p in self.dst_face.landmarks:
            print p[0], p[1]
            self.draw_point(self.dst_face.image, p, (255,0,0))
            subdiv.insert((p[0], p[1]))

        self.draw_delaunay(self.dst_face.image, subdiv, (255,255,255))
        cv2.imshow("Triangulation", self.dst_face.image)
        cv2.waitKey(0)

    # Draw a point
    def draw_point(self, img, p, color):
        cv2.circle(img, (p[0], p[1]), 2, color, thickness=1, lineType=8, shift=0)
 
 
    # Draw delaunay triangles
    def draw_delaunay(self, img, subdiv, delaunay_color):
        triangleList = subdiv.getTriangleList();
        size = img.shape
        r = (0, 0, size[1], size[0])
 
        for t in triangleList :
         
            pt1 = (t[0], t[1])
            pt2 = (t[2], t[3])
            pt3 = (t[4], t[5])
         
         
            cv2.line(img, pt1, pt2, delaunay_color, 1, 8, 0)
            cv2.line(img, pt2, pt3, delaunay_color, 1, 8, 0)
            cv2.line(img, pt3, pt1, delaunay_color, 1, 8, 0)

        


# Dispatcher
if __name__ == '__main__':
    class MyParser(OptionParser):
        def format_epilog(self, formatter):
            return self.epilog

    examples = ("")
    parser = MyParser(usage="Usage: " + sys.argv[0] + " <options>", epilog=examples)
    parser.add_option('-c', '--celebrity', dest='celebrity_img',
        help='specify path to celebrity picture (default: ../Data/rambo.jpg)', default="../Data/rambo.jpg")
    parser.add_option('-t', '--target',  dest='target_path',
        help='specify path to target image (default: ../Data/me.jpg)', default="../Data/me.jpg")
    parser.add_option('--predictor',  dest='predictor_path',
        help='specify path to predictor (default: ./FaceDetectorCodes/DLib/python_examples/shape_predictor_68_face_landmarks.dat)', 
                                         default="./FaceDetectorCodes/DLib/python_examples/shape_predictor_68_face_landmarks.dat")

    da_swap = DaSwap(parser)
    da_swap.swap()

