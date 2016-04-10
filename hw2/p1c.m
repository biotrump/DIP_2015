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
samples=['raw\sample1.raw';'raw\sample2.raw'; 'raw\sample3.raw'];

for i = 1: size(lthresholds,1),
	hthreshold = hthresholds(i,:);
    lthreshold = lthresholds(i,:);
    sigma = sigmas(i,:);
	S={};
	final={};
	thre={};
	for j = 1: size(samples,1),
		%load image
		raw_image=samples(j,:);
		fin=fopen(raw_image,'r');
		I=fread(fin,row*col,'uint8=>uint8');
		fclose(fin);
		S{j}=reshape(I,row,col);
		S{j}=S{j}';
	
		%figure,imshow(S1, 'Border','tight');
		[final{j}, thre{j}] = canny(S{j},lthreshold, hthreshold, sigma);
		%disp thre
		fig_name = sprintf('sample%d :canny 01 edge map,lthr=%4.2f, hthr=%4.2f, sigma=%4.2f', j, lthreshold, hthreshold, sigma);
		figure('name',fig_name),imshowpair(S{j},final{j},'montage');title(fig_name);
		fig_name = sprintf('sample%d-canny-01-edgemap-suite-%d',j, i);
		print(gcf, '-dpng', fig_name);
	end

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	%show all results
	fig_name = sprintf('canny edge detect-thr(%4.2f-%4.2f) sigma=%4.2f',lthreshold, hthreshold, sigma);
	figure('name',fig_name);
	subplottight(2,3,1),imshow(S{1});
	subplottight(2,3,2),imshow(S{2});
	subplottight(2,3,3),imshow(S{3});
	subplottight(2,3,4),imshow(final{1});
	subplottight(2,3,5),imshow(final{2});
	subplottight(2,3,6),imshow(final{3});
	fig_name = sprintf('canny-edge-detect-suite-%d',i);
	print(gcf, '-dpng', fig_name);
end
