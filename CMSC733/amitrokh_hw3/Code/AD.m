im = imread('../Images/Brain.jpg')
AD_(im, 0.1, 0.05, 1, 2);



function AD_out = AD_(im, Kappa, Lambda, Variant, NIter)
if(ndims(im)==3)
    im = rgb2gray(im);
end

im = im2double(im);

AD_out = im;
AD_out = padarray(AD_out, [1, 1], 0);
temp = AD_out;
[row col] = size(AD_out);
for k = 1:NIter
    for i = 2:row - 1
        for j = 2:col - 1
            delta_n = norm(temp(i - 1,j) - temp(i,j));
            delta_ne = norm(temp(i - 1,j + 1) - temp(i,j));
            delta_nw = norm(temp(i - 1,j - 1) - temp(i,j));
            delta_s = norm(temp(i + 1,j) - temp(i,j));
            delta_se = norm(temp(i + 1,j + 1) - temp(i,j));
            delta_sw = norm(temp(i + 1,j - 1) - temp(i,j));
            delta_w = norm(temp(i,j - 1) - temp(i,j));
            delta_e = norm(temp(i,j + 1) - temp(i,j));
            switch Variant
                case 1
                    cn = exp(-(delta_n/Kappa).^2);
                    cne = exp(-(delta_ne/Kappa).^2);
                    cnw = exp(-(delta_nw/Kappa).^2);
                    cs = exp(-(delta_s/Kappa).^2);
                    cse = exp(-(delta_se/Kappa).^2);
                    csw = exp(-(delta_sw/Kappa).^2);
                    ce = exp(-(delta_e/Kappa).^2);
                    cw = exp(-(delta_w/Kappa).^2);
                case 2
                    cn = 1 / ( 1 + (delta_n/Kappa).^2);
                    cne = 1 / ( 1 + (delta_ne/Kappa).^2);
                    cnw = 1 / ( 1 + (delta_nw/Kappa).^2);
                    cs = 1 / ( 1 + (delta_s/Kappa).^2);
                    cse = 1 / ( 1 + (delta_se/Kappa).^2);
                    csw = 1 / ( 1 + (delta_sw/Kappa).^2);
                    ce = 1 / ( 1 + (delta_e/Kappa).^2);
                    cw = 1 / ( 1 + (delta_w/Kappa).^2);
                otherwise
                    error('Wrong option given');
            end
            AD_out(i,j) = temp(i,j) + Lambda * (cn * delta_n + cne * delta_ne + cnw * delta_nw + ...
                cs * delta_s + cse * delta_se + csw * delta_sw + ce * delta_e + cw * delta_w);
        end
    end
    temp = AD_out;
end
imshow(AD_out);
end
