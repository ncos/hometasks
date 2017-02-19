%% Function to run all scripts - HW1 CMSC733
% Code by: Nitin J. Sanket, PhD in Computer Science Student,
% University of Maryland, College Park
% Code modified from: Jianbo Shi's source code


%% Setup paths
caliImgDir = './Imgs';  % the folder of your calibration images in a jpg format. e.g. './Imgs'
squareSize = 21.5;  % in mm

%% Initialize calibration process
% This part takes a long time (30-45mins sometimes) to run so don't think
% your PC has crashed! Run this once and save the matrices
% this part needs the computer vision toolbox, code will error out if not available
if(~exist('Data.mat','file'))
    [x, X, imgs] = InitCalibration(caliImgDir, squareSize);
    save('Data.mat');
else
    load('Data.mat');
end

%% Estimate an intrinsic parameter
[K, Hs] = EstimateK_linear(x, X);

%% Estimate an extrinsic parameter
[Rs, ts] = EstimateRt_linear(Hs, K);
cprintf('blue', 'Reprojection error per measurement: %f pixel(s)\n',...
    GeoError(x, X, [0 0], K, Rs, ts)/(size(x, 1)*size(x, 3)));

%% Distortion is assumed to be zero
ks = [0, 0]';

%% Minimize reprojection errors
[ ks_opt, K_opt, Rs_opt, ts_opt ] = MinGeoError( x, X, ks, K, Rs, ts );
cprintf('green', 'Reprojection error per measurement: %f pixel(s)\n',...
    GeoError(x, X, ks_opt, K_opt, Rs_opt, ts_opt)/(size(x, 1)*size(x, 3)));

%% Evaluate parameters
Evaluate;
