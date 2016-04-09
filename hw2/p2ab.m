%p2ab : image stiching
% sample files are under "./raw" folder.
%sample4.raw, sample5.raw, sample6.raw, sample7.raw :324x324 gray-scale
row=324;  col=324;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
thresholds=[50;100;250];
raw_images=['raw\sample4.raw';'raw\sample5.raw';'raw\sample6.raw';'raw\sample7.raw'];
S={};
S_HE={};
c={};
for i = 1 : size(raw_images,1),
	%load image
	raw_image=raw_images(i,:);
	fin=fopen(raw_image,'r');
	I=fread(fin,row*col,'uint8=>uint8');
	fclose(fin);
	S{i}=reshape(I,row,col);
	S{i}=S{i}';
	S_HE{i}=histeq(S{i});
%    c{i} = corner(S_HE{i});
%	figure('name',raw_image),imshowpair(S{i}, S_HE{i},'montage');
%    hold on
%plot(c{i}(:,1), c{i}(:,2), 'r*');
end

%1. sample5 stitching to sample4
startc=550;
startr=400;
endc=580;
endr=440;
[sI1, corner_S1, corner_S2, rect_o1]=stitching(S{1}, S{2}, startc, startr, endc, endr);
%stitching S2 to S1
fig_name =sprintf('%s + %s',raw_images(1,:), raw_images(2,:));
figure('name', fig_name),imshow(sI1, 'Border','tight');
fig_name = sprintf('sample4-sample5');
print(gcf, '-dpng', fig_name);

%BBox=bbox(sI);   %bounding box of current stitching (y0,x0,y1,x1) in the canvas
%sI1 = sI(BBox(1):BBox(3),BBox(2):BBox(4));  %the bounding box as a source image to stiching

%2. stiching S4=sample7.raw to S1=sample4.raw
startc=200;
startr=600;
endc=230;
endr=630;
[sI2, corner_S1, corner_S4, rect_o3]=stitching(S{1}, S{4}, startc, startr, endc, endr);
fig_name =sprintf('%s + %s',raw_images(1,:), raw_images(4,:));
figure('name',fig_name),imshow(sI2, 'Border','tight');
fig_name = sprintf('sample4-sample7');
print(gcf, '-dpng', fig_name);
%BBox=bbox(sI);   %bounding box of current stitching (y0,x0,y1,x1) in the canvas
%sI2 = sI(BBox(1):BBox(3),BBox(2):BBox(4));  %the bounding box as a source image to stiching
%stitching Sample4+Sample5+sample7
sI1(corner_S4(1):corner_S4(1)+row-1 , corner_S4(2):corner_S4(2)+col-1) = S{4};
%fig_name =sprintf('%s + %s',raw_images(1,:), raw_images(4,:));
figure('name','sample4 +  sample5 +sample7'),imshow(sI1, 'Border','tight');

%3. stiching S2=sample5.raw to S3=sample6.raw
startc=260;
startr=530;
endc=290;
endr=560;
[sI3, corner_SC2, corner_S3, rect_o2]=stitching(S{2}, S{3}, startc, startr, endc, endr);
fig_name =sprintf('%s + %s',raw_images(2,:), raw_images(3,:));
figure('name',fig_name),imshow(sI3, 'Border','tight');
fig_name = sprintf('sample5-sample6');
print(gcf, '-dpng', fig_name);
%BBox=bbox(sI);   %bounding box of current stitching (y0,x0,y1,x1) in the canvas
%sI3 = sI(BBox(1):BBox(3),BBox(2):BBox(4));  %the bounding box as a source image to stiching
%translate (S2, ie,sample5) from center of canvas to  corner_S2
tr= corner_S2 - corner_SC2;
%stitching (S1 + S2) + (S1 + S4)  + (S2 +  S3)
sI1=padarray(sI1,[50 50], 'replicate','post');
sI1(corner_S3(1)+tr(1):corner_S3(1)+tr(1) + row-1 , corner_S3(2)+tr(2):corner_S3(2)+tr(2)+col-1) = S{3};
figure('name','sample4 +  sample5 + sample7 + sample6'),imshow(sI1, 'Border','tight');
%draw inscribe rectangle
corner_S3 = corner_S3 + tr;	%translat corner of S3 by tr
%x1 of corner_S1(y1, x1) -> x2 of corner_S3(y2, x2)
%y1 of corner_S2(y1, x1) -> y2 of corner_S4(y2, x2)
hold on;
% draw rectangle [x y w h]
x=corner_S1(2);
y=corner_S2(1);
w = (corner_S3(2) + col -1) - corner_S1(2) + 1;
h = (corner_S4(1) + row -1) - corner_S2(1) + 1;
rectangle('Position', [x y w h]);

BBox=bbox(sI1);   %bounding box of current stitching (y0,x0,y1,x1) in the canvas
final = sI1(BBox(1):BBox(3),BBox(2):BBox(4));  %the bounding box as a source image to stiching
figure('name','final:sample4 +  sample5 + sample7 + sample6'),imshow(final, 'Border','tight');
%crop image
crop_rect = sI1(y:y+h-1,x:x+w-1);
fig_name=sprintf('crop width=%d height=%d', w,h);
figure('name', fig_name),imshow(crop_rect, 'Border','tight');
%internal rectangle

%
    if 0,
    %rotate 5 deg couterclockwise
    %X = gpuArray(S_HE{4});
    %Y = imrotate(X, 5, 'loose', 'bilinear');
    %figure,imshow(Y, 'Border','tight');

        [EM3, BW3] = sobel(S4,threshold,1);
        figure('name',raw_image),imshowpair(S3, EM3,'montage');
        fig_name = sprintf('%s : sobel 01 edge map thr=%d',raw_image, threshold);
        figure('name',fig_name),imshowpair(S3,BW3,'montage');%title(fig_name);
        fig_name = sprintf('sample3-sobel-01-edgemap-thre-%d',threshold);
        print(gcf, '-dpng', fig_name);
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        %show all results
        figure('name','sobel Edge Detection');
        subplot(2,3,1),imshow(S1);
        subplot(2,3,2),imshow(S2);
        subplot(2,3,3),imshow(S3);
        subplot(2,3,4),imshow(BW1);
        subplot(2,3,5),imshow(BW2);
        subplot(2,3,6),imshow(BW3);
        fig_name = sprintf('sobel-edge-detect-thr-%d',threshold);
        print(gcf, '-dpng', fig_name);
    end
%end
