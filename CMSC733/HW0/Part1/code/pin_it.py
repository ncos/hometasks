#!/usr/bin/python

import cv2
import numpy as np
from optparse import OptionParser
from os.path import isfile, join
import sys, os, math


class OSHelpers:
    @staticmethod
    def ensure_dir(f):
        if not os.path.exists(f):
            os.makedirs(f)


class PinIt:
    def __init__(self, arg_parser):
        (self.options, self.args) = arg_parser.parse_args()

    def start(self):
        try:
            print "Opening " + self.options.input_name
            image = cv2.imread(self.options.input_name, cv2.IMREAD_COLOR)
            denoised = self.denoise(image)
        except:
            print "Error reading image, exiting..."
            exit(1)

        self.gaussian3D(denoised, self.options.target_class)

    def denoise(self, image):
        image_smth = image
        if (self.options.denoise_method == 'NlMeans'):
            image_smth = cv2.fastNlMeansDenoisingColored(image, templateWindowSize=7,
                                                                searchWindowSize=21,
                                                                h=3,
                                                                hColor=10)
        if (self.options.denoise_method == 'BLT'):
            image_smth = cv2.bilateralFilter(image, 5, 50, 50)

        return image_smth

    def gaussian3D(self, image, target):
        height, width, channels = image.shape 
        filter_image = np.zeros((height,width))
        
        try:
            mean_vec = np.load(join(target, 'mean.npy'))
            cov_mat = np.load(join(target, 'cov.npy'))
        except:
            print "Error reading target mean and covariance files, exiting..."
            exit(1)

        print mean_vec
        print cov_mat

        inv_cov_mat = np.linalg.inv(cov_mat)

        for i in xrange(height):
            for j in xrange(width):
                error = image[i, j] - mean_vec
                filter_image[i, j] = math.exp(-1.0 * error.dot(inv_cov_mat.dot(error)) / 2.0)  
                #filter_image[i, j] = error.dot(inv_cov_mat.dot(error))

        print filter_image

        minVal, maxVal, minLoc, maxLoc = cv2.minMaxLoc(filter_image)
        print minVal, maxVal, minLoc, maxLoc

        cv2.imshow( "Display window", filter_image);
        cv2.waitKey(0);


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
    parser.add_option('-n', '--name', dest='input_name',
        help='specify input image file name', default="../TestImgResized.jpg")
    parser.add_option('--class', dest='target_class',
        help='specify path to folder with target class to find', default="../database/blue")
    parser.add_option('--denoise', dest='denoise_method',
        help="specify denoise method; available: 'BLT' for bilateral filter and 'NlMeans' for " +
             "non-local means denoise algorithm", default='NlMeans', choices=['NlMeans', 'BLT'])
    parser.add_option('--debug_info', dest='debug',
        help='save/show/print debug images/info', default=True, action="store_true")

    if (len(sys.argv) == 1):
        parser.print_help()
        exit(1)

    pin_it = PinIt(parser)
    pin_it.start()
