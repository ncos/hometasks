%% CMSC 733: Project 5 Helper Code
% Written by: Nitin J. Sanket (nitinsan@terpmail.umd.edu) & Anton Mitrokhin (and mb Chahat)
% PhD in CS Student at University of Maryland, College Park
% Acknowledgements: Bhoram Lee of University of Pennsylvania for help with depthToCloud function

clc
clear all
close all


Path = '../Dataset/SingleObject/'; 
SceneNum = 12;
SceneName = sprintf('%0.3d', SceneNum);
fname = 'frame_';
frames = dir([Path,'scene_',SceneName,'/frames/', fname, '*depth.png']);
%frames = 1 : numel(frames); // All frames; Not all frames are there!
frames = [7, 110, 170, 224, 372];
%frames = [7, 110];

Clouds = cell(1, 0);
for i = frames
    fprintf('Frame: %i \n', i);
    FrameNum = num2str(i);

    %% Extract 3D Point cloud
    I = imread([Path,'scene_',SceneName,'/frames/', fname, FrameNum,'_rgb.png']);
    ID = imread([Path,'scene_',SceneName,'/frames/', fname, FrameNum,'_depth.png']);
    [pcx, pcy, pcz, r, g, b, D_, X, Y, validInd] = depthToCloud_full_RGB(ID, I, './params/calib_xtion.mat');
    Pts = [pcx pcy pcz];
    
    Cloud = struct('Points', Pts, 'Colors', [r, g, b], 'Normals', []);
    Cloud = trim(Cloud, 300);
    Cloud = downsample(Cloud, 20000); % Downsample the pointcoud

    % Remove large enough planes
    cloud_size = size(Cloud.Points, 1);
    while (true)
        ids = plane_detector(Cloud, 150, 10);
        p_size = size(ids, 1);
        if (p_size / cloud_size < 0.2)
            break;
        end

        display(['Removing ', num2str(p_size), ' points \n']);
        Cloud.Points(ids, :) = [];
        Cloud.Colors(ids, :) = [];
    end

    tic;
    Cloud.Normals = getNormals(Cloud.Points, 50);
    toc;
    
    % Center point cloud
    mu = mean(Cloud.Points);
    Cloud.Points = bsxfun(@minus, Cloud.Points, mu);
    
    Clouds = [Clouds, Cloud];
    
    %Pts = Cloud.Points;
    %scatter3(Pts(:, 1), Pts(:, 2), Pts(:, 3), 50, [r, g, b], '.');
    
    %C = Cloud.Colors;
    C = ((Cloud.Normals +1) ./2);

    figure,
    pcshow(Cloud.Points, C, 'MarkerSize', 30);
    drawnow;
    title('PointCloud');
    xlabel('X'); ylabel('Y'); zlabel('Z');
end

% ICP stuff:
Cloud = Clouds{1};
for i = 2 : numel(Clouds)
    Cloud_current = Clouds{i};

    tic;
    [Cloud_rot, R, T] = icp(Cloud_current, Cloud, 100);
    toc;
    
    Pts  = [Cloud_rot.Points;  Cloud_current.Points];
    C    = [Cloud_rot.Colors;  Cloud_current.Colors];
    Norm = [Cloud_rot.Normals; Cloud_current.Normals];
    
    Cloud = struct('Points', Pts, 'Colors', C, 'Normals', Norm);
end

pcd = struct('Label', 'Penguin', 'Model', Cloud);
save('3d/penguin.mat', 'pcd');

C = ((Cloud.Normals +1) ./2);
figure,
pcshow(Cloud.Points, C, 'MarkerSize', 30);
drawnow;
title('Result PointCloud');
xlabel('X'); ylabel('Y'); zlabel('Z');
