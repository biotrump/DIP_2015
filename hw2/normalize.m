%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%normalize the min to max range to 0.0-1.0
function output_image = normalize(input_image)
	MIN = min(min(input_image)); MAX = max(max(input_image));
	output_image = (input_image - MIN) / (MAX - MIN);
end
function stand_dev = image_stat_stand_dev(im_in, window_size)
	[im_height im_width] = size(im_in);
	i = 1;
  shift=(window_size+1)/2-1;
	%border is ignored
	for row = (window_size+1)/2 : im_height-(window_size-1)/2,
		j = 1;
		for column = (window_size+1)/2 : im_width-((window_size-1)/2),
			buffer = im_in(row-shift : row+shift, column-shift : column+shift);
			stand_dev(i, j) = std(buffer(1:end));
			j = j + 1;
		end
		i = i + 1;
	end
end

%Sum all the sqaure of every element in M(i,j)
function enery = image_energy(im_in, window_size)
	[im_height im_width] = size(im_in);
	i = 1;
  shift=(window_size+1)/2-1;
	%border is ignored
	for row = (window_size+1)/2 : im_height-(window_size-1)/2,
		j = 1;
		for column = (window_size+1)/2 : im_width-((window_size-1)/2),
			buffer = im_in(row-shift : row+shift, column-shift : column+shift);
			enery(i, j) = sumsqr(buffer);
			j = j + 1;
		end
		i = i + 1;
	end
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
