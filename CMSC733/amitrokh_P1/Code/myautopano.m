function [] = myautopano(folder_name, nbest, focus)

rthresh = 2;
RANSACiter = 900;
SSDthresh = 8;

%% Get images
close all; %% ?!!

f = dir (sprintf('../Images/%s/*.jpg', folder_name));
files = {f.name};
filepaths = cellfun(@(x) sprintf('../Images/%s/%s', folder_name, x), files, 'UniformOutput', false);

set_size = numel(files);

global Im
Im = cell(1, set_size);
Igray = cell(1, set_size);

for k = 1 : set_size
    Im{k} = im2double(imread(filepaths{k}));
    Im{k} = cylindrify(Im{k}, focus); %% Do cylindrical stuff
    Igray{k} = rgb2gray(Im{k});
end

[fdesc, anms_x, anms_y] = get_features(Igray, nbest);

close all;
fmatch12 = cell(set_size);
numMatches = zeros(set_size);
for a = 1 : set_size
    for b = 1 : set_size
        if a == b
            continue
        end
        
        fprintf('Matching %i with %i\n', a, b);
        
        %% Feature matching
        fdesc_a = fdesc{a};
        fdesc_b = fdesc{b};
        fmatch_ab = [];
        for i = 1 : nbest
            d1 = inf;
            d2 = inf;
            dmatch = 1;
            for j = 1 : nbest
                dist = sum((fdesc_a{i} - fdesc_b{j}).^2);
                if dist <= d1
                    d2 = d1;
                    d1 = dist;
                    dmatch = j;
                elseif dist <= d2
                    d2 = dist;
                end 
            end
            r = d2/d1;
            if r >= rthresh
                fmatch_ab = [fmatch_ab; [i dmatch]];
            end
        end
        
        if numel(fmatch_ab) == 0
            fmatch1 = [];
            fmatch2 = [];
            %figure;
            %imshow([Im{a} Im{b}]);
            %title(sprintf('feature match %d %d',a,b));
        else
            fmatch1 = [anms_x{a}(fmatch_ab(:,1)), anms_y{a}(fmatch_ab(:,1))];
            fmatch2 = [anms_x{b}(fmatch_ab(:,2)), anms_y{b}(fmatch_ab(:,2))];
            fmatch12{a,b} = {fmatch1 fmatch2};
            numMatches(a,b) = size(fmatch_ab,1);
            %figure;
            %dispMatchedFeatures(Im{a}, Im{b}, fmatch1, fmatch2, 'montage');
            %title(sprintf('feature match %d %d', a, b));
        end
    end
end
numMatches(numMatches < 10) = 0;

fprintf('Matching done; Starting RANSAC...');

%% RANSAC
%%close all;
%%imatch12 = cell(set_size);
numRansac = zeros(set_size);
global Hab;
Hab = cell(set_size);
for a = 1 : set_size
    for b = 1 : set_size
        if numMatches(a,b) == 0
            continue
        end
        
        fprintf('RANSACing %i with %i\n', a, b);
        
        fmatch1 = fmatch12{a,b}{1};
        fmatch2 = fmatch12{a,b}{2};
        bestInliers = [];
        bestNumMatches = 0;
        for i = 1 : RANSACiter
            rmatch = [];
            while numel(rmatch) ~= 4
                rmatch = unique(randi(size(fmatch1,1),4,1));
            end
            rmatch1 = fmatch1(rmatch,:);
            rmatch2 = fmatch2(rmatch,:);
            H = est_homography(rmatch1(:,1),rmatch1(:,2),rmatch2(:,1),rmatch2(:,2));
            [Hx,Hy] = apply_homography(H,fmatch2(:,1),fmatch2(:,2));
            SSD = (fmatch1(:,1) - Hx).^2 + (fmatch1(:,2) - Hy).^2;
            matches = SSD < SSDthresh;
            inliers = find(matches);
            if numel(inliers) > bestNumMatches
                bestNumMatches = numel(inliers);
                bestInliers = inliers;
            end
            if numel(inliers) > 0.9 * size(fmatch1,1)
                break
            end
        end
        imatch1 = fmatch1(bestInliers,:);
        imatch2 = fmatch2(bestInliers,:);
        %%imatch12{a, b} = {imatch1 imatch2};
        numRansac(a, b) = numel(bestInliers);
        H = est_homography(imatch1(:,1),imatch1(:,2),imatch2(:,1),imatch2(:,2));
        Hab{a,b} = H./H(3,3);
        %figure;
        %dispMatchedFeatures(Im{a},Im{b},imatch1,imatch2,'montage');
        %title(sprintf('RANSAC %d %d',a,b));
    end
end
numRansac(numRansac < 5) = 0;

close all;
mapping = zeros(set_size - 1, 1);


for a = 1 : (set_size - 1)
    disp(numRansac)
    [val, idx] = max(numRansac(a,:));
    if (val == 0)      
        [val, idx] = max(numRansac(:,a));
        if (val == 0) 
            continue;
        end
    end
    mapping(a) = idx;
    
    numRansac(idx, a) = 0;

end

disp(mapping);

Ipano = Im{1};
H = eye(3);
Htmp = eye(3);
for a = 1 : (set_size - 1)
    fprintf('Stitching %i and %i\n', a, mapping(a));
    Htmp = Htmp * Hab{a, mapping(a)};
    [Ipano, xtrans, ytrans] = stitch(Ipano, Im{mapping(a)}, H * Htmp);
    H = H * [1 0 xtrans; 0 1 ytrans; 0 0 1];
end


imwrite(Ipano, sprintf('../Images/%s.jpg', folder_name));

%%k = waitforbuttonpress;
end
