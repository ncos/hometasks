function [Norm] = getNormals(P, k, v)

m = size(P, 1);
KD = KDTreeSearcher(P);
Norm = zeros(m, 3);

[Pk_idx, ~]= KD.knnsearch(P, 'k', k);

for i = 1 : m
   idx = Pk_idx(i, :);
   p = P(i, :);
   K = P(idx', :);
   
   if nargin < 3
     v = p;
   end
   
   [V, ~] = eig(cov(K));
   norm_i = V(:, 1)';
   norm_i = norm_i/norm(norm_i);
   norm_i = norm_i * -sign(dot(norm_i, v));
   
   Norm(i, :) = norm_i;
end

end

