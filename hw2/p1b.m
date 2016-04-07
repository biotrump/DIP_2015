%p1a : laplacian 2nd order edge detection with zerocrossing edge detection
% sample files are under "./raw" folder.
%sample1.raw, sample2.raw, sample3.raw : 256x256 gray-scale
row=256;  col=256;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%laplacian
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if 1,
thresholds=[25;50;150];
for i = 1: size(thresholds,1),
	threshold = thresholds(i,:);
	%load image
	raw_image='raw\sample1.raw';
	fin=fopen(raw_image,'r');
	I=fread(fin,row*col,'uint8=>uint8');
	fclose(fin);
	S1=reshape(I,row,col);
	S1=S1';

	%figure,imshow(S1, 'Border','tight');

	[zc_8h1, zc_4h1] = laplacian(S1,  threshold, 1);
	fig_name = sprintf('%s : laplacian 01 edge map thr=%d',raw_image, threshold);
	figure('name',fig_name),imshowpair(zc_8h1, zc_4h1,'montage');
	fig_name = sprintf('sample1-laplacian-01-edgemap-thre-%d',threshold);
	print(gcf, '-dpng', fig_name);

	%%%%%%%%%%%%%%%%%%%%%
	raw_image='raw\sample2.raw';
	fin=fopen(raw_image,'r');
	I=fread(fin,row*col,'uint8=>uint8');
	fclose(fin);
	S2=reshape(I,row,col);
	S2=S2';

	[zc_8h2,zc_4h2] = laplacian(S2,threshold,1);
	fig_name = sprintf('%s : laplacian 01 edge map thr=%d',raw_image, threshold);
	figure('name',fig_name),imshowpair(zc_8h2, zc_4h2,'montage');
	fig_name = sprintf('sample1-laplacian-01-edgemap-thre-%d',threshold);
	print(gcf, '-dpng', fig_name);
	%%%%%%%%%%%%%%%%%%%%%
	raw_image='raw\sample3.raw';
	fin=fopen(raw_image,'r');
	I=fread(fin,row*col,'uint8=>uint8');
	fclose(fin);
	S3=reshape(I,row,col);
	S3=S3';

	[zc_8h3, zc_4h3] = laplacian(S3,threshold,1);
	fig_name = sprintf('%s : laplacian 01 edge map thr=%d',raw_image, threshold);
	figure('name',fig_name),imshowpair(zc_8h3, zc_4h3,'montage');
	fig_name = sprintf('sample1-laplacian-01-edgemap-thre-%d',threshold);
	print(gcf, '-dpng', fig_name);
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	%show all results
	fig_name=sprintf('thre=%d laplacian Edge Detection zc_4h',threshold);
	figure('name',fig_name);
	subplot(2,3,1),imshow(S1);
	subplot(2,3,2),imshow(S2);
	subplot(2,3,3),imshow(S3);
	subplot(2,3,4),imshow(zc_4h1);
	subplot(2,3,5),imshow(zc_4h2);
	subplot(2,3,6),imshow(zc_4h3);
	fig_name = sprintf('laplacian-L4H-edge-detect-thr-%d',threshold);
	print(gcf, '-dpng', fig_name);

	fig_name=sprintf('thre=%d laplacian Edge Detection zc_8h',threshold);
	figure('name',fig_name);
	subplot(2,3,1),imshow(S1);
	subplot(2,3,2),imshow(S2);
	subplot(2,3,3),imshow(S3);
	subplot(2,3,4),imshow(zc_8h1);
	subplot(2,3,5),imshow(zc_8h2);
	subplot(2,3,6),imshow(zc_8h3);
	fig_name = sprintf('laplacian-L8H-edge-detect-thr-%d',threshold);
	print(gcf, '-dpng', fig_name);
end
end
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	%laplacian of Gaussian
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   	%gaussian
    %# Create the gaussian filter with hsize = [5 5] and sigma = 1
    GKF_5x5 = fspecial('gaussian',[5 5],1);
	thresholds=[1;3;10];
	for i = 1: size(thresholds,1),
		threshold = thresholds(i,:);
		%load image
		raw_image='raw\sample1.raw';
		fin=fopen(raw_image,'r');
		I=fread(fin,row*col,'uint8=>uint8');
		fclose(fin);
		S1=reshape(I,row,col);
		S1=S1';

        %pad image border for 5x5 kernel
        S1pad = padarray(S1,[2 2],'replicate', 'both');
