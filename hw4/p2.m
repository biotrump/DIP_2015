% PROBLEM 2: Morphological Processing
% Implementation: main program for problem 2
% M-file name: p2.m
% Usage: p2
% Output image:
% Parameters: no
% p2 loads Sample2.raw, 256x256, image. It performs morphological operations.
function p2()
    close all;
    %Sample1.raw : 256x256
    %Sample2.raw : 256x256
    %Sample3.raw : 256x256
    %TrainingSet.raw : 450 x 248
    row=256;  col=256;
    raw_image='sample2.raw';
    fin=fopen(raw_image,'r');
    I=fread(fin,row*col,'uint8=>uint8');
    fclose(fin);
    S1=reshape(I,row,col);
    S1=S1';%image is in row-major, but matlab uses col0-major
    figure;
    imshow(S1);title(raw_image);

    %direct call matlab
    figure('name','skeleton');
    skeleton = bwmorph(S1,'skel',Inf);
    imshow(skeleton);title('skelenton Image');

    figure('name','thining');
    thinning = bwmorph(S1,'thin',Inf);
    imshow(thinning);title('thinning Image');

    %boundary extract
    se=strel('disk',4,0);%Structuring element
    %F=imerode(S1,se);%Erode the image by structuring element
    F=bmorph('erode', S1, se.getnhood, 5, 5);
    %Difference between binary image and Eroded image
    Boundary=S1-F;
    figure('name','morphology'),imshow(Boundary);title('Boundary extracted Image');

	%opening
    se=strel('disk',7,0);%Structuring element
    %F=imerode(S1,se);%Erode the image by structuring element
    eS1=bmorph('erode', S1, se.getnhood, 8, 8);
    figure('name','eS1'),imshow(eS1);title('ed eS1');

    se=strel('disk',6,0);%Structuring element
    oS1=bmorph('dilate', eS1, se.getnhood, 6, 6);
    figure('name','opening'),imshow(oS1);title('disk opening');
    afterOpening = oS1;
if 0
    %matlab ball opening
    %se = strel('disk',10);
    se = offsetstrel('ball',10,10);
    afterOpening = imopen(S1,se);
    figure('name','ball opening');
    imshow(afterOpening);title('ball opening Image');
end

	%show all results
    figure('name','Problem 2: morphological Processing');
    subplot(2,3,2);imshow(S1);title('Sample2.raw');
    subplot(2,3,4);imshow(thinning);title('thinning');
    subplot(2,3,5);imshow(Boundary);title('boundary extract');
    subplot(2,3,6);imshow(afterOpening);title('disk opening');

end
