%% Benchmarking Tool for Pb-lite
% Code by Nitin J. Sanket (nitinsan@terpmail.umd.edu) and Kiran Y. D. V.
% (kirany@ymd.edu)
% University of Maryland, College Park
% Code adapted from: Brown University's CS143 by James Hays

clc
clear all
close all

%% Setup all the necessary Paths
SetupPaths;

%% Sobel and Canny baselines are already computed!

% Save the pb-lite output images with the same name as that of original
% image but in png format, use the command
% imwrite(PbLiteOutput, 'Path/to/file/FileName.png'); where PbLiteOutput is
% the matrix obtained after PbLite save these images in '../data/mypb/' if
% you are running from stencil folder!

%% Evaluation against human
% Sobel and Canny evaluation has been performed

% Run benchmark for Pb-lite output
PbLiteBench;

%% Plot the PR Curve
PlotPR;