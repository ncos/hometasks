#!/usr/bin/python

import cv2
import numpy as np
from optparse import OptionParser
from os.path import isfile, join
import sys, os

if __name__ == '__main__':
    class MyParser(OptionParser):
        def format_epilog(self, formatter):
            return self.epilog

    examples = ("")
    parser = MyParser(usage="averager.py <options>\nGenerate an average vector for image and the covariance matrix\n", epilog=examples)
    parser.add_option('-i', '--input', dest='input_folder',
        help='specify input folder with sample images')

    (options, args) = parser.parse_args()

    if (len(sys.argv) == 1):
        parser.print_help()
        exit(1)

    files = [join(options.input_folder, f) for f in os.listdir(options.input_folder) if isfile(join(options.input_folder, f))] 
    # print "found " + str(len(files)) + " images"
    if (len(files) == 0):
        print "No images to process!"
        exit(1)

    images = np.empty([0, 3])
    for i, f in enumerate(files):
        try:
            image = cv2.imread(f, cv2.IMREAD_COLOR).reshape(1, -1, 3)[0]  
            images = np.concatenate((images, image))
        except:
            pass

    cov_mat = np.cov(images, rowvar=False)
    mean_vec = np.mean(images, axis=0)


    print "Mean:"
    print mean_vec
    print ""
    print "Covariance matrix:"
    print cov_mat

    np.save(join(options.input_folder, 'mean'), mean_vec)
    np.save(join(options.input_folder, 'cov'), cov_mat)

