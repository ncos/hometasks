clear all;
K = 7;

SquareTrainImgs = ReadImgs('../Images/Dataset/Train/','pgm');
[subjs, imgs] = size(SquareTrainImgs);
M = numel(SquareTrainImgs);
vecsize = numel(SquareTrainImgs{1,1});

avg_face = zeros(vecsize, 1);

Imgs = cell(size(SquareTrainImgs));
for img = 1:M
    Imgs{img} = reshape(SquareTrainImgs{img}, [vecsize, 1]);
    avg_face = avg_face + Imgs{img} / M;
end

for img = 1:M
    Imgs{img} = Imgs{img} - avg_face;
end

A = [Imgs{:}];
[v, d] = eig(transpose(A) * A);
Asz = size(A);
vsz = size(v);
u = zeros(Asz(1), vsz(2));
for col = 1:vsz(1)
    u(:,col) = transpose(A * v(:,col));
    u(:,col) = u(:,col) / norm(u(:,col));
end

values = sum(d);
[~,inds] = sort(values);
best_eigen = zeros(Asz(1), K);
for k = 1:K
    best_eigen(:,k) = u(:,inds(k));
end

omega_train = zeros(K, M);
for subj = 1:subjs
    for img = 1:imgs
        for eigvec = 1:K
            omega_train(eigvec, sub2ind([subjs, imgs], subj, img)) = transpose(best_eigen(:,eigvec)) * Imgs{subj, img};
        end
    end
end


SquareImgs = ReadImgs('../Images/Dataset/Test/','pgm');
M = numel(SquareImgs);
vecsize = numel(SquareImgs{1,1});

Imgs = cell(size(SquareImgs));
[subjs, imgs] = size(SquareImgs);
for img = 1:M
    Imgs{img} = reshape(SquareImgs{img}, [vecsize, 1]);
end

PredLabel = zeros(M,1);
ind = 1;
for subj = 1:subjs
    for img = 1:imgs
        PredLabel(ind) = predict(Imgs{subj, img}, avg_face, omega_train, best_eigen);
        ind = ind + 1;
    end
end


numImgs = 6;
TrueLabels = repmat(1:40,numImgs,1);
TrueLabels = TrueLabels(:);
RR = sum(PredLabel==TrueLabels) / (40 * numImgs) * 100;
disp(['Recognition Rate is: ', num2str(RR), '%']);
