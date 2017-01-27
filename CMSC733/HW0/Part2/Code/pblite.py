#!/usr/bin/python

import cv2
import numpy as np
from multiprocessing import Pool
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


# Multiprocessing workarounds
def chi_sq_comp(arg):
    tmp = np.zeros_like(np.float32(arg[2][0]))
    for i, slice_ in enumerate(arg[2]):
        print wrn("\tchi-square: (" + str(arg[0]) + " " + str(arg[1]) + " " + str(i) + ")")
        g_i = signal.convolve2d(slice_, arg[3], boundary='symm', mode='same')
        h_i = signal.convolve2d(slice_, arg[4], boundary='symm', mode='same')
        tmp += 0.5 * (g_i - h_i)**2 / (g_i + h_i + 10**(-6))
    return tmp

def frange(start, stop, step):
    i = start
    while i < stop:
        yield i
        i += step

# Core
class PBlite:
    def __init__(self, arg_parser):
        (self.options, self.args) = arg_parser.parse_args()
        if (self.options.input_name == 'None'):
            arg_parser.print_help()
            exit(1)

        self.rgb_image = cv2.imread(self.options.input_name, cv2.CV_LOAD_IMAGE_COLOR)
        if (self.rgb_image is None):
            print err("No image at " + self.options.input_name + " was found!")
            arg_parser.print_help()
            exit(1)

        self.im_height, self.im_width, channels = self.rgb_image.shape
        self.im_name = os.path.basename(self.options.input_name)
        self.lab_image = cv2.cvtColor(self.rgb_image, cv2.COLOR_BGR2LAB)
        self.grayscale = cv2.cvtColor(self.rgb_image, cv2.COLOR_BGR2GRAY)
        self.l_channel, self.a_channel, self.b_channel = cv2.split(self.lab_image)

        print wrn("Computing Filters...")
        self.hdmasks   = self.get_hdmasks()
        self.gaussians = self.get_gaussians()
        self.LM = self.get_LM()
        self.S  = self.get_S()
        self.MR = self.get_MR()
        self.filters = []


    def start(self):
        # Pick the filters
        self.add_filters(self.gaussians)
        self.add_filters(self.LM)
        self.add_filters(self.S)
        #self.add_filters(self.MR)

        # Texton map
        print wrn("Computing Texton Maps...")
        a_filt = np.concatenate(tuple([cv2.filter2D(self.a_channel, -1, kernel).reshape(-1,1) \
                                for kernel in self.filters]), axis = 1)
        b_filt = np.concatenate(tuple([cv2.filter2D(self.b_channel, -1, kernel).reshape(-1,1) \
                                for kernel in self.filters]), axis = 1)

        criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 10, 1.0)
        a_rets, a_labels, a_centers = cv2.kmeans(np.float32(a_filt), self.options.K, criteria, 10, cv2.KMEANS_RANDOM_CENTERS)
        b_rets, b_labels, b_centers = cv2.kmeans(np.float32(b_filt), self.options.K, criteria, 10, cv2.KMEANS_RANDOM_CENTERS)
        TMa = a_labels.reshape(self.im_height, self.im_width)         
        TMb = b_labels.reshape(self.im_height, self.im_width)        
        cv2.imwrite(os.path.join(self.options.tmap_path, "TextonMap_A_" + self.im_name), self.scale(TMa) * 255)
        cv2.imwrite(os.path.join(self.options.tmap_path, "TextonMap_B_" + self.im_name), self.scale(TMb) * 255)
                
        # Brightness map
        BM = np.int_(np.floor(self.l_channel * (self.options.bbins / float(np.max(self.l_channel)))))
        cv2.imwrite(os.path.join(self.options.bmap_path, "BrightnessMap_" + self.im_name), self.scale(BM) * 255)
        
        # Color map
        CMa = np.int_(np.floor(self.a_channel * (self.options.cbins / float(np.max(self.a_channel)))))
        CMb = np.int_(np.floor(self.b_channel * (self.options.cbins / float(np.max(self.b_channel)))))
        cv2.imwrite(os.path.join(self.options.cmap_path, "ColorMap_A_" + self.im_name), self.scale(CMa) * 255)
        cv2.imwrite(os.path.join(self.options.cmap_path, "ColorMap_B_" + self.im_name), self.scale(CMb) * 255)

        # Sobel PB
        sobel_pb = self.get_sobel_pb()
        cv2.imwrite(os.path.join(self.options.sobel_path, "SobelPb_" + self.im_name), sobel_pb * 255)

        # Canny PB
        canny_pb = self.get_canny_pb()
        cv2.imwrite(os.path.join(self.options.canny_path, "CannyPb_" + self.im_name), canny_pb * 255)

        # tg
        print wrn("Computing Texture Gradients...")
        tga = self.chi_sq(TMa)
        tgb = self.chi_sq(TMb) 
        tg_img = np.concatenate(tuple([self.scale(im) for im in (tga + tgb)]), axis=1)
        cv2.imwrite(os.path.join(self.options.tg_path, "tg_" + self.im_name), tg_img * 255)

        # bg
        print wrn("Computing Brightness Gradients...")
        bg = self.chi_sq(BM) 
        bg_img = np.concatenate(tuple([self.scale(im) for im in bg]), axis=1)
        cv2.imwrite(os.path.join(self.options.bg_path, "bg_" + self.im_name), bg_img * 255)

        # cg
        print wrn("Computing Color Gradients...")
        cga = self.chi_sq(CMa)
        cgb = self.chi_sq(CMb)
        cg = [a + b for a, b in zip(cga, cgb)]
        cg_img = np.concatenate(tuple([self.scale(im) for im in cg]), axis=1)
        cv2.imwrite(os.path.join(self.options.cg_path, "cg_" + self.im_name), cg_img * 255)


        # PB Lite
        tga_ = np.mean(tga, axis=0) / np.mean(tga)
        tgb_ = np.mean(tgb, axis=0) / np.mean(tgb)
        bg_  = np.mean(bg,  axis=0) / np.mean(bg)
        cg_  = np.mean(cg,  axis=0) / np.mean(cg)
        sbl_ = sobel_pb * np.sum(sobel_pb)
        cny_ = canny_pb * np.sum(canny_pb)

        pb_lite = self.scale((tga_ + tgb_ + bg_ + cg_) * (sbl_ + cny_))
        cv2.imwrite(os.path.join(self.options.pblite_path, "PbLite_" + self.im_name), pb_lite * 255)



    def chi_sq(self, mat):
        bins = np.max(mat) + 1
        mat_slices = [np.float32(mat == i) for i in xrange(bins)]

        args = []
        for scale_id, scaled in enumerate(self.hdmasks.values()):
            for rot_id, [L, R] in enumerate(scaled.values()):
                args.append([scale_id, rot_id, mat_slices, L, R])
        pool = Pool(processes=len(args))
        return pool.map(chi_sq_comp, args)


    def get_sobel_pb(self):
        sobel = cv2.Sobel(self.grayscale, -1, 1, 1, ksize=5)
        ret = np.zeros_like(sobel)
        for th in frange(0.08 * 255, 0.3 * 255, 0.02 * 255):
            ret_, thresh = cv2.threshold(sobel, th, 255, cv2.THRESH_BINARY)
            ret += thresh
        return self.scale(ret)


    def get_canny_pb(self):
        ret = np.zeros_like(self.grayscale)
        for th in frange(0.1 * 255, 0.7 * 255, 0.1 * 255):
            for sigma in range(1, 4):
                canny = cv2.Canny(self.grayscale, th/2, th, sigma)
                ret += canny
        return self.scale(ret)


    def add_filters(self, filters):
        self.filters += filters


    # Filter generation
    def gkern(self, nsig):
        inp = np.zeros((self.options.filter_size, self.options.filter_size))
        inp[self.options.filter_size // 2, self.options.filter_size // 2] = 1.0
        return fi.gaussian_filter(inp, nsig)

    def scale(self, img):
        min_, max_, min_loc_, max_loc_ = cv2.minMaxLoc(img)
        if (max_ == min_): return img - min_
        return (img - min_) / float(max_ - min_)

    def get_gaussians(self):
        gaussians = []
        xsobel = np.array([[-1.0, 0.0, 1.0], [-2.0, 0.0, 2.0], [-1.0, 0.0, 1.0]])
        ysobel = np.array([[-1.0, -2.0, -1.0], [0.0, 0.0, 0.0], [1.0, 2.0, 1.0]])
        for scale in xrange(self.options.scales):
            sigma = (scale + 1) * math.sqrt(2)
            gaussian = self.gkern(sigma)

            gx = signal.convolve2d(gaussian, xsobel, boundary='symm', mode='same')
            gy = signal.convolve2d(gaussian, ysobel, boundary='symm', mode='same')
            for rot in xrange(self.options.rotations):
                angle = (rot + 1) * 2.0 * math.pi / float(self.options.rotations)
                g_image = gx * math.cos(angle) + gy * math.sin(angle)
                gaussians.append(g_image)

        g_set = np.concatenate(tuple([self.scale(im) for im in gaussians]), axis=1)
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


    def gkern_derivatives(self, nsig1, nsig2):
        base = np.array(xrange(self.options.filter_size)) - (self.options.filter_size // 2)
        gx = np.exp(np.square(base) / float(-2.0 * nsig1)) / float(math.sqrt(2.0 * math.pi * nsig1))
        gy = np.exp(np.square(base) / float(-2.0 * nsig2)) / float(math.sqrt(2.0 * math.pi * nsig2))
        gyy = base * gy / float(-nsig2)
        gyyy = gy / float(-nsig2) + base * gyy / float(-nsig2)
        return (np.matmul(np.resize(gy, (gy.size, 1)), np.resize(gx, (1, gx.size))),
                np.matmul(np.resize(gyy, (gyy.size, 1)), np.resize(gx, (1, gx.size))),
                np.matmul(np.resize(gyyy, (gyyy.size, 1)), np.resize(gx, (1, gx.size))))

    def LoG(self, nsig):
        base = (np.array(xrange(self.options.filter_size)) - (self.options.filter_size // 2))
        base2 = base**2
        base2x2 = np.array([e + base2 for e in base2]) 
        gx = np.exp(np.square(base) / float(-2.0 * nsig)) 
        gaussian = np.matmul(np.resize(gx, (gx.size, 1)), np.resize(gx, (1, gx.size)))
        norm = (base2x2 - 2.0 * nsig) / float(nsig**2)
        return norm * gaussian

    def get_LM(self):
        lm_filters = []
        for scale in xrange(3):
            nsig2 = 2**(scale + 2)
            nsig1 = 9 * nsig2
            g, gx, gxx = self.gkern_derivatives(nsig1, nsig2)
            for rot in xrange(6):
                M = cv2.getRotationMatrix2D((self.options.filter_size // 2, self.options.filter_size // 2), \
                                             int((rot + 1) * 30), 1)
                gxR = cv2.warpAffine(gx, M, (self.options.filter_size, self.options.filter_size))
                gxxR = cv2.warpAffine(gxx, M, (self.options.filter_size, self.options.filter_size))
                lm_filters.append(gxR)
                lm_filters.append(gxxR)
            
        for scale in xrange(4):
            lm_filters.append(self.LoG(2**(scale / 2.0)))

        for scale in xrange(4):
            lm_filters.append(self.LoG(9.0 * 2**(scale / 2.0)))
    
        for scale in xrange(4):
            lm_filters.append(self.gkern(2**scale))
 
        lm_image = np.concatenate(tuple([self.scale(im) for im in lm_filters]), axis=1)
        cv2.imwrite(os.path.join(self.options.lm_path, "lm.jpg"), lm_image * 255)
        return lm_filters           


    def get_S(self):
        s_filters = []
        base = (np.array(xrange(self.options.filter_size)) - (self.options.filter_size // 2))
        r = np.sqrt(np.array([e + base**2 for e in base**2]))
        magic = [(2,1), (4,1), (4,2), (6,1), (6,2), (6,3), (8,1), (8,2), (8,3), (10,1), (10,2), (10,3), (10,4)]
        for sigma, tau in magic:
            s_flt = np.cos(r * math.pi * tau / float(sigma)) * np.exp(r**2 / float(-2.0 * sigma**2))
            s_flt -= np.mean(s_flt)
            s_flt /= np.sum(np.abs(s_flt))
            s_filters.append(s_flt)

        s_image = np.concatenate(tuple([self.scale(im) for im in s_filters]), axis=1)
        cv2.imwrite(os.path.join(self.options.s_path, "s.jpg"), s_image * 255)
        return s_filters  


    def get_MR(self):
        mr_filters = []
        for scale in xrange(3):
            nsig2 = (scale + 1)**2
            nsig1 = 9 * nsig2
            g, gx, gxx = self.gkern_derivatives(nsig1, nsig2)
            for rot in xrange(6):
                M = cv2.getRotationMatrix2D((self.options.filter_size // 2, self.options.filter_size // 2), \
                                             int((rot + 1) * 30), 1)
                gxR = cv2.warpAffine(gx, M, (self.options.filter_size, self.options.filter_size))
                gxxR = cv2.warpAffine(gxx, M, (self.options.filter_size, self.options.filter_size))
                mr_filters.append(gxR)
                mr_filters.append(gxxR)

        mr_filters.append(self.gkern(8))
        mr_filters.append(self.LoG(64))
        mr_image = np.concatenate(tuple([self.scale(im) for im in mr_filters]), axis=1)
        cv2.imwrite(os.path.join(self.options.mr_path, "mr.jpg"), mr_image * 255)
        return mr_filters  


# Dispatcher
if __name__ == '__main__':
    class MyParser(OptionParser):
        def format_epilog(self, formatter):
            return self.epilog

    examples = ("")
    parser = MyParser(usage="Usage: pblite.py <options>", epilog=examples)
    parser.add_option('-n', '--name', dest='input_name',
        help='specify input image file name (default: ../TestImages/1.jpg)', default="../TestImages/1.jpg")

    parser.add_option('--filter_size', dest='filter_size',
        help='specify the size of filters to use (odd number, default: 49)', type="int", default=49)
    parser.add_option('--scales', dest='scales',
        help='number of scales for gaussians and etc. (default: 2)', type="int", default=2)
    parser.add_option('--rotations', dest='rotations',
        help='number of rotations for gaussians and etc. (default: 8)', type="int", default=8)
    parser.add_option('--tbins', dest='K',
        help='number of bins for texture (default: 16)', type="int", default=16)
    parser.add_option('--bbins', dest='bbins',
        help='number of bins for brightness (default: 100)', type="int", default=100)
    parser.add_option('--cbins', dest='cbins',
        help='number of bins for color (default: 100)', type="int", default=100)

    parser.add_option('--path_gaussian', dest='gaussian_path',
        help='specify path to folder to save gaussian database (default: ../Images)', default="../Images")
    parser.add_option('--path_hdmask', dest='hdmask_path',
        help='specify path to folder to save hdmask database (default: ../Images)', default="../Images")
    parser.add_option('--path_lmfilt', dest='lm_path',
        help='specify path to folder to save LM filters (default: ../Images)', default="../Images")
    parser.add_option('--path_sfilt', dest='s_path',
        help='specify path to folder to save S filters (default: ../Images)', default="../Images")
    parser.add_option('--path_mrfilt', dest='mr_path',
        help='specify path to folder to save MR filters (default: ../Images)', default="../Images")
    parser.add_option('--path_tmaps', dest='tmap_path',
        help='specify path to folder to save Texton Maps (default: ../Images/TextonMap)', default="../Images/TextonMap")
    parser.add_option('--path_tg', dest='tg_path',
        help='specify path to folder to save Texture Gradients (default: ../Images/tg)', default="../Images/tg")
    parser.add_option('--path_bmaps', dest='bmap_path',
        help='specify path to folder to save Brightness Maps (default: ../Images/BrightnessMap)', \
        default="../Images/BrightnessMap")
    parser.add_option('--path_bg', dest='bg_path',
        help='specify path to folder to save Brightness Gradients (default: ../Images/bg)', default="../Images/bg")
    parser.add_option('--path_cmaps', dest='cmap_path',
        help='specify path to folder to save Color Maps (default: ../Images/ColorMap)', default="../Images/ColorMap")
    parser.add_option('--path_cg', dest='cg_path',
        help='specify path to folder to save Color Gradients (default: ../Images/cg)', default="../Images/cg")
    
    parser.add_option('--path_sobel', dest='sobel_path',
        help='specify path to folder to save Sobel Pb (default: ../Images/SobelPb)', default="../Images/SobelPb")
    parser.add_option('--path_canny', dest='canny_path',
        help='specify path to folder to save Canny Pb (default: ../Images/CannyPb)', default="../Images/CannyPb")
    parser.add_option('--path_pblite', dest='pblite_path',
        help='specify path to folder to save Pb Lite output (default: ../Images/PbLite)', default="../Images/PbLite")

    pblite = PBlite(parser)
    pblite.start()
