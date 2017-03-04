function PlotBB(FaceData)
% Function plots bounding boxes
% FaceData is a structure with BB and LandMarks as the fiels, BB has the
% bounding box and LandMarks has all the landmarks
% BB is [x, y, w, h]
% LandMarks is [xi, yi] for the ith landmark
% Code written by: Nitin J. Sanket, PhD in Computer Science Student,
% University of Maryland, College Park

hold on;
for count  = 1:length(FaceData)
    rectangle('Position', [FaceData(count).BB(1), FaceData(count).BB(2),...
        FaceData(count).BB(3), FaceData(count).BB(4)], 'EdgeColor', 'r');
end
hold off;

end