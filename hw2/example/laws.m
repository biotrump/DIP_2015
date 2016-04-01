% @params image input grayscale image
% @params mask_dim	3x3 or 5x5 convolution mask
% @params WindSize window size for energy computation, 13x13 or 15x15
function [ image_out ] = laws( image , mask_dim, WindSize)
%LAWS Laws image filters

%% define fiters
    filters={};
if(mask_dim > 3)
%up to 5x5
    filters{1}=[1 4 6 4 2];		%level, local average
    filters{2}=[-1 -2 0 2 1];	%edge
    filters{3}=[-1 0 2 0 -1];	%spot
    filters{4}=[1 -4 6 -4 1];	%ripple, gabor
else
%default is 3x3 mask
    filters{1}=[1 2 1];		%level, local average
    filters{2}=[-1 0 1];	%edge, 1st order
    filters{3}=[1 -2 1];	%spot, 2nd order
end

% generating Micro structure impulse response arrays
%(a basis set) M_i =F * H_i

    filtered2D={};
%size(filters,2) returns the second,2, dimension of filters 1x5, 1x3,
%so size(filters,2) return 5 or 3.
	mask_dim = size(filters,2);
    for i=1:mask_dim
        for j=1:mask_dim
            temp=filters{i}'*filters{j};	% nx1 * 1xn = nxn
            filtered2D{end+1}=imfilter(image, temp);	%M_i = F cov H_i
        end
    end

%% get resulting 9 maps

    image_out={};
	if(mask_dim > 3)	%5x5
	    image_out{end+1}=wfusmat(filtered2D{2},filtered2D{5},'mean');
	    image_out{end+1}=wfusmat(filtered2D{4},filtered2D{13},'mean');
	    image_out{end+1}=wfusmat(filtered2D{7},filtered2D{10},'mean');
	    image_out{end+1}=filtered2D{11};
	    image_out{end+1}=filtered2D{16};
	    image_out{end+1}=wfusmat(filtered2D{3},filtered2D{9},'mean');
	    image_out{end+1}=filtered2D{6};
	    image_out{end+1}=wfusmat(filtered2D{8},filtered2D{14},'mean');
	    image_out{end+1}=wfusmat(filtered2D{12},filtered2D{15},'mean');
	else	%3x3

	end

end
