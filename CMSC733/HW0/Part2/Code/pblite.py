#!/usr/bin/python

import cv2
import numpy as np
from optparse import OptionParser
from os.path import isfile, join
import scipy.ndimage.filters as fi
from scipy import signal
from scipy import misc
import sys, os, math

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
    

# File manipulation
class OSHelpers:
    @staticmethod
    def ensure_dir(f):
        if not os.path.exists(f):
            os.makedirs(f)
    @staticmethod
    def get_db_folders(d):
        full_paths = [os.path.join(d,o) for o in os.listdir(d) if os.path.isdir(os.path.join(d,o))]
        names = [o for o in os.listdir(d) if os.path.isdir(os.path.join(d,o))]
        return (full_paths, names)
    @staticmethod
    def get_db_numpy(db_path, file_name):
        try:
            mat = np.load(join(db_path, file_name))
        except:
            print "Error reading file " + file_name + " at " + db_path
            return
        return mat


# Core
class PBlite:
    def __init__(self, arg_parser):
        (self.options, self.args) = arg_parser.parse_args()
        if (self.options.input_name == 'None'):
            arg_parser.print_help()
            exit(1)

    def start(self):
        self.get_gaussians()
        self.get_hdmasks()


    def gkern2(self, nsig):
        inp = np.zeros((self.options.filter_size, self.options.filter_size))
        inp[self.options.filter_size // 2, self.options.filter_size // 2] = 1.0
        return fi.gaussian_filter(inp, nsig)

    def scale(self, img):
        min_, max_, min_loc_, max_loc_ = cv2.minMaxLoc(img)
        return (img - min_) / float(max_ - min_)

    def get_gaussians(self):
        gaussians = {}
        xsobel = np.array([[-1.0, 0.0, 1.0], [-2.0, 0.0, 2.0], [-1.0, 0.0, 1.0]])
        ysobel = np.array([[-1.0, -2.0, -1.0], [0.0, 0.0, 0.0], [1.0, 2.0, 1.0]])
        for scale in xrange(self.options.scales):
            gaussians[scale] = {}
            sigma = (scale + 1) * math.sqrt(2)
            gaussian = self.gkern2(sigma)

            gx = signal.convolve2d(gaussian, xsobel, boundary='symm', mode='same')
            gy = signal.convolve2d(gaussian, ysobel, boundary='symm', mode='same')
            for rot in xrange(self.options.rotations):
                angle = (rot + 1) * 2.0 * math.pi / float(self.options.rotations);
                g_image = gx * math.cos(angle) + gy * math.sin(angle);
                gaussians[scale][rot] = g_image

        g_set = np.concatenate(tuple([np.concatenate(tuple([self.scale(im) for im in scaled.values()]), axis=1) \
                               for scaled in gaussians.values()]))
        cv2.imwrite(os.path.join(self.options.gaussian_path, "gaussians.jpg"), g_set * 255)
        return gaussians


    def get_hdmasks(self):
        hdmasks = {}
        for scale in xrange(self.options.scales):
            hdmasks[scale] = {}
            radius = (scale + 1) * (self.options.filter_size - 1) / float(2 * self.options.scales)
            circle = np.zeros((self.options.filter_size, self.options.filter_size))
            cv2.circle(circle, (self.options.filter_size // 2, self.options.filter_size // 2), \
                                int(radius), (1, 1, 1), -1, 8, 0)
            diskL = circle
            diskL[:][xrange(self.options.filter_size / 2)] = 0.0
            diskR = np.flipud(diskL)

            for rot in xrange(self.options.rotations):
                M = cv2.getRotationMatrix2D((self.options.filter_size // 2, self.options.filter_size // 2), \
                                             int((rot + 1) * 180.0 / float(self.options.rotations)), 1)
                diskLR = cv2.warpAffine(diskL, M, (self.options.filter_size, self.options.filter_size))
                diskRR = cv2.warpAffine(diskR, M, (self.options.filter_size, self.options.filter_size))
                hdmasks[scale][rot] = [diskLR, diskRR]
        hd_set = np.concatenate(tuple([np.concatenate(tuple([im[0] for im in scaled.values()]), axis=1) \
                                for scaled in hdmasks.values()]))
        cv2.imwrite(os.path.join(self.options.hdmask_path, "hdmasks.jpg"), hd_set * 255)
        return hdmasks

    
    def LM(self):
        for scale in xrange(3):
             


# Dispatcher
if __name__ == '__main__':
    class MyParser(OptionParser):
        def format_epilog(self, formatter):
            return self.epilog

    examples = ("")
    parser = MyParser(usage="Usage: pblite.py <options>", epilog=examples)
    parser.add_option('-n', '--name', dest='input_name',
        help='specify input image file name (default: ../TestImgResized.jpg)', default="../TestImgResized.jpg")
    parser.add_option('--path_gaussian', dest='gaussian_path',
        help='specify path to folder to save gaussian database (default: ../Images)', default="../Images")
    parser.add_option('--path_hdmask', dest='hdmask_path',
        help='specify path to folder to save hdmask database (default: ../Images)', default="../Images")
    parser.add_option('--filter_size', dest='filter_size',
        help='specify the size of filters to use (odd number, default: 49)', type="int", default=49)
    parser.add_option('--scales', dest='scales',
        help='number of scales for gaussians and etc. (default: 2)', type="int", default=2)
    parser.add_option('--rotations', dest='rotations',
        help='number of rotations for gaussians and etc. (default: 8)', type="int", default=8)
    

    pblite = PBlite(parser)
    pblite.start()
