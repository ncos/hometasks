function [C,R] = NonlinearPnP(X, x, K, C, R)
%% Inputs and Outputs
% X and x: Nx3 and Nx2 matrices whose row represents correspondences 
%                  between 3D and 2D points, respectively.
% K: intrinsic parameter
% C and R: for pose

opts = optimoptions(@lsqnonlin, 'Algorithm', 'levenberg-marquardt', 'MaxIter', 1e3, 'Display', 'none');

%% Your code goes here


end
