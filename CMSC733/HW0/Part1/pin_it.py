#!/usr/bin/python

import cv2
import numpy as np
from optparse import OptionParser
from os.path import isfile, join
import os


class OSHelpers:
    @staticmethod
    def ensure_dir(f):
        if not os.path.exists(f):
            os.makedirs(f)


class PinIt:
    def __init__(self, arg_parser):
        (self.options, self.args) = arg_parser.parse_args()

    def start():


    def denoise(image):
        image_smth = image
        self.options.:
            image_smth = cv2.fastNlMeansDenoisingColored(image, templateWindowSize=7,
                                                      searchWindowSize=21,
                                                      h=3,
                                                      hColor=10)
        else:
            image_smth = cv2.bilateralFilter(image, 5, 50, 50)

       




def convert(src, dst):
    bgr = cv2.imread(src, cv2.IMREAD_COLOR)
    denoised = denoise(bgr)
    cv2.imwrite(dst, denoised)

if __name__ == '__main__':
    class MyParser(OptionParser):
        def format_epilog(self, formatter):
            return self.epilog

    examples = ("")
    parser = MyParser(usage="Usage: pin_it.py <options>", epilog=examples)
    parser.add_option('--debug_info', dest='debug',
        help='save/show/print debug images/info', default=True, action="store_true")
    parser.add_option('--denoise', dest='denoise_method',
        help="specify denoise method; available: 'BLT' for bilateral filter and 'NlMeans' for " +
             "non-local means denoise algorithm", default='NlMeans', choices=['NlMeans', 'BLT'])
    parser.add_option('-n', '--name', dest='input_name',
        help='specify input image file name', default="TestImgResized.img")
   
    if (len(sys.argv) == 1):
        parser.print_help()
        exit(1)

    PinIt pin_it(parser)
    pin_it.start()
