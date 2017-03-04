function res_pt = project(X, K, R, T, ks)
    in_pt = [R,T] * [X,zeros(length(X),1),ones(length(X),1)].';    
    in_pt_norm = in_pt(1:2,:) ./ repmat(in_pt(3,:), [2,1]);
    r2 = sum(in_pt_norm.^2);
    dPt = in_pt_norm .* repmat(1 + ks(1)*r2 + ks(2)*r2.^2, [2,1]);
    dPt(3,:) = 1;
    res_pt_ = K * dPt;
    res_pt = res_pt_(1:2,:) ./ repmat(res_pt_(3,:), [2,1]);
end