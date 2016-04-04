% http://deepeshrawat987.blogspot.tw/2013/04/edge-detection.html
% http://blog.yam.com/chu24688/article/50729404
% @PARAM image uint8 grey image
% @PARAM Thresh=100
function [EM, BEM]=sobel(image, Thresh, show)
	if nargin < 3
	    show=0;
	end
	if show,
		figure('name', 'sobel edge detection');
        %figure();
		subplot(2,2,1); imshow(image);title('original');
	end
	
	I=double(image);

	for i=1:size(I,1)-2
		for j=1:size(I,2)-2
			%Gx : Sobel mask for x-direction:
			dx=((2*I(i+2,j+1)+I(i+2,j)+I(i+2,j+2))-(2*I(i,j+1)+I(i,j)+I(i,j+2)));
			%Gy : Sobel mask for y-direction:
			dy=((2*I(i+1,j+2)+I(i,j+2)+I(i+2,j+2))-(2*I(i+1,j)+I(i,j)+I(i+2,j)));
			%Orthogonal gradient Magnitude
			G(i,j)=sqrt(dx.^2+dy.^2);
            Gx(i,j)=dx;
            Gy(i,j)=dy;
		end
    end
    %h = histogram(G);
	if show,
		G_Nx = normalize(Gx);
		subplot(2,2,2); imshow(G_Nx); title('Sobel gradient:Gx');
		G_Ny = normalize(Gy);
		subplot(2,2,3); imshow(G_Ny); title('Sobel gradient:Gy');
		G_N = normalize(G);
		subplot(2,2,4); imshow(G_N); title('Sobel gradient: G=Gx^2+Gy^2');
    end
    %threshold the magnitude: if the magnitude <= threshold. set it to "0"
    %else keep the magnitude.
    % max(G, Thresh) : if G(i,j) <= threshold, G(i,j)= threshold,
    % else if G(i,j) > threshold, G(i,j) is kept intact.
	EM=max(G,Thresh);
    %compare each element of EM to round(Thresh), if the element is equal
    %to round(Thresh), it's 1 else it's 0.
    %So if the elements of EM == round(threshold) will become "0".
    %else the elements of EM does not change.
	EM(EM==round(Thresh))=0;% elements <= threshold becomes "0"
	EM=uint8(EM);

    %binary edge map, 1: edge, 0 : background
    BEM= G >= Thresh;
	if show,	
		%subplot(2,2,3);	
        %edge intensity "1", background is "0"
        %figure();imshow(EM);title('Sobel Edge detected Image');
	end
end
