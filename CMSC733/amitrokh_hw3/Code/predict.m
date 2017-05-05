function PredLabel = predict(I, MeanFace, Model, Basis)

[K,M] = size(Model);
I = I - MeanFace;
omega = zeros(K, 1);
for eigvec = 1:K
    omega(eigvec) = transpose(Basis(:,eigvec)) * I;
end

subjects = 40;
trainImgs = 4;

minval = Inf;
minind = 0;

for face = 1:M
    val = norm(omega - Model(:, face));
    if val < minval
        minval = val;
        minind = face;
    end
end

[PredLabel, ~] = ind2sub([40, 4], minind);
end
