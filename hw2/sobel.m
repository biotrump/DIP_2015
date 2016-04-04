% http://deepeshrawat987.blogspot.tw/2013/04/edge-detection.html
% http://blog.yam.com/chu24688/article/50729404
% @PARAM image uint8 grey image
% @PARAM Thresh=100
function EM=sobel(image, Thresh, show)
	if nargin < 3
	    show=0;
	end
	if show,
		figure('name', 'sobel edge detection');
		subplot(2,2,1); imshow(image);title('original');
	end
	
	I=double(image);

	for i=1:size(I,1)-2
		for j=1:size(I,2)-2
			%Gx : Sobel mask for x-direction:
			mx=((2*I(i+2,j+1)+I(i+2,j)+I(i+2,j+2))-(2*I(i,j+1)+I(i,j)+I(i,j+2)));
			%Gy : Sobel mask for y-direction:
			my=((2*I(i+1,j+2)+I(i,j+2)+I(i+2,j+2))-(2*I(i+1,j)+I(i,j)+I(i+2,j)));
			%Orthogonal gradient Magnitude
			G(i,j)=sqrt(mx.^2+my.^2);
		end
	end
	if show,
		subplot(2,2,2); imshow(G); title('Sobel gradient');
	end
	%Define a threshold value
	Thresh=100;
	EM=max(G,Thresh);
	EM(EM==round(Thresh))=0;

	EM=uint8(EM);
	if show,	
		subplot(2,2,3);	imshow(~EM);title('Edge detected Image');
	end
end
