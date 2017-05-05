function Imgs = ReadImgs(Path,ImgFormat,RGBFlag)
if(nargin<3)
    RGBFlag = 1;
end

Dirs = dir(Path);
Dirs = Dirs(~ismember({Dirs.name},{'.','..'}));
Imgs = cell(length(Dirs),0);

ImgNames = dir([Path,Dirs(1).name,'/*','.',ImgFormat]);
NumImagesPerClass = length(ImgNames);

if(length(ImgNames)==6)
StartIdx = 5;
else
    StartIdx = 1;
end

for count = 1:length(Dirs)
    for instance = StartIdx:StartIdx+NumImagesPerClass-1
        Imgs{count, instance-StartIdx+1} = im2double(imread([Path,'s',num2str(count),'/',...
                num2str(instance),'.',ImgFormat]));
        if(~RGBFlag && ndims(Imgs{count, instance-StartIdx+1})==3)
            Imgs{count, instance-StartIdx+1} = rgb2gray(I);
        end
    end
end
end
