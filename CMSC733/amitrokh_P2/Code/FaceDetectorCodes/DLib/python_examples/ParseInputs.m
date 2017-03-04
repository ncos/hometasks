function FaceData = ParseInputs(cmdout)
% cmdout is the console output after running the pything script
% FaceData is a structure with BB and LandMarks as the fiels, BB has the
% bounding box and LandMarks has all the landmarks
% BB is [x, y, w, h]
% LandMarks is [xi, yi] for the ith landmark
% Code written by: Nitin J. Sanket, PhD in Computer Science Student,
% University of Maryland, College Park

Str = strsplit(cmdout, {'\n', ' ', '(', ')', ',',});
NumFaces = str2double(Str{1});

% No faces found
if(isempty(NumFaces) || (NumFaces==0))
    error('No Faces Found!');
end

% Faces found so parse them
NumLandMarks = 68; % we have 68 landmarks in dlib package
FaceData = struct('BB',zeros(4,1), 'LandMarks', zeros(NumLandMarks,2));
for count = 1:NumFaces
    % BB has the Bounding Box in the format [x, y, w, h]
    FaceData(count).BB = [str2double(Str{(count-1)*141 + 3}),...
        str2double(Str{(count-1)*141 + 4}),...
        str2double(Str{(count-1)*141 + 5})-str2double(Str{(count-1)*141 + 3}),...
        str2double(Str{(count-1)*141 + 6})-str2double(Str{(count-1)*141 + 4})];
    LandMarkData = str2double({Str{(count-1)*141 + 7:(count-1)*141 + 142}})';
    LandMarkData = [LandMarkData(1:2:end), LandMarkData(2:2:end)];
    FaceData(count).LandMarks = LandMarkData;
end
end