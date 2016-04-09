% Laws' method and k-means
%
clear all;
row=512;  col=512;
% k= 4, 4 known classes/textures
k = 4;
%load the raw image
raw_image='raw\sample8.raw';
fin=fopen(raw_image,'r');
I=fread(fin,row*col,'uint8=>uint8');
fclose(fin);
imageG=reshape(I,row,col);
imageG=imageG';
figure('name',raw_image),imshow(imageG,'Border','tight');
print(gcf, '-dpng', 'sample8.raw');

%crop some textures inside cheetah. The textures are actually zebra's.
hold on;
%[x y w h]
rectangle('Position',[103 303 177 105], 'LineWidth',2, 'EdgeColor','b');
hold on;
rectangle('Position',[180-10 340-10 20 20], 'LineWidth',2, 'EdgeColor','b');
zebra_texture = imageG(303:408, 103:280);	% it contains zebra texture
zebra = repmat(zebra_texture, ceil(row/105), ceil(col/177) );
zebra=zebra(1:row,1:col);

%crop some textures inside zebra. The texture are actually giraffe's.
hold on;
rectangle('Position',[70 90 118 54], 'LineWidth',2, 'EdgeColor','g');
hold on;
rectangle('Position',[140-20 120-20 40 40], 'LineWidth',2, 'EdgeColor','g');
giraffe_texture = imageG(90:144, 70:188);	%it contains giraffe texture
giraffe = repmat(giraffe_texture, ceil(row/54), ceil(col/118) );
giraffe = giraffe(1:row,1:col);

%crop some textures inside giraffe. The texture are actually cheetah's.
hold on;
rectangle('Position',[450 168 46 136], 'LineWidth',2, 'EdgeColor','r');
hold on;
rectangle('Position',[460-10 220-10 20 20], 'LineWidth',2, 'EdgeColor','r');
cheetah_texture = imageG(168:304, 450:496);	%it contains cheetah texture
cheetah = repmat(cheetah_texture, ceil(row/136), ceil(col/46) );
cheetah = cheetah(1:row,1:col);
print(gcf, '-dpng', '3 texture bounding box');

%tile up the single texture to a full image
figure('name','zebra texture'),imshow(zebra,'Border','tight');
print(gcf, '-dpng', 'zebra texture');
figure('name','giraffe texture'),imshow(giraffe,'Border','tight');
print(gcf, '-dpng', 'giraffe texture');
figure('name','cheetah texture'),imshow(cheetah,'Border','tight');
print(gcf, '-dpng', 'cheetah texure');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%giraffe needs smaller window size
window_sizes = [7; 25 ; 15]; %usually 13x13 or 15x15
%3x3 filters = 9 filters
filter_types_3x3 = ['L3L3'; 'L3E3'; 'L3S3';...
    'E3L3'; 'E3E3'; 'E3S3';...
    'S3L3'; 'S3E3'; 'S3S3'];
%5x5 filters = 25 filters
filter_types_5x5 = ['L5L5'; 'L5E5'; 'L5S5'; 'L5W5'; 'L5R5';...
    'E5L5'; 'E5E5'; 'E5S5'; 'E5W5'; 'E5R5';...
    'W5L5'; 'W5E5'; 'W5S5'; 'W5W5'; 'W5R5';...
    'S5L5'; 'S5E5'; 'S5S5'; 'S5W5'; 'S5R5';...
    'R5L5'; 'R5E5'; 'R5S5'; 'R5W5'; 'R5R5'];
%column-wise by ";" seperator in [], computing features by
statistic_types = ['ENRG' ; 'ABSM'; 'STDD'; 'MEAN' ];
normalizing_types = ['MINMAX'; 'FORCON'];
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  for ws = 1: size(window_sizes,1),
    window_size = window_sizes(ws,:);
    normalizing_type = normalizing_types(1, :);

	for st = 1: size(statistic_types, 1),
	    statistic_type = statistic_types(st, :); % ENRG

	    disp(sprintf('>>>%d : window_size=%d',ws, window_size));
	    %%%%%%%%%%%%%%%%%%%%%%
		% 3x3 filters
		%%%%%%%%%%%%%%%%%%%%%%
	    for i = 1 : size(filter_types_3x3,1),
			temporary = Laws_mask(imageG, filter_types_3x3(i,:), window_size, statistic_type, normalizing_type);
			% The result matrix, D, concatnates the Laws_mask result matrixs, M * N, to a 3D matrix.
			% D is M * N * 9, if filter is 3x3
			if(i == 1),
			D = cat(3,temporary);%merge 2D M to 3D matrix. Each M is indexed by the 3rd dimension
			else
			D = cat(3,D,temporary);
			end
			figure(1+ st*10 + 100*ws);
			%subplot(3,3,i); imshow(temporary); title([filter_types_3x3(i,:) ' , ' statistic_type ' , ' normalizing_type]);
			subplot(3,3,i); imshow(temporary); title(filter_types_3x3(i,:));
	    end
	    fig_name=sprintf('feature-f3x3 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
	    print(gcf, '-dpng', fig_name);	
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		%k-means
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	    [label, model, final_texture] = kmeans(D,k, zebra, cheetah, giraffe);
	    fig_name=sprintf('f-3x3 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
	    figure('name', fig_name),imshow(label, 'Border','tight', 'Colormap',jet(255));
	    print(gcf, '-dpng', fig_name);

	    fig_name=sprintf('t-3x3 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
	    figure('name', fig_name),imshow(final_texture, 'Border','tight');
	    print(gcf, '-dpng', fig_name);
	
	    %%%%%%%%%%%%%%%%%%%%%%
		% 5x5 filters
		%%%%%%%%%%%%%%%%%%%%%%	
	    for i = 1 :  size(filter_types_5x5,1),
			temporary = Laws_mask(imageG, filter_types_5x5(i,:), window_size, statistic_type, normalizing_type);
			% The result matrix, D, concatnates the Laws_mask result matrixs, M * N, to a 3D matrix.
			% D is M * N * 9, if filter is 3x3
			if(i == 1),
			D = cat(3,temporary); %merge 2D Mi to 3D matrix along 3rd dimension, 3x3 or 5x5. Each M is indexed by the 3rd dimension
			else
			D = cat(3,D,temporary);
			end
			figure(2+ 10*st + 100*ws); subplot(5,5,i); imshow(temporary); 
			%title([filter_types_5x5(i,:) ' , ' statistic_type ' , ' normalizing_type]);
			title(filter_types_5x5(i,:));
	    end
	    fig_name=sprintf('feature-f5x5 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
	    print(gcf, '-dpng', fig_name);
	
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		%k-means
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	    [label, model] = kmeans(D,k, zebra, cheetah, giraffe);
	    fig_name=sprintf('f-5x5 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
	    figure('name', fig_name),imshow(label, 'Border','tight', 'Colormap',jet(255));
	    print(gcf, '-dpng', fig_name);  %save figure

	    fig_name=sprintf('t-5x5 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
	    figure('name', fig_name),imshow(final_texture, 'Border','tight');
	    print(gcf, '-dpng', fig_name);
	end

  disp(sprintf('<<<%d : window_size=%d',ws, window_size));
  end
