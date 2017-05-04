clear; clc

load('../Data/Data.mat');


N = numel(ts);
V = zeros(N, 2);
X =  [x, y, ts];

L = 3; 
dt = 1000;


display(N);

for i = 1 : N
    V(i, :) = NaN(1, 2);

    mask_ts = (ts <= ts(i) + dt) & (ts >= ts(i) - dt);
    mask_pol = (pol == pol(i));
    mask_x = (x <= x(i) + L) & (x >= x(i) - L);
    mask_y = (y <= y(i) + L) & (y >= y(i) - L);
    mask = mask_x & mask_y & mask_ts & mask_pol;    
    
    [err, pl_0] = get_pl(X(mask, :));
    if err
        continue;
    end    


    th1 = 1e-5; th2 = 0.5;
    diff = 1e7;
    
    while diff > th1
        outliers = abs(get_pl_err(pl_0, X(mask, :))) > th2;    
        mask(mask) = ~outliers;
            
        [err, pl] = get_pl(X(mask, :));
            
        if err
            break;
        end    
            
        diff = norm(pl_0 - pl);           
        pl_0 = pl;
    end

    if err
        continue;
    end

    V(i, :) = -[pl(3)/pl(1), pl(3)/pl(2)]* 1e6;    
end


figure;
toShow = ~isnan(V(:, 1)) & ~isnan(V(:, 2)) & (sqrt(sum(V.^2, 2)) > 10^2) & (sqrt(sum(V.^2, 2)) < 5*10^3);
quiver(x(toShow), y(toShow), V(toShow, 1), V(toShow, 2));



function [err, pl] = get_pl(X)
  coeff = pca(X);
  pl = NaN(3, 1);
  err = true;
  if size(coeff, 2) >= 3
    pl = coeff(:, 3);
    err = false;
  end
end


function ret = get_pl_err(pl, X)
  ret = bsxfun(@minus, X, mean(X, 1)) * pl;
end



