%========== plot the PR curve =============
close all;
%number of colors and dirs must be the same
dirs{1}='../data/baseline1tmp/';
dirs{2}='../data/baseline2tmp/';
dirs{3}='../data/mypbtmp/';
colors={'g','m', 'k'};

%number of names should be 4+number of dirs
%these will appear in the legend of the plot
%the first four are precomputed and not evaluated on the fly
names={'human','contours','canny','gpb','baseline1(sobel)','baseline2(canny)','pb-lite'};
% names={'human','contours','canny','gpb','baseline1(sobel)','baseline2(canny)'};
h=plot_eval_multidir(dirs,colors,names);
saveas(h,'PR_Curve','png');

%note the dotted lines are copied from figure 17 in the 2011 PAMI paper
%they are tuned/optimized to perform well than the baselines 
%given in the above code.