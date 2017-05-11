
RR = do_eigenface(40);
display(RR);


function [RR] = do_eigenface(ftrs)
    h = 112;
    w = 92;

    Img = zeros(h, w, 40*4);

    average_img = zeros(h, w);
    n = 1;
    for i = 1 : 40
        for j = 1 : 4
            tmpImg = imread(['../Images/Dataset/Train/s',num2str(i),'/',num2str(j),'.pgm']);
            Img(:,:,n) = double(tmpImg);
            average_img = average_img + double(tmpImg);
            n = n + 1;
        end
    end
    average_img = average_img / (40*4);
    Img = reshape(Img, h * w, 40*4);



    DiffFace = zeros(h * w, 40*4);
    for i = 1 : (40*4)
        DiffFace(:,i) = Img(:,i) - reshape(average_img, h * w, 1);
    end
    L = DiffFace' * DiffFace;
    [ev, ~] = eig(L);
    u = ev(:, 40*4 - ftrs + 1 : 40*4);
    v = DiffFace * u;



    for i = 1 : ftrs
        v(:,i) = v(:,i) / norm(v(:,i));
    end

    RR = faceprocess(v, h, w, DiffFace, average_img);
end

