#!/usr/bin/python

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
class PinIt:
    def __init__(self, arg_parser):
        (self.options, self.args) = arg_parser.parse_args()
        if (self.options.pinit_method == 'None'):
            arg_parser.print_help()
            exit(1)

    def start(self):
        try:
            print "Opening " + self.options.input_name
            image = cv2.imread(self.options.input_name, cv2.IMREAD_COLOR)
            denoised = self.denoise(image)
        except:
            print "Error reading image, exiting..."
            exit(1)

        if (self.options.pinit_method == 'G1D'):
            self.gaussian1D(denoised)
        
        if (self.options.pinit_method == 'G3D'):
            self.gaussian3D(denoised)

        if (self.options.pinit_method == 'GMM'):
            self.gaussianGMM(denoised)



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


    def do_box_stuff(self, input_image, mask, color = (255, 255, 255)):
        im2, contours, hierarchy = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

        try: hierarchy = hierarchy[0]
        except: hierarchy = []

        height, width, channels = input_image.shape
        min_x, min_y = width, height
        max_x = max_y = 0

        cnt = 0
        for contour, hier in zip(contours, hierarchy):
            (x,y,w,h) = cv2.boundingRect(contour)
            min_x, max_x = min(x, min_x), max(x+w, max_x)
            min_y, max_y = min(y, min_y), max(y+h, max_y)
            if w < width/3.0 and h < height/3.0:
                cv2.rectangle(input_image, (x,y), (x+w,y+h), color, 2)
                cnt = cnt + 1
        return (input_image, cnt)


    def gaussian1D(self, image):
        height, width, channels = image.shape

        red_normalized   = np.zeros((height,width))
        green_normalized = np.zeros((height,width))
        blue_normalized  = np.zeros((height,width))

        red_probability    = np.zeros((height,width))
        green_probability  = np.zeros((height,width))
        blue_probability   = np.zeros((height,width))
        yellow_probability = np.zeros((height,width))

        nu = {'red' : 0.85, 'green': 0.85, 'blue' : 0.85, 'yellow' : 0.70}
        s_sq = {'red' : 0.05, 'green': 0.05, 'blue' : 0.05, 'yellow' : 0.05}

        for i in xrange(height):
            for j in xrange(width):
                blue_normalized[i, j]  = image[i, j, 0]/float(np.sum(image[i, j]))
                green_normalized[i, j] = image[i, j, 1]/float(np.sum(image[i, j]))
                red_normalized[i, j]   = image[i, j, 2]/float(np.sum(image[i, j])) 
                red_probability[i, j]    = math.exp(-1.0 * (red_normalized[i, j] - nu['red'])**2 / (2.0 * s_sq['red']))
                green_probability[i, j]  = math.exp(-1.0 * (green_normalized[i, j] - nu['green'])**2 / (2.0 * s_sq['green']))
                blue_probability[i, j]   = math.exp(-1.0 * (blue_normalized[i, j] - nu['blue'])**2 / (2.0 * s_sq['blue']))

        cv2.imshow("B: ", blue_normalized)
        cv2.imshow("G: ", green_normalized)
        cv2.imshow("R: ", red_normalized)
        yellow_probability = red_probability * green_probability;
        
        print "Blue: " + str(cv2.minMaxLoc(blue_probability))
        ret, blue_mask = cv2.threshold(np.uint8(blue_probability * 255), 70, 255, cv2.THRESH_BINARY)
        print "Green: " + str(cv2.minMaxLoc(green_probability))
        ret, green_mask = cv2.threshold(np.uint8(green_probability * 255), 70, 255, cv2.THRESH_BINARY)
        print "Red: " + str(cv2.minMaxLoc(red_probability))
        ret, red_mask = cv2.threshold(np.uint8(red_probability * 255), 130, 255, cv2.THRESH_BINARY)
        print "Yellow: " + str(cv2.minMaxLoc(yellow_probability))
        ret, yellow_mask = cv2.threshold(np.uint8(yellow_probability * 255), 5, 255, cv2.THRESH_BINARY)

        kernel = np.ones((5,5),np.uint8)
        blue_mask = cv2.dilate(blue_mask, kernel, iterations = 3)
        green_mask = cv2.dilate(green_mask, kernel, iterations = 3)
        red_mask = cv2.dilate(red_mask, kernel, iterations = 3)
        yellow_mask = cv2.dilate(yellow_mask, kernel, iterations = 3)

        cv2.imshow("Blue probablilities: ", np.concatenate((blue_probability, blue_mask)))
        cv2.imshow("Green probablilities: ", np.concatenate((green_probability, green_mask)))
        cv2.imshow("Red probablilities: ", np.concatenate((red_probability, red_mask)))
        cv2.imshow("Yellow probablilities: ", np.concatenate((yellow_probability, yellow_mask)))

        box_img = image

        box_img, cnt = self.do_box_stuff(box_img, blue_mask, (255, 0, 0))
        print "Found " + str(cnt) + " blue pins"
        box_img, cnt = self.do_box_stuff(box_img, green_mask, (0, 255, 0))
        print "Found " + str(cnt) + " green pins"
        box_img, cnt = self.do_box_stuff(box_img, red_mask, (0, 0, 255))
        print "Found " + str(cnt) + " red pins"
        box_img, cnt = self.do_box_stuff(box_img, yellow_mask, (0, 255, 255))
        print "Found " + str(cnt) + " yellow pins"

        cv2.imshow("Box image ", box_img)
        cv2.waitKey(0);


    def gaussian3D_base(self, image, class_names, mean_vecs, cov_mats):
        mask_images = {}
        height, width, channels = image.shape 
        for id_, name in enumerate(class_names):
            print bld(wrn("\n\nProcessing class " + name))
            print "Mean vec:\n" + str(mean_vecs[id_])
            print "\nCovariance matrix:\n" + str(cov_mats[id_])
            
            try:
                inv_cov_mat = np.linalg.inv(cov_mats[id_])
            except:
                print "Apparently one of the matrices has no inverse!"
                continue
            
            mask_image = np.zeros((height,width))

            for i in xrange(height):
                for j in xrange(width):
                    error = image[i, j] - mean_vecs[id_]
                    mask_image[i, j] = math.exp(-1.0 * error.dot(inv_cov_mat.dot(error)) / 2.0)  

            mask_images[name] = mask_image
        return mask_images


    def gaussian3D(self, image):
        class_paths, class_names = OSHelpers.get_db_folders(self.options.db_root_path)
        print "Found classes: " + str(class_names)

        mean_vecs = [OSHelpers.get_db_numpy(p, 'mean.npy') for p in class_paths]
        cov_mats  = [OSHelpers.get_db_numpy(p, 'cov.npy' ) for p in class_paths]
        mask_images = self.gaussian3D_base(image, class_names, mean_vecs, cov_mats)

        box_img = image
        colormap = {'red': (0,0,255), 'green': (0,255,0), 'blue': (255,0,0), 
                    'yellow': (0,255,255), 'white': (255,255,255), 'transparent': (0,0,0)}
        total_pins = 0
        for name in mask_images.keys():
            print name + ": " + str(cv2.minMaxLoc(mask_images[name]))
            ret, bin_mask = cv2.threshold(np.uint8(mask_images[name] * 255), 70, 255, cv2.THRESH_BINARY)
            kernel = np.ones((5,5),np.uint8)
            bin_mask = cv2.erode(bin_mask, kernel, iterations = 1)
            bin_mask = cv2.dilate(bin_mask, kernel, iterations = 5)
            cv2.imshow(name + " probablilities: ", np.concatenate((mask_images[name], bin_mask)))
            
            box_img, cnt = self.do_box_stuff(box_img, bin_mask, colormap[name])
            print okb("Found " + str(cnt) + " " + name + " pins")
            total_pins += cnt

        print okb("Total number of pins: " + str(total_pins))
        cv2.imshow("Box image ", box_img)
        cv2.waitKey(0);


    def gaussianGMM(self, image):
        class_paths, class_names = OSHelpers.get_db_folders(self.options.db_root_path)
        print "Found classes: " + str(class_names)

        # that's an ugly workaround...
        mean_names = [f for f in os.listdir(class_paths[0]) if ("mean_kmeans" in f)]
        cov_names = [f for f in os.listdir(class_paths[0]) if ("cov_kmeans" in f)]
        
        mask_images_all = {}
        for m_name, c_name in zip(mean_names, cov_names):
            mean_vecs = [OSHelpers.get_db_numpy(p, m_name) for p in class_paths]
            cov_mats  = [OSHelpers.get_db_numpy(p, c_name) for p in class_paths]
            mask_images = self.gaussian3D_base(image, class_names, mean_vecs, cov_mats)
            for key in mask_images.keys():
                if key not in mask_images_all.keys():
                    mask_images_all[key] = []
                mask_images_all[key].append(mask_images[key])

        mask_images = {}
        for name in mask_images_all.keys():
            height, width, channels = image.shape
            mask_images[name] = np.zeros((height,width))
            for mask in mask_images_all[name]:
                mask_images[name] += mask

        box_img = image
        colormap = {'red': (0,0,255), 'green': (0,255,0), 'blue': (255,0,0), 
                    'yellow': (0,255,255), 'white': (255,255,255), 'transparent': (0,0,0)}
        total_pins = 0
        for name in mask_images.keys():
            print name + ": " + str(cv2.minMaxLoc(mask_images[name]))
            ret, bin_mask = cv2.threshold(np.uint8(mask_images[name] * 255), 7, 255, cv2.THRESH_BINARY)
            kernel = np.ones((5,5),np.uint8)
            bin_mask = cv2.erode(bin_mask, kernel, iterations = 1)
            bin_mask = cv2.dilate(bin_mask, kernel, iterations = 5)
            cv2.imshow(name + " probablilities: ", np.concatenate((mask_images[name], bin_mask)))
            
            box_img, cnt = self.do_box_stuff(box_img, bin_mask, colormap[name])
            print okb("Found " + str(cnt) + " " + name + " pins")
            total_pins += cnt

        print okb("Total number of pins: " + str(total_pins))
        cv2.imshow("Box image ", box_img)
        cv2.waitKey(0);   



# Dispatcher
if __name__ == '__main__':
    class MyParser(OptionParser):
        def format_epilog(self, formatter):
            return self.epilog

    examples = ("")
    parser = MyParser(usage="Usage: pin_it.py <options>", epilog=examples)
    parser.add_option('-n', '--name', dest='input_name',
        help='specify input image file name (default: ../TestImgResized.jpg)', default="../TestImgResized.jpg")
    parser.add_option('--db', dest='db_root_path',
        help='specify path to folder with pin database (default: ../database)', default="../database")
    parser.add_option('--method', dest='pinit_method',
        help="specify pin_it method; available: 'G1D', 'G3D', 'GMM'", default='None', choices=['None', 'G1D', 'G3D', 'GMM'])
    parser.add_option('--denoise', dest='denoise_method',
        help="specify denoise method; available: 'BLT' for bilateral filter and 'NlMeans' for " +
             "non-local means denoise algorithm", default='NlMeans', choices=['NlMeans', 'BLT'])


    pin_it = PinIt(parser)
    pin_it.start()
