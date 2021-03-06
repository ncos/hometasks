%% Run SFM (Visualizer)
% RANSAC might take time, just to test, change the image_count ...
...to 2 for one set of image.

clear; close all;

addpath('../Data');
image_count = 6;

% Read Images:
for i = 1:image_count
    filename = sprintf('%d.jpg', i);
    I{i} = imread(filename);
end

% Matching:
XM = []; YM = []; M = [];

for i = 1:image_count-1
    filename = sprintf('matching%d.txt', i);
    %x Matching, y Matching, Match Idx
    [xm, ym, m] = match_features(filename, image_count, i);
    M = [M;m]; 
    XM = [XM; xm]; 
    YM = [YM; ym];
end

% Play with the value of M and eps in GetInliersRANSAC function to ...
... change the number of corr. data.
visualize_matches(image_count, M, XM, YM); % Requires GetInliersRANSAC...
... and EstimateFundamentalMatrix.m


F = EstimateFundamentalMatrix(x1, x2);
E = EssentialMatrixFromFundamentalMatrix(F, K);
[Cset, Rset] = ExtractCameraPose(E);


Xset = cell(4,1);
for i=1 : 4
    Xset{i} = LinearTriangulation(K, [0; 0; 0], eye(3), Cset{i}, Rset{i}, x1, x2);
end

[Cset, Rset, Xset] = DisambiguateCameraPose(Cset, Rset, Xset);




