function [Out] = trim(Cloud, r)
    D = Cloud.Points - repmat(mean(Cloud.Points), [size(Cloud.Points, 1), 1]);
    D = sqrt(sum(D.^2, 2));

    mask = D < r;
    Out.Points = Cloud.Points(mask, :);
    Out.Colors = Cloud.Colors(mask, :); 
end

