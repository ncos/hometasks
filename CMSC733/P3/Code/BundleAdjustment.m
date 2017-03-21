function [Cset, Rset, Xset] = BundleAdjustment(Cset, Rset, X, K, traj, V)
% 1st level wrapper function for running sba
%% Inputs:
% (K, Cr_set, Rr_set, X3D, ReconX, V_bundle, Mx_bundle, My_bundle)
% K - Camera callibration Matrix
% Cr_set - Set of Camera Center Positions
% Rr_set - Set of Rotation matrices
% Pose is defined as P = KR[I,-C]
% X3D - 3D points corresponding to all possible feature points
% Xin - An indicator vector to indicate if a particular point has a
%       valid 3D triangulation associated with it
% V - Visibility matrix (NxM) => N features, M poses
% Mx - Set of pixel x-coordinates for each feature => NxM
% My - Set of pixel y-coordinates for each featuer => NxM
%% Outputs
% Updated Cr_set, Rr_set and X3D 

%% Your code goes here

end
