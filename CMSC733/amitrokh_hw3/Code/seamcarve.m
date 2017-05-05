im = imread('../Images/SeamCarving.jpg')
carved = seamcarve_(im, 100, 0);
imshow(carved);


function carved = seamcarve_(im, horizontal, vertical)

carved = im;
for h = 1:horizontal
    carved = horizontalseam(carved);
    carved = verticalseam(carved);
    carved = verticalseam(carved);
end


%for verticalSeam = 1:vertical
%    carved = verticalseam(carved);
%end
%for horizontalSeam = 1:horizontal
%    carved = horizontalseam(carved);
%end

end

function [gMag] = energy1(im)
[gr, ~] = imgradient(im(:,:,1));
[gg, ~] = imgradient(im(:,:,2));
[gb, ~] = imgradient(im(:,:,3));
gMag = sqrt(gr.^2 + gg.^2 + gb.^2);    
end

function [gMag] = energy2(im)
im = rgb2hsv(im);
[gh, ~] = imgradient(im(:,:,1));
[gv, ~] = imgradient(im(:,:,3));
gMag = sqrt(gh.^2 + gv.^2);    
end

function carved = verticalseam(im)
[m, n, ~] = size(im);
gMag = energy1(im);
seam = findverticalseam(gMag, m, n);
carved = zeros(m,n-1,3,'uint8');


for y = 1:m
    row = im(y,:,:);
    row(:,seam(y),:) = [];
    carved(y,:,:) = row(:,:,:);
end

end

function seam = findverticalseam(gMag, m, n)
for y = 1:(m-1)
    gMag(y+1,1) = gMag(y+1,1) + min([gMag(y,1), gMag(y,2)]);
    for x = 2:(n-1)
        gMag(y+1,x) = gMag(y+1,x) + min([gMag(y,x-1), gMag(y,x), gMag(y,x+1)]);
    end
    gMag(y+1,n) = gMag(y+1,n) + min([gMag(y,n), gMag(y,n-1)]);
end

seam = zeros(1,m);
[~, ind] = min(gMag(m));
seam(m) = ind;
for ind = m:-1:2
    tmp = seam(ind);
    if tmp == 1
        a = gMag(ind - 1, 1);
        b = gMag(ind - 1, 2);
        if a < b
            seam(ind - 1) = 1;
        else
            seam(ind - 1) = 2;
        end
    elseif tmp == n
        a = gMag(ind - 1, n);
        b = gMag(ind - 1, n - 1);
        if a < b
            seam(ind - 1) = n;
        else
            seam(ind - 1) = n - 1;
        end
    else
        a = gMag(ind - 1, tmp - 1);
        b = gMag(ind - 1, tmp);
        c = gMag(ind - 1, tmp + 1);
        path = min([a, b, c]);
        if path == a
            seam(ind - 1) = tmp - 1;
        elseif path == b
            seam(ind - 1) = tmp;
        else
            seam(ind - 1) = tmp + 1;
        end
    end
end
end


function carved = horizontalseam(im)
[m, n, ~] = size(im);
gMag = energy1(im);
seam = findhorizontalseam(gMag, m, n);
carved = zeros(m-1,n,3,'uint8');
for x = 1:n
    row = im(:,x,:);
    row(seam(x),:,:) = [];
    carved(:,x,:) = row(:,:,:);
end
end

function seam = findhorizontalseam(gMag, m, n)
for x = 1:(n-1)
    gMag(1,x+1) = gMag(1,x+1) + min([gMag(1,x), gMag(2,x)]);
    for y = 2:(m-1)
        gMag(y,x+1) = gMag(y,x+1) + min([gMag(y-1,x), gMag(y,x), gMag(y+1,x)]);
    end
    gMag(m,x+1) = gMag(m,x+1) + min([gMag(m,x), gMag(m-1,x)]);
end

seam = zeros(1,n);
[~, ind] = min(gMag(n));
seam(n) = ind;
for ind = n:-1:2
    tmp = seam(ind);
    if tmp == 1
        a = gMag(1, ind - 1);
        b = gMag(2, ind - 1);
        if a < b
            seam(ind - 1) = 1;
        else
            seam(ind - 1) = 2;
        end
    elseif tmp == n
        a = gMag(n, ind - 1);
        b = gMag(n - 1, ind - 1);
        if a < b
            seam(ind - 1) = n;
        else
            seam(ind - 1) = n - 1;
        end
    else
        a = gMag(tmp - 1, ind - 1);
        b = gMag(tmp, ind - 1);
        c = gMag(tmp + 1, ind - 1);
        path = min([a, b, c]);
        if path == a
            seam(ind - 1) = tmp - 1;
        elseif path == b
            seam(ind - 1) = tmp;
        else
            seam(ind - 1) = tmp + 1;
        end
    end
end
end