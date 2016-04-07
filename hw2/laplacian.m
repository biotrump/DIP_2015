%
% @param image input image
% @param
% @return zc_4
% @return zc_8
function [zc_4, zc_8]=laplacian(image, threshold, show)
	if nargin < 3
	    show=0;
	end
	persistent my_count;
    if isempty(my_count)
        my_count=1;
    end

    % perform laplacian
    %Filter Masks
    LKF_4H=[0 1 0;1 -4 1; 0 1 0];   % 4 neighbors
    LKF_8H=[1 1 1;1 -8 1; 1 1 1];   % 8 neighbors
    %pad image border for 3x3 kernel
    pimage = padarray(image,[1 1],'replicate', 'both');
    pimage = double(pimage);
if 1,
    L_4H = conv2(pimage, LKF_4H, 'same');
    L_8H = conv2(pimage, LKF_8H, 'same');
else
    for i=1:size(pimage,1)-2
        for j=1:size(pimage,2)-2
            L_4H(i,j)=sum(sum(LKF_4H.*pimage(i:i+2,j:j+2)));
            L_8H(i,j)=sum(sum(LKF_8H.*pimage(i:i+2,j:j+2)));
        end
    end
%    L_4H=uint8(L_4H);
%    L_8H=uint8(L_8H);
end
    fig_name=sprintf('laplacian operator %d', my_count);
    figure('name',fig_name);
    subplot(1,3,1),imshow(image);title('original');
	subplot(1,3,2),imshow(normalize(L_4H));title('4-neighbor');
	subplot(1,3,3),imshow(normalize(L_8H));title('8-neighbor');
    fig_name=sprintf('laplacian-operator-%d', my_count);
    print(gcf, '-dpng', fig_name);

    %zero crossing edge detection
    %4-neighbors
    zc_4 = zerocross(L_4H, 4, threshold);
%    fig_name=sprintf('fig%d zerocrossing 4 neighbors', my_count);
%    figure('name',fig_name);
%    imshow(zc_4*255);title('zc 4');

    zc_8 = zerocross(L_8H, 8, threshold);
%    fig_name=sprintf('fig%d zerocrossing 8 neighbors', my_count);
%    figure('name',fig_name);
%    imshow(zc_8*255);title('zc 8');
end
