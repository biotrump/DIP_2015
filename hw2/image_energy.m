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