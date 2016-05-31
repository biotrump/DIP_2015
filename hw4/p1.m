% PROBLEM 1: Shape analysis
% Implementation: main program for problem 1
% M-file name: p1.m
% Usage: p1
% Output image:
% Parameters: no
% p1 loads Sample1.raw, 256x256, image and TrainingSet.raw, 450 x 248, image. It will
% perform morphological operations and shape analysis to find the matches between
% Sample1.raw and TrainingSet.raw.
%Sample1.raw : 256x256
%Sample2.raw : 256x256
%Sample3.raw : 256x256
%TrainingSet.raw : 450 x 248
function p1()
    close all;
    clear;
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %loading TrainingSet.raw
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    trow=450;  tcol=248;
    train_image='TrainingSet.raw';
    fin=fopen(train_image,'r');
    Tr=fread(fin,trow*tcol,'uint8=>uint8');
    fclose(fin);
    T1=reshape(Tr,trow,tcol);
    T1=T1';%image is in row-major, but matlab uses col0-major
    figure;
    imshow(T1);title(train_image);
    figure('name','hist of TrainingSet.raw');
    h = histogram(T1,256);
    
    if 1,
        Tlevel=142; %binary thredshold, the value is from ostu threshold!
        TBW=T1;
        TBW(T1 < 142 )=0;
        TBW(T1 >= 142 )=1;
    else
        Tlevel = graythresh(T1);
        TBW = im2bw(T1,Tlevel);
        fprintf('TrainingSet.raw:ostu level:%d\n', Tlevel*255);
    end
    figure('name','ostu threshold Train');
    TBW = ~TBW;%foreground is 1
    TBW=logical(median(TBW));   %median filter to remove impulse noise
    imshow(TBW);title('binary TrainingSet.raw');
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % to close the spur edge by opening
    if 0
    se=strel('disk',1,0);%Structuring element
    TBW=bmorph('dilate', TBW, se.getnhood, 2, 2);
    se=strel('disk',1,0);%Structuring element
    TBW=bmorph('erode', TBW, se.getnhood, 2, 2);
    %se=strel('disk',1,0);%Structuring element
    %TBW=bmorph('dilate', TBW, se.getnhood, 2, 2);
    end
    %bounding box:y0,y1,x0,x1
	tbb=bbox(TBW);   %bounding box for all characters/alphabet

    %
    %figure('name','skeleton Train.raw');
    tskeleton = bwmorph(TBW,'skel',Inf);
    %imshow(tskeleton);title('skelenton train.raw');
    tbbs=tskeleton;
	for i=1:size(tbb,1)
        y0=tbb(i,1);
        y1=tbb(i,2);
        x0=tbb(i,3);
        x1=tbb(i,4);
		tbbs(y0,x0:x1)=1;
        tbbs(y1,x0:x1)=1;
        tbbs(y0:y1,x0)=1;
        tbbs(y0:y1,x1)=1;
    end
    figure('name','skeleton Train.raw');
	imshow(tbbs);title('bounding skeleton TrainingSet.raw');

    %figure('name','thining train.raw');
    tthinning = bwmorph(TBW,'thin',Inf);
    %imshow(tthinning);title('thinning Image');
    tbbt=tthinning;
    %ttfi=1;
	for i=1:size(tbb,1)
        y0=tbb(i,1);
        y1=tbb(i,2);
        x0=tbb(i,3);
        x1=tbb(i,4);

		%fine tune bounding box
		roi = tthinning(y0:y1,x0:x1);
        new_bb=bbox(roi);
        ny0=new_bb(1,1);
        ny1=new_bb(1,2);
        nx0=new_bb(1,3);
        nx1=new_bb(1,4);
        yy0=y0+ny0-1;
        xx0=x0+nx0-1;
        yy1=y0+ny1-1;
        xx1=x0+nx1-1;
        tbb(i,1)=yy0;%-1;
        tbb(i,2)=yy1;%+1;
        tbb(i,3)=xx0;%-1;
        tbb(i,4)=xx1;%+1;
		%mark the bounding box
        tbbt(yy0,xx0:xx1)=1;
        tbbt(yy1,xx0:xx1)=1;
        tbbt(yy0:yy1,xx0)=1;
        tbbt(yy0:yy1,xx1)=1;

        %[A0, Aa, P, Pa, E4, E8, La, Wa, Wr, Hr]=get_feature(roi,'GRAY');
        %tthin_feature(ttfi,:)=[A0, Aa, P, Pa, E4, E8, La, Wa, Wr, Hr];
        %ttfi=ttfi+1;
    end
    figure('name','bounding thinning TrainingSet.raw');
    imshow(tbbt);title('bounding thinning TrainingSet.raw');
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %read sample1.raw
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    row=256;  col=256;
    sample_image='Sample1.raw';
    fin=fopen(sample_image,'r');
    I=fread(fin,row*col,'uint8=>uint8');
    fclose(fin);
    S1=reshape(I,row,col);
    S1=S1';%image is in row-major, but matlab uses col0-major
    figure;
    imshow(S1);title(sample_image);
    figure('name','hist of Sample1.raw');
    h = histogram(S1,256);

    if 1,
        level=111;  %binary threshold by ostu threshold
        SBW=S1;
        SBW(S1 < 111 )=0;
        SBW(S1 >= 111 )=1;
    else
        level = graythresh(S1);
        SBW = im2bw(S1,level);
        fprintf('Sample1.raw:ostu level:%d\n', level*255);
    end
    SBW = ~SBW; %foreground is "1"
    figure('name','ostu threshold');
    imshow(SBW);title('Binary Sample1.raw');

    SBW=logical(median(SBW));   %median filter to remove pepper and salt noise
    fSBW=figure('name','median filter');
    imshow(SBW);title('median Sample1.raw');
    imwrite(SBW, 'bsample1.jpg');

    gtruth_tbl=test_set(SBW);   %bounding box :x0,y0,x1,y1

