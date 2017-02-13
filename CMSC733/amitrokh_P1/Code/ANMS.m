function [ x, y ] = ANMS(im, Nbest)
% ANMS
C = cornermetric(im);
Cmax = imregionalmax(C);
[y_max, x_max] = find(Cmax);
Nstrong = sum(sum(Cmax));
r = Inf(Nstrong, 1);

for i = 1 : Nstrong
    for j = 1 : Nstrong
        if C(y_max(j), x_max(j)) > C(y_max(i), x_max(i))
            ED = (y_max(j) - y_max(i))^2 + (x_max(j) - x_max(i))^2;
            if ED < r(i)
                r(i) = ED;
            end
        end
    end
end

[~, sortedIndicies] = sort(r,'descend');
bestIndicies = sortedIndicies(1:Nbest);
x = x_max(bestIndicies);
y = y_max(bestIndicies);
end

