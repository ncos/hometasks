function [X Y V] = match_features(filename, image_count, image_index)

matfile = fopen(filename);

fscanf(matfile, '%s', 1);
val1 = fscanf(matfile, '%d', 1);

% Initialize as zero(s):
X = zeros(val1, image_count);
Y = zeros(val1, image_count);
V = zeros(val1, image_count);


for i = 1 : val1
    
    val2 = fscanf(matfile, '%d', 1);
    fscanf(matfile, '%d', 3);


    X(i, image_index) = fscanf(matfile, '%f', 1);
    Y(i, image_index) = fscanf(matfile, '%f', 1);
    V(i, image_index) = 1;


    for k = 1 : val2-1
        j       = fscanf(matfile, '%d', 1);
        X(i, j) = fscanf(matfile, '%f', 1);
        Y(i, j) = fscanf(matfile, '%f', 1);
        V(i, j) = 1;
    end
end

fclose(matfile);
