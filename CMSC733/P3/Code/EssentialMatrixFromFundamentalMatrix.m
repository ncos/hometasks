function E = EssentialMatrixFromFundamentalMatrix(F,K)
%% Inputs:
% K: 3x3 camera intrinsic parameter
% F: fundamental matrix
%% Outputs:
% E: 3x3 essential matrix with singular values (1,1,0)

%% Your Code goes here

[U, ~, V] = svd(K' * F * K);

D = eye(3);
D(3,3) = 0;
E = U*D*V';
end
