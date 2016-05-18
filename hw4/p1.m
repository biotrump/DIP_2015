function p1()
    close all;
    %Sample1.raw : 256x256
    %Sample2.raw : 256x256
    %Sample3.raw : 256x256
    %TrainingSet.raw : 450 x 248
    row=256;  col=256;
    sample_image='Sample1.raw';
    fin=fopen(sample_image,'r');
    I=fread(fin,row*col,'uint8=>uint8');
    fclose(fin);
    S1=reshape(I,row,col);
    S1=S1';%image is in row-major, but matlab uses col0-major
    figure;
    imshow(S1);title(sample_image);
    if 1,
        level=111;
        SBW=S1;
        SBW(S1 < 111 )=0;
        SBW(S1 >= 111 )=1;
    else
        level = graythresh(S1);
        SBW = im2bw(S1,level);
    end
    fprintf('Sample1.raw:ostu level:%d\n', level*255);
    SBW = ~SBW;
    figure('name','ostu threshold');
    imshow(SBW);title('Binary Sample1.raw');
    SBW=logical(median(SBW));
    figure('name','median filter');
    imshow(SBW);title('median Sample1.raw');

    se=strel('disk',1,0);%Structuring element
    eSBW=bmorph('erode', SBW, se.getnhood, 2, 2);
    figure('name','erode binary');
    imshow(eSBW);title('erode Binary Sample1.raw');

    trow=450;  tcol=248;
    train_image='TrainingSet.raw';
    fin=fopen(train_image,'r');
    Tr=fread(fin,trow*tcol,'uint8=>uint8');
    fclose(fin);
    T1=reshape(Tr,trow,tcol);
    T1=T1';%image is in row-major, but matlab uses col0-major
    figure;
    imshow(T1);title(train_image);
    if 1,
        Tlevel=142;
        TBW=T1;
        TBW(T1 < 142 )=0;
        TBW(T1 >= 142 )=1;
    else
        Tlevel = graythresh(T1);
        TBW = im2bw(T1,Tlevel);
    end
    fprintf('TrainingSet.raw:ostu level:%d\n', Tlevel*255);
    figure('name','ostu threshold Train');
    TBW = ~TBW;
    imshow(TBW);title('binary TrainingSet.raw');

    %
    figure('name','skeleton Sample1.raw');
    skeleton = bwmorph(SBW,'skel',Inf);
    imshow(skeleton);title('skelenton sample1.raw');

    figure('name','thining Sample1.raw');
    thinning = bwmorph(SBW,'thin',Inf);
    imshow(thinning);title('thinning Image');
    %
    figure('name','skeleton Train.raw');
    tskeleton = bwmorph(TBW,'skel',Inf);
    imshow(tskeleton);title('skelenton sample1.raw');

    figure('name','thining train.raw');
    tthinning = bwmorph(TBW,'thin',Inf);
    imshow(tthinning);title('thinning Image');

    figure('name','Problem 2: morphological Processing');
    subplot(2,3,2);imshow(S1);title('Sample2.raw');
    subplot(2,3,4);imshow(thinning);title('thinning');

end
