%p1b : laplacian 2nd order edge detection with zerocrossing edge detection
% sample files are under "./raw" folder.
%sample1.raw, sample2.raw, sample3.raw : 256x256 gray-scale
%clc;
clear;
row=256;  col=256;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%laplacian
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if 1,
thresholds=[20;60;100];
samples=['raw\sample1.raw';'raw\sample2.raw'; 'raw\sample3.raw'];

for i = 1: size(thresholds,1),
	threshold = thresholds(i,:);
	S={};
	zc_8n={};
	zc_4n={};
	for j = 1: size(samples,1),
		%load image
		%raw_image='raw\sample1.raw';
		raw_image=samples(j,:);
		fin=fopen(raw_image,'r');
		I=fread(fin,row*col,'uint8=>uint8');
		fclose(fin);
		S{j}=reshape(I,row,col);
		S{j}=S{j}';
	
		[zc_8n{j}, zc_4n{j}] = laplacian(S{j},  threshold, 1);
		fig_name = sprintf('%s : laplacian 01 edge map thr=%d 4N-8N',raw_image, threshold);
		figure('name',fig_name),imshowpair(zc_4n{j}, zc_8n{j},'montage');
		fig_name = sprintf('sample%d-laplacian-01-edgemap-thre-%d-4N-8N',j, threshold);
		print(gcf, '-dpng', fig_name);
	end
	
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	%show all results
	fig_name=sprintf('thre=%d laplacian Edge Detection zc_4n-zc_8n',threshold);
	figure('name',fig_name);
	%subplottight(2,3,1),imshow(S1);
	%subplottight(2,3,2),imshow(S2);
	%subplottight(2,3,3),imshow(S3);
	
	subplottight(2,3,1),imshow(zc_4n{1});title('sample1 4N1');
	subplottight(2,3,2),imshow(zc_4n{2});title('sample2 4N2');
	subplottight(2,3,3),imshow(zc_4n{3});title('sample3 4N3');
	%fig_name = sprintf('laplacian-L4H-edge-detect-thr-%d',threshold);
	%print(gcf, '-dpng', fig_name);

	%fig_name=sprintf('thre=%d laplacian Edge Detection zc_8n',threshold);
	%figure('name',fig_name);
	%subplottight(2,3,1),imshow(S1);
	%subplottight(2,3,2),imshow(S2);
	%subplottight(2,3,3),imshow(S3);
	
	subplottight(2,3,4),imshow(zc_8n{1});title('sample1 8N1');
	subplottight(2,3,5),imshow(zc_8n{2});title('sample2 8N2');
	subplottight(2,3,6),imshow(zc_8n{3});title('sample3 8N3');
	fig_name = sprintf('Laplacian-ZC-edge-detect-thr-%d-4N-8N',threshold);
	print(gcf, '-dpng', fig_name);
end

end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%laplacian of Gaussian
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if 1,
% Create the gaussian filter with hsize = [5 5] and sigma = 1
GKF_5x5 = fspecial('gaussian',[5 5],1);
SG={};
for j = 1: size(samples,1),
    %pad 0 to image border for 5x5 kernel
    S1pad = padarray(S{j},[2 2],'replicate', 'both');
	SG{j} = imfilter(S1pad, GKF_5x5, 'same');
    fig_name = sprintf('sample%d-Gaussian 5x5, sig=2',j);
	figure('name',fig_name),imshow(SG{j}, 'Border','tight');
    fig_name = sprintf('sample%d-Gaussian-5x5-sig=2',j);
    print(gcf, '-dpng', fig_name);
end

thresholds=[1;3;10];
for i = 1: size(thresholds,1),
	threshold = thresholds(i,:);
	zc_8n={};
	zc_4n={};
	for j = 1: size(samples,1),
		[zc_8n{j}, zc_4n{j}] = laplacian(SG{j},  threshold, 1);
		fig_name = sprintf('sample%d : LOG 01 edge map thr=%d 4N-8N',j, threshold);
		figure('name',fig_name),imshowpair(zc_4n{j}, zc_8n{j},'montage');
		fig_name = sprintf('sample%d-LOG-01-edgemap-thre-%d-4N-8N',j, threshold);
		print(gcf, '-dpng', fig_name);
	end
	
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	%show all results for a threshold
	fig_name=sprintf('thre=%d LOG Edge Detection zc_4n-zc_8n',threshold);
	figure('name',fig_name);

	subplottight(2,3,1),imshow(zc_4n{1});title('sample1 4N1');
	subplottight(2,3,2),imshow(zc_4n{2});title('sample2 4N2');
	subplottight(2,3,3),imshow(zc_4n{3});title('sample3 4N3');

	subplottight(2,3,4),imshow(zc_8n{1});title('sample1 8N1');
	subplottight(2,3,5),imshow(zc_8n{2});title('sample2 8N2');
	subplottight(2,3,6),imshow(zc_8n{3});title('sample3 8N3');
	fig_name = sprintf('LOG-ZC-edge-detect-thr-%d-4N-8N',threshold);
	print(gcf, '-dpng', fig_name);
end

end

