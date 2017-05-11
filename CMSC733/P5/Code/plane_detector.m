function [ids] = plane_detector(Cloud, N, eps)

m = size(Cloud.Points, 1);
mask = false(m, 1);

best_count = 0;
for i = 1 : N
    sel = randperm(m, 3); 
    P = Cloud.Points(sel, :);
    
    % Normal to plane:
    n = cross(P(1,:) - P(2,:), P(1,:) - P(3,:));
    n = n / norm(n);
    p = P(1, :);

    % fixup (adjust the direction of the normal)
    n = n * sign(-dot(n, p)); 
    
    % Find all points on this plane
    for k = 1 : m
       x = Cloud.Points(k, :);

       % distance from point to plane
       d = abs(dot(x - p, n)); 
       mask(k) = d < eps;
    end
    
    count = sum(mask);
    if count > best_count
        best_count = count;
        best_mask = mask;
    end     
end

ids = (1 : m)';
ids = ids(best_mask);
end

