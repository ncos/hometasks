function [y1,y2,idx] = GetInliersRANSAC_temp(x1,x2)
%% Input:
% x1 and x2: Nx2 matrices whose row represents a correspondence.
%% Output:
% y1 and y2: Nix2 matrices whose row represents an inlier correspondence
%            where Ni is the number of inliers.
% idx: Nx1 vector that indicates ID of inlier y1.


%% Your Code goes here

% Initializations:
iter = 1000;
S_old = [];
len = length(x1);
eps = 2 * 1e-2; % Sweet spot e is about 2*1e-2
rands = 8;
n = 0;

for i = 1 : iter
    in = zeros(1,rands);
    j = 0;
    
    % Initializing x for Fundamental Matrix as zero
    x1F = zeros(rands,2);
    x2F = zeros(rands,2);
    
    while j < rands
        ran = floor(rand*len);
        if ~ismember(ran, in)
            in(j+1) = ran;
            j = j + 1;
        end
    end
    
    for j=1:rands
        x1F(j,:) = x1(in(j),:);
        x2F(j,:) = x2(in(j),:);
    end
    
    % Estimate the Fundamental Matrix:
    F = EstimateFundamentalMatrix(x1F, x2F);
    S = [];
    
    for j=1:len
        if norm([x2(j,:) 1] * F * [x1(j,:)'; 1]) < eps
            S = [S, j]; % Size of S changes after every loop; ignore.
        end
    end
    if n < length(S)
        n = length(S);
        S_old = S;
    end
    
end

% Initialize Inlier Correspondence as zero:
y1 = zeros(n,2);
y2 = zeros(n,2);

%% Spitting the inlier corr. data:
for i=1:n
    y1(i,:) = x1(S_old(i),:);
    y2(i,:) = x2(S_old(i),:);
    idx = S_old(i);
end
end
