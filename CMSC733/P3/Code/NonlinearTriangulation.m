function X = NonlinearTriangulation(K, C1, R1, C2, R2, x1, x2, X0)
%% Notes:
% (INPUT) C1 and R1: the 1st camera pose
% (INPUT) C2 and R2: the 2nd camera pose
% (INPUT) x1 and x2: two Nx2 matrices whose row represents correspondence 
%                    between the 1st and 2nd images where N is the number 
%                    of correspondences.
% (INPUT and OUTPUT) X: Nx3 matrix whose row represents 3D triangulated point.


    % Do we need this?
    x1 = [x1, ones(size(x1,1),1)];
    x2 = [x2, ones(size(x2,1),1)];

    P1 = K*R1*[eye(3), -C1];
    P2 = K*R2*[eye(3), -C2];

    opts = optimoptions(@lsqnonlin, 'Algorithm', 'levenberg-marquardt', 'TolX', 1e-64, 'TolFun', 1e-64, 'MaxFunEvals', 1e64, 'MaxIter', 1e64, 'Display', 'none');
    
    X = zeros(size(X0));
    for i = 1 : length(X0)
        X(i,:) = lsqnonlin(@Rerror, X0(i,:), [], [], opts, P1, P2, x1(i,:), x2(i,:));
    end
    
    function err = Rerror(X, P1, P2, x1, x2)
        X_aug = [X ones(size(X,1), 1)];
        x_p1 = bsxfun(@rdivide, (P1(1:2,:)*X_aug'), (P1(3,:)*X_aug'))';
        x_p2 = bsxfun(@rdivide, (P2(1:2,:)*X_aug'), (P2(3,:)*X_aug'))';
        err = [x1 - x_p1 x2 - x_p2];
    end
end

