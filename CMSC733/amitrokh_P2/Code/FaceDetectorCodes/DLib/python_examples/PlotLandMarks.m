function PlotLandMarks(FaceData)
% Function plots facial landmarks
% FaceData is a structure with BB and LandMarks as the fiels, BB has the
% bounding box and LandMarks has all the landmarks
% BB is [x, y, w, h]
% LandMarks is [xi, yi] for the ith landmark
% Code written by: Nitin J. Sanket, PhD in Computer Science Student,
% University of Maryland, College Park

hold on;
for count  = 1:length(FaceData)
    LandMarksNow = FaceData(count).LandMarks;
    plot(LandMarksNow(:,1), LandMarksNow(:,2), 'g.');
end
hold off;

end