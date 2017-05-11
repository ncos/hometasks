function [Out] = downsample(Original, N)

k = 10;
mask = zeros(N, 1);

kmns_id = kmeans(Original.Points, k, 'Distance', 'sqeuclidean');
p_id = (1 : size(Original.Points, 1))';

i_start = 0;
i_end = 0;
for i = 1 : k
    I_i = p_id((kmns_id==i));
    n_i = size(I_i, 1);
    n_s = min(n_i, N / k);
    
    sel = randperm(n_i, n_s)';
    i_start = i_end + 1;
    i_end = i_start + n_s - 1;
    
    mask(i_start:i_end, :) = I_i(sel);
end

mask = mask(1 : i_end);
Out = struct('Points', Original.Points(mask, :));
Out.Colors = Original.Colors(mask, :); 
end
