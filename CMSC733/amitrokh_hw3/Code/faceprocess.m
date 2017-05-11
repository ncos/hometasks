function [RR] = faceprocess(v, h, w, dfce, average_img)
    Feature = dfce' * v;

    correct = zeros(40, 6);
    correctValue = zeros(40, 6);
    wrong = zeros(40, 6);
    for i = 1 : 40
        for j = 1 : 6
            diff = double(imread(['../Images/Dataset/Test/s',num2str(i),'/',num2str(j+4),'.pgm'])) - average_img;
            diff = reshape(diff, h * w, 1);

            omega = diff' * v;
            for k = 1 : size(Feature,1)
                tmp = Feature(k,:) - omega;
                epsilon(k) = norm(tmp);
            end

            [minNum, minIndex] = min(epsilon);
            if (ceil(minIndex/4) == i)
                correct(i,j) = 1;
                correctValue(i,j) = minNum;
            else
                wrong(i,j) = minIndex;
            end
        end
    end
    RR = sum(sum(correct))/(40 * 6);
end
