%% visualize R, t
figure(1); clf;
% 3D points
plot3(X(:, 1), X(:, 2), zeros(size(X, 1), 1), 'kx');
% cameras
for i=1:numel(imgs)
    hold on;
    camLoc = Rs_opt(:, :, i)'*(-ts_opt(:, :, i));
    plot3(camLoc(1), camLoc(2), camLoc(3), 'rs');
    quiver3(camLoc(1), camLoc(2), camLoc(3), Rs_opt(3, 1, i), Rs_opt(3, 2, i), Rs_opt(3, 3, i), 100);
    hold off;
end
% others
grid on; axis equal;
xlabel('X');
ylabel('Y');
zlabel('Z');
set(gca,'XAxisLocation', 'top', 'YAxisLocation', 'left', 'YDir', 'reverse', 'ZDir', 'reverse');
title('camera positions and directions with 3D points')
saveas(1, 'Rt.png');


%% run baseline
cameraParams = estimateCameraParameters(x, X, 'EstimateSkew', false);

%% fin: check outputs
if(mean(mean(abs(cameraParams.IntrinsicMatrix' - K_opt)))>1.85 |...
        mean(mean(abs(cameraParams.TranslationVectors' - reshape(ts_opt, [3, size(ts_opt, 3)]))))>0.58 |...
        mean(mean(mean(abs(permute(cameraParams.RotationMatrices, [2 1 3]) - Rs_opt))))>8e-4 |...
        mean(mean(abs(cameraParams.RadialDistortion(:) - ks_opt(:))))>0.02)
    error('Your results are not accurate enough!');
end

if(~exist('Rt.png', 'file'))
   error('Please save Rt.png'); 
end

%% If you completed everything correctly!
msgbox('Congragulations! You have completed the homework successfully');
