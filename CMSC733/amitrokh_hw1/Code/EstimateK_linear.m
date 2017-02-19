function [K, Hs] = EstimateK_linear(x, X)
% Linear parameter estimation of K
%
%   x:  2D points. n x 2 x N matrix, where n is the number of corners in
%   a checkerboard and N is the number of calibration images
%       
%   X:  3D points. n x 2 matrix, where n is the number of corners in a
%   checkerboard and assumes the points are on the Z=0 plane
%
%   imgs: calibration images. N x 1 cell, where N is the number of calibration images
%
%   K: a camera calibration matrix. 3 x 3 matrix.
%
%   Hs: a homography from the world to images. 3 x 3 x N matrix, where N is 
%   the number of calibration images. You can use est_homography.m to
%   estimate homographies.
%


% from the paper
vij = @(i,j,H) [ H(1,i)*H(1,j)
                 H(1,i)*H(2,j) + H(2,i)*H(1,j)
                 H(2,i)*H(2,j)
                 H(3,i)*H(1,j) + H(1,i)*H(3,j)
                 H(3,i)*H(2,j) + H(2,i)*H(3,j)
                 H(3,i)*H(3,j) ];

V = [];
Hs = [];
for i = 1 : length(x)
    H = est_homography(x[i, :, 1], x[i, :, 2], X[:, 1], X[:, 2]);
    Hs = [Hs; H];
    G = [vij(1,2,H)'; (vij(1,1,H)-vij(2,2,H))'];
    V = [V; G];
end

[~,~,RIGHT] = svd( V );
b = RIGHT(:, end);

% build A
v0 = ( b(2)*b(4)-b(1)*b(5) ) / ( b(1)*b(3)-b(2)^2 );
lambda = b(6) - ( b(4)^2 + v0*(b(2)*b(4)-b(1)*b(5)) ) / b(1);

alpha = sqrt( lambda / b(1) );
beta = sqrt( lambda*b(1) / (b(1)*b(3)-b(2)^2) );
if imag(beta) ~= 0
    beta = 0.1;
end
gamma = -b(2)*alpha^2*beta / lambda;
u0 = gamma*v0 / beta - b(4)*alpha^2 / lambda;


K = [alpha  gamma  u0
       0    beta   v0
       0      0    1 ];
end
