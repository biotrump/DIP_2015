%Laws method texture masks methods
% @params imageG grey level image
% @params filter_type L3S3 or L5S5 mask
% @params statistic_type energy computation
% @params normalization_type
% @return image_out the result after Laws' step1 and step2
function [image_out] = Laws_mask(imageG, filter_type, window_size, statistic_type, normalization_type, show)
if nargin < 6
    show=0;
end
	%filter_type == 'x3y3' or 'x5y5'
	if filter_type(2) ~= filter_type(4),
		error(['wrong filter type given : ' filter_type]);
	end
	if filter_type(2)=='3', % 3x3 mask
		L3 = [1 2 1]; E3 = [-1 0 1]; S3 = [-1 2 -1]; L3L3mask = L3' * L3;
		switch filter_type
			case 'L3L3'
				filter_mask = L3' * L3;
			case 'E3E3'
				filter_mask = E3' * E3;
			case 'S3S3'
				filter_mask = S3' * S3;
			case 'E3L3'
				filter_mask = E3' * L3;
			case 'L3E3'
				filter_mask = L3' * E3;
			case 'S3L3'
				filter_mask = S3' * L3;
			case 'L3S3'
				filter_mask = L3' * S3;
			case 'S3E3'
				filter_mask = S3' * E3;
			case 'E3S3'
				filter_mask = E3' * S3;
			otherwise
				error(['wrong filter type given: ' filter_type]);
		end
	elseif filter_type(2)=='5', %5x5 mask
		L5 = [1 4 6 4 1]; E5 = [-1 -2 0 2 1]; S5 = [-1 0 2 0 -1];
		R5 = [1 -4 6 -4 1]; W5 = [-1 2 0 -2 1];  L5L5mask = L5' * L5;
		switch filter_type
			case 'L5L5'
				filter_mask = L5' * L5;
			case 'E5E5'
				filter_mask = E5' * E5;
			case 'S5S5'
				filter_mask = S5' * S5;
			case 'W5W5'
				filter_mask = W5' * W5;
			case 'R5R5'
				filter_mask = R5' * R5;
			case 'L5E5'
				filter_mask = L5' * E5;
			case 'E5L5'
				filter_mask = E5' * L5;
			case 'L5S5'
				filter_mask = L5' * S5;
			case 'S5L5'
				filter_mask = S5' * L5;
			case 'L5W5'
				filter_mask = L5' * W5;
			case 'W5L5'
				filter_mask = W5' * L5;
			case 'L5R5'
				filter_mask = L5' * R5;
			case 'R5L5'
				filter_mask = R5' * L5;
			case 'E5S5'
				filter_mask = E5' * S5;
			case 'S5E5'
				filter_mask = S5' * E5;
			case 'E5W5'
				filter_mask = E5' * W5;
			case 'W5E5'
				filter_mask = W5' * E5;
			case 'E5R5'
				filter_mask = E5' * R5;
			case 'R5E5'
				filter_mask = R5' * E5;
			case 'S5W5'
				filter_mask = S5' * W5;
			case 'W5S5'
				filter_mask = W5' * S5;
			case 'S5R5'
				filter_mask = S5' * R5;
			case 'R5S5'
				filter_mask = R5' * S5;
			case 'W5R5'
				filter_mask = W5' * R5;
			case 'R5W5'
				filter_mask = R5' * W5;
			otherwise
				error(['wrong filter type given: ' filter_type]);
		end
	else
		error(['only 3x3 or 5x5 filter type is supported:' filter_type]);
	end
	image = im2double(imageG);  %to double precision

	% % STEP 1 mask convolution
	%disp(file_name);
	disp([filter_type  ' Mask convolution -> in progress...' ]);
	tic
	image_conv = conv2(image,filter_mask,'valid');
	disp([filter_type ' Mask convolution -> done.']);
	% % STEP 2 statistic energy computation
	% F * H_i = M_i
	% E ( M_i) => T_i
    %av_filter is a nxn matrix whose element is 1.0/n*n
    %an image convolves av_filter means "low pass", average the image.
	av_filter = fspecial('average', [window_size window_size]);
	switch statistic_type
		case 'MEAN'
			disp([statistic_type ' Statistic computation (mean) -> in progress...']);
			image_conv_TEM = conv2(image_conv,av_filter,'valid');
			disp([statistic_type ' Statistic computation (mean) -> done.']);
		case 'ABSM'
			disp([statistic_type ' Statistic computation (abs mean) -> in progress...']);
			image_conv_TEM = conv2(abs(image_conv),av_filter,'valid');
			disp([statistic_type ' Statistic computation (abs mean) -> done.']);
		case 'STDD'
			disp([statistic_type ' Statistic computation (st deviation) -> in progress...']);
			image_conv_TEM = image_stat_stand_dev(image_conv, window_size);
			disp([statistic_type ' Statistic computation (st deviation) -> done...']);
			case 'ENRG'
            disp([statistic_type ' Statistic computation (energy) -> in progress...']);
			image_conv_TEM = image_energy(image_conv, window_size);
			disp([statistic_type ' Statistic computation (energy) -> done.']);
		otherwise
			error([statistic_type ' wrong statistic type given']);
	end
	switch normalization_type
		case 'MINMAX'
			image_out = normalize(image_conv_TEM);
		case 'FORCON'
			if filter_type(2)=='3',
				image_conv_norm = conv2(image,L3L3mask,'valid');
			else
				image_conv_norm = conv2(image,L5L5mask,'valid');
			end
			switch statistic_type
				case 'MEAN'
					image_norm = conv2(image_conv_norm,av_filter,'valid');
				case 'ABSM'
					image_norm = conv2(abs(image_conv_norm),av_filter,'valid');
				case 'STDD'
					image_norm = image_stat_stand_dev(image_conv_norm, window_size);
				case 'ENRG'
						image_norm = image_energy(image_conv_norm, window_size);
			end
			image_out = normalize(image_conv_TEM ./ image_norm);
	end
	t = toc
	% % present results, normalize before plotting
	if show,
        figure,
        imshow(image_out,'Border','tight'); title(['filter type: ' filter_type ' statistic type: ' statistic_type ' norm type: ' normalization_type ' elapsed:' num2str(t)]);
	end
end
