function [P, R, T] = icp(PC1, PC2, NItter)
    % PC2 is fitted to PC1
    % P is the combination of PC1 and PC2
    % https://www.cs.princeton.edu/~smr/papers/icpstability.pdf
    % https://www-new.comp.nus.edu.sg/~lowkl/publications/lowk_point-to-plane_icp_techrep.pdf
    
    KD = KDTreeSearcher(PC1.Points);
    
    R = eye(3);
    T = zeros(1, 3);
    err_last = 0;
    err_last2 = 0;
    
    for it = 1 : NItter
        [Idx, MD] = KD.knnsearch(PC2.Points);
        MIdx = [Idx, (1:numel(Idx))'];

        Sigma = std(MD);
        sel = MD < 1*Sigma;
        MIdx = MIdx(sel, :);

        Pd = PC1.Points(MIdx(:,1), :);
        Nd = PC1.Normals(MIdx(:,1), :);
        Ps = PC2.Points(MIdx(:,2), :);
        N = size(Pd, 1);
        
        [Ri, Ti] = icp_step(Ps, Pd, Nd);
        Ps = ((Ri*Ps') + repmat(Ti', 1,N))';

        err = icp_err(Ps, Pd, Nd);
        
        % Do the point transform
        PC2.Points = (Ri * PC2.Points')';
        PC2.Points = bsxfun(@plus, PC2.Points, Ti);
        PC2.Normals = (Ri * PC2.Normals')';

        R = R * Ri;
        T = T + Ti;
        
        if(abs(err-err_last) < 10^-5)
           break; 
        end
        if(abs(err-err_last2) < 10^-5)
           break; 
        end        
        err_last2 = err_last;
        err_last = err;
    end
    fprintf('ICP err: %1.5f \n', err);
    P = PC2;
end


function [R, T] = icp_step(Ps, Pd, Nd)
N = size(Ps, 1);

b = zeros(6, 1);
C = zeros(6, 6);

for i = 1 : N
    p = Ps(i, :);
    q = Pd(i, :);
    n = Nd(i, :);
    c = cross(p, n);
   
	C_ = [(c(1)*c(1)), (c(1)*c(2)), (c(1)*c(3)), (c(1)*n(1)), (c(1)*n(2)), (c(1)*n(3));...
          (c(2)*c(1)), (c(2)*c(2)), (c(2)*c(3)), (c(2)*n(1)), (c(2)*n(2)), (c(2)*n(3));...
          (c(3)*c(1)), (c(3)*c(2)), (c(3)*c(3)), (c(3)*n(1)), (c(3)*n(2)), (c(3)*n(3));...
          (n(1)*c(1)), (n(1)*c(2)), (n(1)*c(3)), (n(1)*n(1)), (n(1)*n(2)), (n(1)*n(3));...
          (n(2)*c(1)), (n(2)*c(2)), (n(2)*c(3)), (n(2)*n(1)), (n(2)*n(2)), (n(2)*n(3));...
          (n(3)*c(1)), (n(3)*c(2)), (n(3)*c(3)), (n(3)*n(1)), (n(3)*n(2)), (n(3)*n(3))...
       ];
	C = C + C_;
    
    d = dot(p-q, n);
	b_ = [(c(1) * d);...
          (c(2) * d);...
          (c(3) * d);...
          (n(1) * d);...
          (n(2) * d);...
          (n(3) * d)...
    ];
    b = b + b_;
end

X = C\-b;

t = X(1);
Rx = [1 0 0; 0 cos(t) -sin(t); 0 sin(t) cos(t)];
t = X(2);
Ry = [cos(t) 0 sin(t); 0 1 0; -sin(t) 0 cos(t)];
t = X(3);
Rz = [cos(t) -sin(t) 0; sin(t) cos(t) 0; 0 0 1];
R = Rz * Ry * Rx;
T = X(4:6)';
end

function [err] = icp_err(Ps, Pd, Nd)

N = size(Ps, 1);
err = 0;

for i = 1:N
   p = Ps(i, :);
   q = Pd(i, :);
   n = Nd(i, :);
   c = cross(p, n);
   
   err = err + (dot((p-q), n) + dot([0 0 0], n) + dot([0 0 0], c))^2;
end

end
