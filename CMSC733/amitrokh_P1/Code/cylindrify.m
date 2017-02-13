function [Iout] = cylindrify(Iin, f)
Iin = im2double(Iin);

h = size(Iin, 1);
w = size(Iin, 2);
size_ = h * w;

xc = (w + 1) / 2;
yc = (h + 1) / 2;
[x,y] = meshgrid(1 : w, 1 : h);

xp = f * tan((x - xc) / f) + xc;
yp = ((y - yc) ./ cos((x - xc) / f)) + yc;


%% Proper new indexes
x1 = floor(xp);
y1 = floor(yp);
x1(x1 < 1) = 1;
y1(y1 < 1) = 1;
x1(x1 >= w) = w - 1; 
y1(y1 >= h) = h - 1;
    

%% Smooth pixels 


ind11 = sub2ind([h, w], y1, x1);
ind21 = sub2ind([h, w], y1 + 1, x1);
ind12 = sub2ind([h, w], y1, x1 + 1);
ind22 = sub2ind([h, w], y1 + 1, x1 + 1);

dx = xp - x1;
dy = yp - y1;
    
outr = Iin(ind11).*(1 - dy).*(1 - dx) + ...
       Iin(ind21).*(dy).*(1 - dx) + ...
       Iin(ind12).*(1 - dy).*(dx) + ...
       Iin(ind22).*(dy).*(dx);
        
ind11 = ind11 + size_;
ind21 = ind21 + size_;
ind12 = ind12 + size_;
ind22 = ind22 + size_;
    
outg = Iin(ind11).*(1 - dy).*(1 - dx) + ...
       Iin(ind21).*(dy).*(1 - dx) + ...
       Iin(ind12).*(1 - dy).*(dx) + ...
       Iin(ind22).*(dy).*(dx);
       
ind11 = ind11 + size_;
ind21 = ind21 + size_;
ind12 = ind12 + size_;
ind22 = ind22 + size_;

outb = Iin(ind11).*(1 - dy).*(1 - dx) + ...
       Iin(ind21).*(dy).*(1 - dx) + ...
       Iin(ind12).*(1 - dy).*(dx) + ...
       Iin(ind22).*(dy).*(dx);

% Reassemble
Iout = cat(3, outr, outg, outb);

% being fancy)
bounds = (yp > h) | (xp > w) | (yp < 1) | (xp < 1);
bounds = cat(3, bounds, bounds, bounds);
Iout(bounds == 1) = 0;
end

