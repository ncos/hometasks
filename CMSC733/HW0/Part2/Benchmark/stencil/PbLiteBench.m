mypbtmpdir = '../data/mypbtmp/';
if (~exist(mypbtmpdir,'dir')) mkdir(mypbtmpdir); end
nthresh = 10;
tic;
boundaryBench(testdir, groundtruthdir, mypbdir, mypbtmpdir, nthresh);
toc;
