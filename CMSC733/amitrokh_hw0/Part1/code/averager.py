#!/usr/bin/python

import cv2
import numpy as np
from optparse import OptionParser
from os.path import isfile, join
import sys, os


def save_mean_cov(array, path, prefix=""):
    cov_mat = np.cov(array, rowvar=False)
    mean_vec = np.mean(array, axis=0)

    print "\nSaving to " + path
    print "Array shape: " + str(array.shape)
    if (prefix != ""):
        print "Prefix: " + prefix
    print "Mean:"
    print mean_vec
    print 
    print "Covariance matrix:"
    print cov_mat
    print
 
    np.save(join(path, 'mean' + prefix), mean_vec)
    np.save(join(path, 'cov' + prefix), cov_mat)


def save_all_kmns(array, K, path):
    Z = np.float32(array)
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 10, 1.0)
    ret, label, center = cv2.kmeans(Z, K, None, criteria, 10, cv2.KMEANS_RANDOM_CENTERS)

    for i in xrange(K):
        save_mean_cov(Z[label.ravel() == i], path, "_kmeans_" + str(i))

if __name__ == '__main__':
    class MyParser(OptionParser):
        def format_epilog(self, formatter):
            return self.epilog

    examples = ("")
    parser = MyParser(usage="averager.py <options>\nGenerate an average vector for image and the covariance matrix\n", epilog=examples)
    parser.add_option('-i', '--input', dest='input_folder',
        help='specify input folder with sample images')
    parser.add_option('--method', dest='cov_method',
        help="specify operation method: kmeans or regular; available: 'RG', 'KMNS' - default: RG", default='RG', choices=['RG', 'KMNS'])
    parser.add_option('--clusters', dest='kmns_num', default=3,
        help='specify number of clusters for kmeans (default is 3)')

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

    if (options.cov_method == "RG"):
        save_mean_cov(images, options.input_folder)
        exit(0)

    if (options.cov_method == "KMNS"):
        save_all_kmns(images, int(options.kmns_num), options.input_folder)
        exit(0)
