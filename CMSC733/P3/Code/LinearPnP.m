function [C, R] = LinearPnP(X, x, K)
%% Inputs
% X and x: Nx3 and Nx2 matrices whose row represents correspondences between 
%          3D and 2D points, respectively.
% K: intrinsic parameter
%% Output
% C and R: camera pose (C;R)

N = size(x, 1);
x = [x, ones(size(x, 1), 1)];
X = [X, ones(size(X, 1), 1)];

Xdiag = zeros(3, 12);
A = [];
for i=1 : N
    Xdiag(1, 1:4) = X(i,:);
    Xdiag(2, 5:8) = X(i,:);
    Xdiag(3, 9:12) = X(i,:);
    A = [A; vec2skew(inv(K) * x(i,:)') * Xdiag];
end

[~, ~, V] = svd(A);
Pvec = V(:, end);
P = [Pvec(1:4)'; Pvec(5:8)'; Pvec(9:12)'];

RC = P;
[U, D, V] = svd(RC(:,1:3));
R = sign(det(U*V'))*U*V';
C = -R'*RC(:,4)/D(1,1);


function C = vec2skew(v)
  C = [0  -v(3) v(2);...
       v(3)  0 -v(1);...
       -v(2) v(1) 0];
end
end

