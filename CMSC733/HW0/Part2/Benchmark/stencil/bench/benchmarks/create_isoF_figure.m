function h = create_isoF_figure()
%clear all;close all;clc;

h=figure;
title('Boundary Benchmark on BSDS');
hold on;

plot(0.700762,0.897659,'go','MarkerFaceColor','g','MarkerEdgeColor','g','MarkerSize',10);
%% isoF lines
[p,r] = meshgrid(0.01:0.01:1,0.01:0.01:1);
F=2*p.*r./(p+r);
[C,h] = contour(p,r,F);

% colormap green
map=zeros(256,3); map(:,1)=0; map(:,2)=1; map(:,3)=0; colormap(map);

box on;
grid on;
set(gca,'XTick',0:0.1:1);
set(gca,'YTick',0:0.1:1);
set(gca,'XGrid','on');
set(gca,'YGrid','on');
xlabel('Recall');
ylabel('Precision');
title('');
axis square;
axis([0 1 0 1]);



%%
%add canny from figure 17 of the paper
xs=0:.1:1;
ys=[.9,.77,.73,.7,.66,.615,.575,.525,.465,.385,.17];
plot(xs,ys,'--');

%%
%add sobel 
xs=0.1:.1:0.9;
ys=[0.9400,0.9300,0.9150,0.8900,0.8500,0.8000,0.7300,0.6250,0.3500];
plot(xs,ys,'--');
end