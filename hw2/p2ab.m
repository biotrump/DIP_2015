%p2ab : image stiching
% sample files are under "./raw" folder.
%sample4.raw, sample5.raw, sample6.raw, sample6.raw :324x324 gray-scale
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
waitforbuttonpress;
point1 = get(gcf,'CurrentPoint'); % button down detected
rect = [point1(1,1) point1(1,2) 50 100];
[r2] = dragrect(rect);

%rotate 5 deg couterclockwise
X = gpuArray(S_HE{4});
Y = imrotate(X, 5, 'loose', 'bilinear');
figure,imshow(Y, 'Border','tight');

%padding 0 around
stiching_board=padarray(S_HE{1},[row col]);
figure,imshow(stiching_board, 'Border','tight');
set(ImageH, 'HitTest', 'off');
set(gca, 'ButtonDownFcn', 'moveaxis(1)', 'Tag', 'legend');


Mc=0;Mr=0;
maxDiff = -10000;
for r=1 : 2*row,
	for c=1 : 2*col,
        %diff the overlap and sum up
        if r <= row && c <= col,
            clip_win=S_HE{1}(1:r,1:c) - S_HE{2}(row-r+1:row, col-c+1:col);
        else
            clip_win=S_HE{1}(r-row:row,c-col:col) - S_HE{2}(1:2*row-r+1, 1:2*col-c+1);
        end
        [m,n]=size(clip_win);
		sa(r,c) = sumabs(clip_win)/(m*n);
        if sa(r,c) > maxDiff,
            maxDiff = sa(r,c);
            Mr=r;Mc=c;
        end
	end
end
	sa=normalize(sa);
	figure,imshow(sa, 'Border','tight');

for i = 1: size(thresholds,1),
	threshold = thresholds(i,:);
	%load image 
	raw_image=raw_images(1,:);
	fin=fopen(raw_image,'r');
	I=fread(fin,row*col,'uint8=>uint8');
	fclose(fin);
	S4=reshape(I,row,col);
	S4=S4';
	S4_HE=histeq(S4);
	figure('name',raw_image),imshowpair(S4, S4_HE,'montage');
	%figure,imshow(S1, 'Border','tight');

	%[EM1, BW1] = sobel(S1,threshold, 1);
	figure('name',raw_image),imshowpair(S1, EM1,'montage');
	fig_name = sprintf('%s : sobel 01 edge map thr=%d',raw_image, threshold);
	figure('name',fig_name),imshowpair(S1,BW1,'montage');%title(fig_name);
	fig_name = sprintf('sample1-sobel-01-edgemap-thre-%d',threshold);
	print(gcf, '-dpng', fig_name);

	%%%%%%%%%%%%%%%%%%%%%
	raw_image=raw_images(2,:);;
	fin=fopen(raw_image,'r');
	I=fread(fin,row*col,'uint8=>uint8');
	fclose(fin);
	S2=reshape(I,row,col);
	S2=S2';
	
	[EM2,BW2] = sobel(S2,threshold,1);
	figure('name',raw_image),imshowpair(S2, EM2,'montage');
	fig_name = sprintf('%s : sobel 01 edge map thr=%d', raw_image, threshold);
	figure('name',fig_name),imshowpair(S2,BW2,'montage');%title(fig_name);
	fig_name = sprintf('sample2-sobel-01-edgemap-thre-%d',threshold);
	print(gcf, '-dpng', fig_name);
	
	%%%%%%%%%%%%%%%%%%%%%
	raw_image=raw_images(3,:);;
	fin=fopen(raw_image,'r');
	I=fread(fin,row*col,'uint8=>uint8');
	fclose(fin);
	S3=reshape(I,row,col);
	S3=S3';
	
	[EM3, BW3] = sobel(S3,threshold,1);
	figure('name',raw_image),imshowpair(S3, EM3,'montage');
	fig_name = sprintf('%s : sobel 01 edge map thr=%d',raw_image, threshold);
	figure('name',fig_name),imshowpair(S3,BW3,'montage');%title(fig_name);
	fig_name = sprintf('sample3-sobel-01-edgemap-thre-%d',threshold);
	print(gcf, '-dpng', fig_name);
	
	%%%%%%%%%%%%%%%%%%%%%
	raw_image=raw_images(4,:);;
	fin=fopen(raw_image,'r');
	I=fread(fin,row*col,'uint8=>uint8');
	fclose(fin);
	S4=reshape(I,row,col);
	S4=S4';
	
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

