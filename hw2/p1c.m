%p1a : canny 1st order edge detection
% sample files are under "./raw" folder.
%sample1.raw, sample2.raw, sample3.raw : 256x256 gray-scale
row=256;  col=256;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%canny
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
lthresholds=[1.5;.1;.6];
hthresholds=[2.6;9;3];
sigmas=[1.0;2.5;3.0];

for i = 1: size(lthresholds,1),
	hthreshold = hthresholds(i,:);
    lthreshold = lthresholds(i,:);
    sigma = sigmas(i,:);
	%load image 
	raw_image='raw\sample1.raw';
	fin=fopen(raw_image,'r');
	I=fread(fin,row*col,'uint8=>uint8');
	fclose(fin);
	S1=reshape(I,row,col);
	S1=S1';

	%figure,imshow(S1, 'Border','tight');
	[final1, thre] = canny(S1,lthreshold, hthreshold, sigma);
	%disp thre
	fig_name = sprintf('sample1 :canny 01 edge map,lthr=%4.2f, hthr=%4.2f, sigma=%4.2f', lthreshold, hthreshold, sigma);
	figure('name',fig_name),imshowpair(S1,final1,'montage');title(fig_name);
	fig_name = sprintf('sample1-canny-01-edgemap-suite-%d',i);
	print(gcf, '-dpng', fig_name);

	%%%%%%%%%%%%%%%%%%%%%
	raw_image='raw\sample2.raw';
	fin=fopen(raw_image,'r');
	I=fread(fin,row*col,'uint8=>uint8');
	fclose(fin);
	S2=reshape(I,row,col);
	S2=S2';
	
	%figure,imshow(S2, 'Border','tight');
	[final2, thre] = canny(S2,lthreshold, hthreshold, sigma);
	%disp thre
	fig_name = sprintf('sample2 :canny 01 edge map,lthr=%4.2f, hthr=%4.2f, sigma=%4.2f', lthreshold, hthreshold, sigma);
	figure('name',fig_name),imshowpair(S2,final2,'montage');title(fig_name);
	fig_name = sprintf('sample2-canny-01-edgemap-suite-%d',i);
	print(gcf, '-dpng', fig_name);

	%%%%%%%%%%%%%%%%%%%%%
	raw_image='raw\sample3.raw';
	fin=fopen(raw_image,'r');
	I=fread(fin,row*col,'uint8=>uint8');
	fclose(fin);
	S3=reshape(I,row,col);
	S3=S3';
	
	%figure,imshow(S3, 'Border','tight');
	[final3, thre] = canny(S3,lthreshold, hthreshold, sigma);
	%disp thre
	fig_name = sprintf('sample3 :canny 01 edge map,lthr=%4.2f, hthr=%4.2f, sigma=%4.2f', lthreshold, hthreshold, sigma);
	figure('name',fig_name),imshowpair(S3,final3,'montage');title(fig_name);
	fig_name = sprintf('sample3-canny-01-edgemap-suite-%d',i);
	print(gcf, '-dpng', fig_name);

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	%show all results
	fig_name = sprintf('canny edge detect-thr(%4.2f-%4.2f) sigma=%4.2f',lthreshold, hthreshold, sigma);
	figure('name',fig_name);
	subplot(2,3,1),imshow(S1);
	subplot(2,3,2),imshow(S2);
	subplot(2,3,3),imshow(S3);
	subplot(2,3,4),imshow(final1);
	subplot(2,3,5),imshow(final2);
	subplot(2,3,6),imshow(final3);
	fig_name = sprintf('canny-edge-detect-suite-%d',i);
	print(gcf, '-dpng', fig_name);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%matlab code verification
if 0,
	method='canny';
	direction='both';%'horizontal' | 'vertical'
	
	%figure,imshow(S1, 'Border','tight');
	[BW1,threshOut] = edge(S1,method);
	threshold = threshOut;
	BW1 = edge(S1,method,threshold,direction);
	%figure,imshow(BW1, 'Border','tight');
	figure('name',['matlab +' method]),imshowpair(S1,BW1,'montage');
	
	%
	raw_image='raw\sample2.raw';
	fin=fopen(raw_image,'r');
	I=fread(fin,row*col,'uint8=>uint8');
	fclose(fin);
	S2=reshape(I,row,col);
	S2=S2';
	%figure,imshow(S2, 'Border','tight');
	BW2 = edge(S2,method,threshold,direction);
	%figure,imshow(BW2, 'Border','tight');
	figure('name',['matlab +' method]),imshowpair(S2,BW2,'montage');
	
	%%%
	raw_image='raw\sample3.raw';
	fin=fopen(raw_image,'r');
	I=fread(fin,row*col,'uint8=>uint8');
	fclose(fin);
	S3=reshape(I,row,col);
	S3=S3';
	%figure,imshow(S3, 'Border','tight');
	BW3 = edge(S3,method,threshold,direction);
	%figure,imshow(BW3, 'Border','tight');
	figure('name',['matlab +' method]),imshowpair(S3,BW3,'montage');
	%
	%show all results
	figure('name','matlab canny Edge Detection');
	subplot(2,3,1),imshow(S1);
	subplot(2,3,2),imshow(S2);
	subplot(2,3,3),imshow(S3);
	subplot(2,3,4),imshow(BW1);
	subplot(2,3,5),imshow(BW2);
	subplot(2,3,6),imshow(BW3);
end