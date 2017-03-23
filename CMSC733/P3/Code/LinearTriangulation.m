function [X] = LinearTriangulation(K, C1, R1, C2, R2, x1, x2)
%% Inputs:
% C1 and R1: the 1st camera pose
% C2 and R2: the 2nd camera pose
% x1 and x2: two Nx2 matrices whose row represents correspondence between the 
%            1st and 2nd images where N is the number of correspondences.
%% Outputs: 
% X: Nx3 matrix whose row represents 3D triangulated point.

%% Your Code goes here


P1 = K * R1 * [eye(3) -C1];
P2 = K * R2 * [eye(3) -C2];
N = size(x1, 1);
X = zeros(N, 3);

for i = 1 : N
  A = [vec2skew([x1(i,:) 1])*P1;...
       vec2skew([x2(i,:) 1])*P2];
  [~, ~, V] = svd(A);
  X_h = V(:,end)/V(end,end);
  X(i,:) = X_h(1:3)';
end

function C = vec2skew(v)
  C = [0  -v(3) v(2);...
       v(3)  0 -v(1);...
       -v(2) v(1) 0];
end
end