%    set(fSBW,'WindowButtonDownFcn',@mouseClickcallback)
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % to close the spur edge by opening
    se=strel('disk',3,0);%Structuring element
    SBW=bmorph('dilate', SBW, se.getnhood, 4, 4);
    se=strel('disk',1,0);%Structuring element
    SBW=bmorph('erode', SBW, se.getnhood, 2, 2);
%    figure('name','erode binary');
%    imshow(eSBW);title('erode Binary Sample1.raw');

    %skeleton
    figure('name','skeleton Sample1.raw');
    skeleton = bwmorph(SBW,'skel',Inf);
    imshow(skeleton);title('skelenton sample1.raw');
    %thining
    figure('name','thining Sample1.raw');
    thinning = bwmorph(SBW,'thin',Inf);
    imshow(thinning);title('thinning Image');

    figure('name','thining bounding box');
    GTBW=thinning;
    tbw=thinning;
    %stfi=1;
	for i=1:size(gtruth_tbl,1)
        x0=gtruth_tbl(i,1);
        y0=gtruth_tbl(i,2);
        x1=gtruth_tbl(i,3);
        y1=gtruth_tbl(i,4);
        %coarse bounding box
        tbw(y0,x0:x1)=1;
        tbw(y1,x0:x1)=1;
        tbw(y0:y1,x0)=1;
        tbw(y0:y1,x1)=1;

        %fine tune bounding box
        roi = GTBW(y0:y1,x0:x1);
        new_bb=bbox(roi);
        ny0=new_bb(1,1);
        ny1=new_bb(1,2);
        nx0=new_bb(1,3);
        nx1=new_bb(1,4);
        yy0=y0+ny0-1;
        xx0=x0+nx0-1;
        yy1=y0+ny1-1;
        xx1=x0+nx1-1;
        GTBW(yy0,xx0:xx1)=1;
        GTBW(yy1,xx0:xx1)=1;
        GTBW(yy0:yy1,xx0)=1;
        GTBW(yy0:yy1,xx1)=1;
        gtruth_tbl(i,1)=xx0;
        gtruth_tbl(i,2)=yy0;
        gtruth_tbl(i,3)=xx1;
        gtruth_tbl(i,4)=yy1+1;
        %roi = thinning(yy0:yy1,xx0:xx1);
        %[A0, Aa, P, Pa, E4, E8, La, Wa, Wr, Hr]=get_feature(roi,'GRAY');
        %sthin_feature(stfi,:)=[A0, Aa, P, Pa, E4, E8, La, Wa, Wr, Hr];
        %stfi=stfi+1;
    end
    subplot(1,2,1);imshow(tbw);title('coarse bounding box');
	subplot(1,2,2);imshow(GTBW);title('fined bounding box');

    %figure('name','Problem 2: morphological Processing');
    %subplot(2,3,2);imshow(S1);title('Sample2.raw');
    %subplot(2,3,4);imshow(thinning);title('thinning');

	%find the matched alphabet by feature distance
	%bounding tbb:y0,y1,x0,x1
	%gtruth_tbl : x0 y0 x1 y1
	minDist1(tthinning, tbb, thinning, gtruth_tbl);
	%minDist1(tthinning, tbb, thinning, gtruth_tbl);

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
