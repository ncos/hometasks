function [ Rs, ts ] = EstimateRt_linear( Hs, K )
% Linear parameter estimation of R and t
%
%   K: a camera calibration matrix. 3 x 3 matrix.
%
%   Hs: a homography from the world to images. 3 x 3 x N matrix, where N is 
%   the number of calibration images. 
%
%   Rs: rotation matrices. 3 x 3 x N matrix, where N is the number of calibration images. 
%
%   ts: translation vectors. 3 x 1 x N matrix, where N is the number of calibration images. 
%

Rs = zeros(3,3,length(Hs(1,1,:)));
ts = zeros(3,1,length(Hs(1,1,:)));

for i = 1 : length(Hs(1,1,:))
    display(i);
    H    = Hs(:,:,i);
    h1 = H(:,1);
    h2 = H(:,2);
    h3 = H(:,3);
    lambda1 = 1 / norm( K\h1 );
    lambda2 = 1 / norm( K\h2 );
    lambdat = (lambda1 + lambda2) / 2;
    r1 = lambda1 * (K\h1);
    r2 = lambda2 * (K\h2);
    r3 = cross(r1, r2);
    T = lambdat * (K\h3);
    R = [r1 r2 r3];
    Rs(:,:,i) = R;
    ts(:,:,i) = T;
end

end

