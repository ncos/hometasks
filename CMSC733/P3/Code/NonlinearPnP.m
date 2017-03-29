function [C, R] = NonlinearPnP(X, x, K, C, R)
%% Inputs and Outputs
% X and x: Nx3 and Nx2 matrices whose row represents correspondences 
%                  between 3D and 2D points, respectively.
% K: intrinsic parameter
% C and R: for pose


% x = [x, ones(size(x,1),1)];
% X = [X, ones(size(X,1),1)];

    opts = optimoptions(@lsqnonlin, 'Algorithm', 'levenberg-marquardt', 'MaxIter', 1e3, 'Display', 'none');

    q0 = R2q(R);
    qC0 = [q0; C];

    qC = lsqnonlin(@Rerror_q, qC0, [], [], opts, X, x, K);
    q = qC(1:4);
    R = q2R(q);
    C = qC(5:7);
end


function err = Rerror_q(qC, X, x, K)
    P = K * q2R(qC(1:4)) * [eye(3), -qC(5:7)];
    projection = bsxfun(@rdivide, P(1:2, :)*X', P(3,:)*X');
    err = (x(:, 1:2)' - projection)';
end

