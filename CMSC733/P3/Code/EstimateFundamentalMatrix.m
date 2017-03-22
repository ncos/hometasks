function F = EstimateFundamentalMatrix(X1,X2)

%% Input
% X1 and X2: Nx2 matrices whose row represents a correspondence

%% Output
% F: 3x3 Fundamental Matrix of Rank 2

% Solving [Ax = 0] linear least squares to estimate the fundamental matrix.
% [X2(1) X2(2) 1] * F [X1(1) X1(2) 1]^T = 0

x1 = X1(:,1);
y1 = X1(:,2);
x2 = X2(:,1);
y2 = X2(:,2);

A = [];

for i = 1:size(X1,1)
   A(i,:) = [x1(i) * x2(i), x1(i) * y2(i), x1(i) ,y1(i) * x2(i), y1(i) * y2(i), y1(i), x2(i), y2(i), 1];
end

[~, ~,V] = svd(A);
F = reshape(V(:,9), 3, 3)';

% Rank 2 Fundamental Matrix:
[U,S,V] = svd(F,0);
F = U * diag([S(1,1), S(2,2), 0]) * V'; % U * (diag(S)) * V';
