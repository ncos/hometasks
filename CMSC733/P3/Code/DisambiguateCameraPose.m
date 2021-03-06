function [C, R, X] = DisambiguateCameraPose(Cset, Rset, Xset)
%% Inputs:
% Cset and Rset: 4 configurations of camera centers and rotations
% Xset: 4 sets of triangulated points from four camera pose configurations
%% Outputs:
% C and R: the correct camera pose
%% Notes:
% The sign of the Z element in the camera coordinate system indicates the 
% location of the 3D point with respect to the camera, i.e., a 3D point X 
% is in front of a camera if (C;R) if r3'(X - C) > 0
% where r3 is the third row of R. Not all triangulated points satisfy this 
% condition due to the presence of correspondence noise. 
% The best camera configuration, (C,R,X) is the one that produces the max 
% number of points satisfying the cheirality condition.



counts = zeros(length(Cset), 1);

for i = 1 : length(Cset)
  xmc = bsxfun(@minus, Xset{i}', Cset{i});
  % score = (Rset{i}(:,3)' * xmc) > 0;
  score = (Xset{i}(:,3)' > 0) & (Rset{i}(:,3)' * xmc) > 0;
  counts(i) = sum(score);
end

[~, idx] = max(counts);
C = Cset{idx};
R = Rset{idx};
X = Xset{idx};

end
