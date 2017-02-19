function [error, f] = GeoError(x, X, ks, K, Rs, ts)
% Measure a geometric error
%
%   x:  2D points. n x 2 x N matrix, where n is the number of corners in
%   a checkerboard and N is the number of calibration images
%       
%   X:  3D points. n x 2 matrix, where n is the number of corners in a
%   checkerboard and assumes the points are on the Z=0 plane
%
%   K: a camera calibration matrix. 3 x 3 matrix.
%
%   Rs: rotation matrices. 3 x 3 x N matrix, where N is the number of calibration images. 
%
%   ts: translation vectors. 3 x 1 x N matrix, where N is the number of calibration images. 
%
%   ks: radial distortion parameters. 2 x 1 matrix, where ks(1) = k_1 and
%   ks(2) = k_2.
%


f = cell(2, length(x), 1);
for i = 1:length(x)
    x_y = project(X, K, Rs(i), ts(i), ks) - x(i);
    f{1, i} = x_y(:, 1);
    f{2, i} = x_y(:, 2);
end
f = cell2mat(f);
error = sum(f.^2);
end


function res_pt = project(X, K, R, T, ks)
    cPt = [R,T] * X;
    nPt = cPt(1:2,:) ./ repmat(cPt(3,:), [2,1]);
    % distance of point from optical centre
    r2 = sum(nPt.^2);
    % apply radial distortion
    dPt = nPt .* repmat(1 + ks(1)*r2 + ks(2)*r2.^2, [2,1]);
    % convert to homogeneous coords
    dPt(3,:) = 1;
    % apply intrinsic transformation
    res_pt = unhomo(K * dPt);
end


function x = unhomo( x )
    if size( x, 1 ) == 3
        x = x ./ repmat( x(3,:), [3,1] );
    elseif size( x, 1 ) == 4
        x = x ./ repmat( x(4,:), [4,1] );
end
