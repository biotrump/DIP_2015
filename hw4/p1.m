function p1()
    close all;
    clear;
    %Sample1.raw : 256x256
    %Sample2.raw : 256x256
    %Sample3.raw : 256x256
    %TrainingSet.raw : 450 x 248

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
        fprintf('TrainingSet.raw:ostu level:%d\n', Tlevel*255);
    end
    figure('name','ostu threshold Train');
    TBW = ~TBW;
    TBW=logical(median(TBW));
    imshow(TBW);title('binary TrainingSet.raw');
    
    %bounding box:y0,y1,x0,x1
	bb=bbox(TBW);
    BBW=TBW;
	for i=1:size(bb,1)
        y0=bb(i,1),y1=bb(i,2),x0=bb(i,3),x1=bb(i,4);
		BBW(y0,x0:x1)=1;
        BBW(y1,x0:x1)=1;
        BBW(y0:y1,x0)=1;
        BBW(y0:y1,x1)=1;
	end
	imshow(BBW);title('bounding TrainingSet.raw');

    %read sample1.raw
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
        fprintf('Sample1.raw:ostu level:%d\n', level*255);
    end
    SBW = ~SBW;
    figure('name','ostu threshold');
    imshow(SBW);title('Binary Sample1.raw');
    SBW=logical(median(SBW));
    fSBW=figure('name','median filter');
    imshow(SBW);title('median Sample1.raw');
    imwrite(SBW, 'bsample1.jpg');
    gtruth_tbl=test_set(SBW);%x0,y0,x1,y1
    
%    set(fSBW,'WindowButtonDownFcn',@mouseClickcallback)
    se=strel('disk',1,0);%Structuring element
    eSBW=bmorph('erode', SBW, se.getnhood, 2, 2);
    figure('name','erode binary');
    imshow(eSBW);title('erode Binary Sample1.raw');

    %
    figure('name','skeleton Sample1.raw');
    skeleton = bwmorph(SBW,'skel',Inf);
    imshow(skeleton);title('skelenton sample1.raw');

    figure('name','thining Sample1.raw');
    thinning = bwmorph(SBW,'thin',Inf);
    imshow(thinning);title('thinning Image');
    
    GTBW=thinning;
	for i=1:size(gtruth_tbl,1)
        x0=gtruth_tbl(i,1),y0=gtruth_tbl(i,2),x1=gtruth_tbl(i,3),y1=gtruth_tbl(i,4);
        %roi = GTBW(y0:y1,x0:x1);
        %fine_bb=bbox(roi);
        GTBW(y0,x0:x1)=1;
        GTBW(y1,x0:x1)=1;
        GTBW(y0:y1,x0)=1;
        GTBW(y0:y1,x1)=1;
	end
	imshow(GTBW);title('bounding Sample1.raw');
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

function mouseClickcallback(hObject,~)
    persistent  i;
    if isempty(i)
        i=1;
    end
    pos=get(hObject,'CurrentPoint');
    %disp([i,' X:',num2str(pos(1)),', Y:',num2str(pos(2))]);
    fprintf('%d:X=%d, Y=%d \n',i, num2str(pos(1)), num2str(pos(2)));
    bb_list(i,:)=[num2str(pos(2)) num2str(pos(1))];
    i=i+1;
end
