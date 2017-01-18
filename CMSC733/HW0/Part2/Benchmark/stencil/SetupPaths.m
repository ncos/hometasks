%=== set up ===
addpath(genpath('./bench/'));
testdir='../data/testset/'; %contains only 10 test images

files=dir([testdir,'*.jpg']); %all jpgs

groundtruthdir='../BSDS500/BSDS500/data/groundTruth/test/'; %need this for benchmark

%create dirs to store pb results
baselinedir1='../data/sobel_baseline/'; %thresholded sobel or canny edge
baselinedir2='../data/canny_baseline/'; %thresholded sobel or canny edge
mypbdir='../data/mypb/';         %your implementation
if (~exist(baselinedir1,'dir')) mkdir(baselinedir1); end
if (~exist(baselinedir2,'dir')) mkdir(baselinedir2); end
if (~exist(mypbdir,'dir')) mkdir(mypbdir); end