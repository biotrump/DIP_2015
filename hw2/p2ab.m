% DIP Homework Assignment #2
% April 12, 2016
% Name: ½²ºû·s Thomas Tsai
% ID %: D04922009
% email: d04922009@ntu.edu.tw, thomas@life100.cc
%###############################################################################%  
% PROBLEM 2: GEOMETRICAL MODIFICATION   
% Implementation: main program problem 2a and proble 2b
% M-file name: p2ab.m  
% Usage: p2ab  
% Output image: sample4-sample5.png, sample4-sample7.png, sample5-sample6.png
% stitching-rectangle-wxh-512x512.png, cropped-wxh-512x512.png  
% Parameters: no
% -------------------------------------------------------------------------------
% subroutine : stitching.m
% -------------------------------------------------------------------------------
% function [sI, corner_a, corner_b, rect_oa, rect_ob] = stitching(Ia, Ib, startc, startr, endc, endr)
% Given 2 images Ia,Ib
% @param Ia image is always at the center of the canvas.
% @param Ib images to stitch to Ia, this acts as a moving window over the
% canvas
% @param startc, startr : the start position of stitching to save time
% @param endc, endr end of moving box to find stitching
% @return sI the stitching image in the canvas
% @return corner_a, corner_b : the left top corner of the image Ia and Ib to stitching
% corner_a, corner_b : (row, col)
% -------------------------------------------------------------------------------
% subroutine : bbox.m
% -------------------------------------------------------------------------------
% function BBox=bbox(O)
% @brrief find a bounding box to contain some object in the canvas
% @param O a canvas with some object inside
% @return BBox bouding box (y0,x0,y1,x1)
% -------------------------------------------------------------------------------
% subroutine : rect_int.m
% -------------------------------------------------------------------------------
% function [rc,o1,o2]=rect_int(r1, r2)
% @brief given r1,r2 rectangles and find the intersection rectangle to the global
% coordination and also the local coordinations to r1 and r2
% @param r1 = [x1 y1 x2 y2]
% @param r2 = [x3 y3 x4 y4]
% and x1 < x2, y1 < y2, x3 < x4, y3 < y4
% @return rc[x5 y5 x6 y6] the intersection rectangle to the glogal coordination
% @return o1  the intersection rectangle to the r1 coordination
% @return o2  the intersection rectangle to the r2 coordination
%###############################################################################%  

%p2ab : image stiching
% sample files are under "./raw" folder.
%sample4.raw, sample5.raw, sample6.raw, sample7.raw :324x324 gray-scale
row=324;  col=324;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
%	S_HE{i}=histeq(S{i});
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

%BBox=bbox(sI);   %bounding box of current stitching (y0,x0,y1,x1) in the canvas
%sI1(BBox(1):BBox(3),BBox(2):BBox(4));  %the bounding box as a source image to stiching
fig_name = sprintf('sample4-sample5');
print(gcf, '-dpng', fig_name);


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
rectangle('Position', [x y w h], 'LineWidth',2, 'EdgeColor','g');
fig_name = sprintf('stitching-rectangle-wxh-%dx%d',w,h);
print(gcf, '-dpng', fig_name);

%draw final stitching
BBox=bbox(sI1);   %bounding box of current stitching (y0,x0,y1,x1) in the canvas
final = sI1(BBox(1):BBox(3),BBox(2):BBox(4));  %the bounding box as a source image to stiching
figure('name','final:sample4 +  sample5 + sample7 + sample6'),imshow(final, 'Border','tight');

%crop image
crop_rect = sI1(y:y+h-1,x:x+w-1);
fig_name=sprintf('crop width=%d height=%d', w,h);
figure('name', fig_name),imshow(crop_rect, 'Border','tight');
fig_name = sprintf('cropped-wxh-%dx%d',w,h);
print(gcf, '-dpng', fig_name);
%writing stitching
fileID = fopen('stitch-512x512.raw','w');
fwrite(fileID, crop_rect);
fclose(fileID);

