#/usr/bin/python

import cv2
import numpy as np
from optparse import OptionParser
from os.path import isfile, join
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
        if (self.options.name == 'None'):
            arg_parser.print_help()
            exit(1)

    def start(self):
        



    def get_gaussians(self):
        gaussians = []
        for scale in xrange(self.options.scales):
            for rot in xrange(self.options.rotations):
                

# Dispatcher
if __name__ == '__main__':
    class MyParser(OptionParser):
        def format_epilog(self, formatter):
            return self.epilog

    examples = ("")
    parser = MyParser(usage="Usage: pblite.py <options>", epilog=examples)
    parser.add_option('-n', '--name', dest='input_name',
        help='specify input image file name (default: ../TestImgResized.jpg)', default="../TestImgResized.jpg")
    parser.add_option('--gaussians', dest='gaussian_path',
        help='specify path to folder with gaussian database (default: ../Images)', default="../Images")
    parser.add_option('--scales', dest='scales',
        help='number of scales for gaussians and etc. (default: 2)', type="int", default=2)
    parser.add_option('--rotations', dest='rotations',
        help='number of rotations for gaussians and etc. (default: 8)', type="int", default=8)
    
    pin_it = PinIt(parser)
    pin_it.start()
