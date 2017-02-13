function [fdesc, anms_x, anms_y] = get_features(Im, nbest)

[~, set_size] = size(Im);

%% Get some featurz
anms_x = cell(1, set_size);
anms_y = cell(1, set_size);
for k = 1 : set_size
    [anms_x{k}, anms_y{k}] = ANMS(Im{k}, nbest);
    figure;
    imshow(Im{k});
    hold on;
    scatter(anms_x{k}, anms_y{k}, '.');
    title(sprintf('ANMS %d', k));
    hold off;
end

%% Features:
fdesc = cell(1, set_size);
for k = 1 : set_size
    fdesc_k = cell(1, nbest);
    pad_im = padarray(Im{k}, [40 40], 'replicate');
    for i = 1 : nbest
        x = anms_x{k}(i);
        y = anms_y{k}(i);
        
        filt_crop = imfilter(imcrop(pad_im, [x + 20 y + 20 39 39]), fspecial('gaussian'));
        subsample = filt_crop(1:5:40, 1:5:40);
        
        vect = reshape(subsample, 64, 1);
        vect = vect - mean(vect);
        vect = vect./std(vect);
        fdesc_k{i} = vect;
    end
    fdesc{k} = fdesc_k;
end

