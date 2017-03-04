%% Wrapper to run dlib's face landmark detector
% Code written by: Nitin J. Sanket, PhD in Computer Science Student,
% University of Maryland, College Park
clc
clear all
close all

%% Setup Paths
CodePath = './FaceLandmarksNitin.py';
PredictorPath = 'shape_predictor_68_face_landmarks.dat';
FacesPath = '../examples/faces2/Nitin.jpg';

%% Execute Python Code
Command = ['python ', CodePath, ' ', PredictorPath, ' ', FacesPath];
[status, cmdout] = system(Command);

%% Parse Outputs
I = imread(FacesPath);
imshow(I);

% Parse the console output and prettify it - please do not modify this code
FaceData = ParseInputs(cmdout);
% Plot Bounding Boxes - feel free to look into the code and extract them
PlotBB(FaceData);
% Plot LandMarks - feel free to look into the code and extract them
PlotLandMarks(FaceData);
