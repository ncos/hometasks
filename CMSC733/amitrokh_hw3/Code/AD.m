im = imread('../Images/Brain.jpg')
%        (im, Kappa, Lambda, Variant, NIter)
im0 = AD_(im, 0.005, 0.1, 2, 40);
im1 = AD_(im, 0.010, 0.1, 2, 40);
im2 = AD_(im, 0.050, 0.1, 2, 40);
im3 = AD_(im, 0.100, 0.1, 2, 40);
im4 = AD_(im, 0.500, 0.1, 2, 40);

im5 = AD_(im, 0.90, 0.01, 2, 40);
im6 = AD_(im, 0.90, 0.03, 2, 40);
im7 = AD_(im, 0.90, 0.05, 2, 40);
im8 = AD_(im, 0.90, 0.10, 2, 40);
im9 = AD_(im, 0.90, 0.15, 2, 40);

figure;
subplot(2,3,1)
imshow(uint8(im));
title('Original Image');

subplot(2,3,2)
imshow(im0);
title('AD Image, kappa = 0.005');

subplot(2,3,3)
imshow(im1);
title('AD Image, kappa = 0.010');

subplot(2,3,4)
imshow(im2);
title('AD Image, kappa = 0.050');

subplot(2,3,5)
imshow(im3);
title('AD Image, kappa = 0.100');

subplot(2,3,6)
imshow(im4);
title('AD Image, kappa = 0.500');


figure;
subplot(2,3,1)
imshow(uint8(im));
title('Original Image');

subplot(2,3,2)
imshow(im5);
title('AD Image, lambda = 0.01');

subplot(2,3,3)
imshow(im6);
title('AD Image, lambda = 0.03');

subplot(2,3,4)
imshow(im7);
title('AD Image, lambda = 0.05');

subplot(2,3,5)
imshow(im8);
title('AD Image, lambda = 0.10');

subplot(2,3,6)
imshow(im9);
title('AD Image, lambda = 0.15');




function AD_out = AD_(im, Kappa, Lambda, Variant, NIter)
if(ndims(im)==3)
    im = rgb2gray(im);
end

display('Processing...');

im = im2double(im);

AD_out = im;
%AD_out = padarray(AD_out, [1, 1], 0);

nw = [1, 0, 0; 
      0, -1, 0; 
      0, 0, 0];

n = [0, 1, 0; 
     0, -1, 0; 
     0, 0, 0];

ne = [0, 0, 1; 
      0, -1, 0; 
      0, 0, 0];

e = [0, 0, 0; 
     0, -1, 1; 
     0, 0, 0];

se = [0, 0, 0; 
      0, -1, 0; 
      0, 0, 1];

s = [0, 0, 0; 
     0, -1, 0; 
     0, 1, 0];

sw = [0, 0, 0; 
     0, -1, 0; 
     1, 0, 0];

w = [0, 0, 0; 
     1, -1, 0; 
     0, 0, 0];


for k = 1 : NIter
    g_nw = imfilter(AD_out, nw, 'symmetric', 'conv');
    g_n = imfilter(AD_out, n, 'symmetric', 'conv');
    g_ne = imfilter(AD_out, ne, 'symmetric', 'conv');
    g_e = imfilter(AD_out, e, 'symmetric', 'conv');
    g_se = imfilter(AD_out, se, 'symmetric', 'conv');
    g_s = imfilter(AD_out, s, 'symmetric', 'conv');
    g_sw = imfilter(AD_out, se, 'symmetric', 'conv');
    g_w = imfilter(AD_out, w, 'symmetric', 'conv');
    
    switch Variant
        case 1
            c_nw = exp(-(g_nw ./ Kappa) .^ 2);
            c_n = exp(-(g_n ./ Kappa) .^ 2);
            c_ne = exp(-(g_ne ./ Kappa) .^ 2);
            c_e = exp(-(g_e ./ Kappa) .^ 2);
            c_se = exp(-(g_se ./ Kappa) .^ 2);
            c_s = exp(-(g_s ./ Kappa) .^ 2);
            c_sw = exp(-(g_se ./ Kappa) .^ 2);
            c_w = exp(-(g_w ./ Kappa) .^ 2);
        case 2
            c_nw = 1.0 ./ (1.0 + (g_nw ./ Kappa) .^ 2);
            c_n = 1.0 ./ (1.0 + (g_n ./ Kappa) .^ 2);
            c_ne = 1.0 ./ (1.0 + (g_ne ./ Kappa) .^ 2);
            c_e = 1.0 ./ (1.0 + (g_e ./ Kappa) .^ 2);
            c_se = 1.0 ./ (1.0 + (g_se ./ Kappa) .^ 2);
            c_s = 1.0 ./ (1.0 + (g_s ./ Kappa) .^ 2);
            c_sw = 1.0 ./ (1.0 + (g_se ./ Kappa) .^ 2);
            c_w = 1.0 ./ (1.0 + (g_w ./ Kappa) .^ 2);
        otherwise
            error('Wrong option given');
    end

    AD_out = AD_out + Lambda * (c_nw .* g_nw + c_n .* g_n + c_ne .* g_ne + c_e .* g_e ...
                              + c_se .* g_se + c_s .* g_s + c_sw .* g_sw + c_w .* g_w);

end
%imshow(AD_out);
end