%    S1pad = double(S1pad);
%        S1pad=S1;
		S1pad = imfilter(S1pad, GKF_5x5, 'same');
        fig_name = sprintf('sample1-Gaussian 5x5, sig=2-%d',threshold);
		figure('name',fig_name),imshow(S1pad, 'Border','tight');
        fig_name = sprintf('sample1-Gaussian-5x5-sig=2-%d',threshold);
        print(gcf, '-dpng', fig_name);

		[zc_8h1, zc_4h1] = laplacian(S1pad,  threshold, 1);
		fig_name = sprintf('%s : LOG 01 edge map thr=%d',raw_image, threshold);
		figure('name',fig_name),imshowpair(zc_8h1, zc_4h1,'montage');
		fig_name = sprintf('sample1-LOG-01-edgemap-thre-%d',threshold);
		print(gcf, '-dpng', fig_name);

		%%%%%%%%%%%%%%%%%%%%%
		raw_image='raw\sample2.raw';
		fin=fopen(raw_image,'r');
		I=fread(fin,row*col,'uint8=>uint8');
		fclose(fin);
		S2=reshape(I,row,col);
		S2=S2';

    %pad image border for 5x5 kernel
    S2pad = padarray(S2,[2 2],'replicate', 'both');
%    S1pad = double(S1pad);
%        S1pad=S1;
		S2pad = imfilter(S2pad, GKF_5x5, 'same');
        fig_name = sprintf('sample1-Gaussian 5x5, sig=2-%d',threshold);
		figure('name',fig_name),imshow(S2pad, 'Border','tight');
        fig_name = sprintf('sample1-Gaussian-5x5-sig=2-%d',threshold);
        print(gcf, '-dpng', fig_name);

		[zc_8h2,zc_4h2] = laplacian(S2pad,threshold,1);
		fig_name = sprintf('%s : LOG 01 edge map thr=%d',raw_image, threshold);
		figure('name',fig_name),imshowpair(zc_8h2, zc_4h2,'montage');
		fig_name = sprintf('sample1-LOG-01-edgemap-thre-%d',threshold);
		print(gcf, '-dpng', fig_name);
		%%%%%%%%%%%%%%%%%%%%%
		raw_image='raw\sample3.raw';
		fin=fopen(raw_image,'r');
		I=fread(fin,row*col,'uint8=>uint8');
		fclose(fin);
		S3=reshape(I,row,col);
		S3=S3';

    %pad image border for 5x5 kernel
    S3pad = padarray(S3,[2 2],'replicate', 'both');
%    S1pad = double(S1pad);
%        S1pad=S1;
		S3pad = imfilter(S3pad, GKF_5x5, 'same');
        fig_name = sprintf('sample1-Gaussian 5x5, sig=2-%d',threshold);
		figure('name',fig_name),imshow(S3pad, 'Border','tight');
        fig_name = sprintf('sample1-Gaussian-5x5-sig=2-%d',threshold);
        print(gcf, '-dpng', fig_name);

		[zc_8h3, zc_4h3] = laplacian(S3pad,threshold,1);
		fig_name = sprintf('%s : LOG 01 edge map thr=%d',raw_image, threshold);
		figure('name',fig_name),imshowpair(zc_8h3, zc_4h3,'montage');
		fig_name = sprintf('sample1-LOG-01-edgemap-thre-%d',threshold);
		print(gcf, '-dpng', fig_name);
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		%show all results
		fig_name=sprintf('thre=%d LOG Edge Detection zc_4h',threshold);
		figure('name',fig_name);
		subplot(2,3,1),imshow(S1);
		subplot(2,3,2),imshow(S2);
		subplot(2,3,3),imshow(S3);
		subplot(2,3,4),imshow(zc_4h1);
		subplot(2,3,5),imshow(zc_4h2);
		subplot(2,3,6),imshow(zc_4h3);
		fig_name = sprintf('LOG-L4H-edge-detect-thr-%d',threshold);
		print(gcf, '-dpng', fig_name);

		fig_name=sprintf('thre=%d LOG Edge Detection zc_8h',threshold);
		figure('name',fig_name);
		subplot(2,3,1),imshow(S1);
		subplot(2,3,2),imshow(S2);
		subplot(2,3,3),imshow(S3);
		subplot(2,3,4),imshow(zc_8h1);
		subplot(2,3,5),imshow(zc_8h2);
		subplot(2,3,6),imshow(zc_8h3);
		fig_name = sprintf('LOG-L8H-edge-detect-thr-%d',threshold);
		print(gcf, '-dpng', fig_name);
    end
