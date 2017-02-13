function [Ipano, xtrans, ytrans] = stitch(Im1, Im2, H)

tform = projective2d(transpose(H));

[xlim,ylim]=outputLimits(tform, [1 size(Im2,2)], [1 size(Im2,1)]);
xMin = min([1 xlim]);
xMax = max([size(Im1,2) xlim]);
yMin = min([1 ylim]);
yMax = max([size(Im1,1) ylim]);
width  = round(xMax - xMin + 1);
height = round(yMax - yMin + 1);

xLimits = [xMin xMax];
yLimits = [yMin yMax];
panoramaView = imref2d([height width], xLimits, yLimits);

Itrans = imwarp(Im2,tform, 'OutputView', panoramaView);
Ir = Im1(:,:,1) ~= 0;
Ig = Im1(:,:,2) ~= 0;
Ib = Im1(:,:,3) ~= 0;
mask = Ir|Ig|Ib;
mask = cat(3,mask,mask,mask);
padmask = mask;
xtrans = max([0 floor(0.5-xlim(1))]);
ytrans = max([0 floor(0.5-ylim(1))]);
padmask = padarray(padmask,[ytrans xtrans],0,'pre');
padmask = padarray(padmask,[height-size(padmask,1) width-size(padmask,2)],0,'post');

padim = padarray(Im1,[ytrans xtrans],0,'pre');
padim = padarray(padim,[height-size(padim,1) width-size(padim,2)],0,'post');


Ir = Itrans(:,:,1) ~= 0;
Ig = Itrans(:,:,2) ~= 0;
Ib = Itrans(:,:,3) ~= 0;
Itransmask = Ir|Ig|Ib;
Itransmask = cat(3,Itransmask,Itransmask,Itransmask);
% image intersect mask
andpadmask = padmask & Itransmask;
% image1 only mask
im1padmask = padmask & ~Itransmask;

blur = fspecial('gaussian', 40, 40);
maska = im2double(~padmask);
maska = imfilter(maska,blur,'replicate');
% .5 is the half point of a linear gaussian distribution
maska = maska * 2;
maska(maska > 1) = 1;

% find edge where I1 overlaps I2
edge_filt = ones(3);
edge = imfilter(im2double(padmask),edge_filt,'replicate');
edge(edge == 9) = 0;
edge = edge.*andpadmask;
% find relavant portion of maska
maskamask = imfilter(edge,blur,'replicate');
maskamask = maskamask.*andpadmask;
maskamask(maskamask > 0) = 1;

maska = maska.*maskamask;

maskb = 1-maska;
Ipano = Itrans;
Ipano(im1padmask == 1) = padim(im1padmask == 1);
Ipano(andpadmask == 1) = padim(andpadmask == 1).*maskb(andpadmask == 1)+Itrans(andpadmask == 1).*maska(andpadmask == 1);
%%figure
%%imshow(Ipano);
end
