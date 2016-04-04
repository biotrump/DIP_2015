% Laws' method and k-means
%
clear all;
row=512;  col=512;
% k= 4, 4 known classes/textures
k = 4;

raw_image='raw\sample8.raw';
fin=fopen(raw_image,'r');
I=fread(fin,row*col,'uint8=>uint8');
fclose(fin);
imageG=reshape(I,row,col);
imageG=imageG';
figure('name',raw_image),imshow(imageG,'Border','tight');

%feature computation
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

  for ws = 1: size(window_sizes,1),
    window_size = window_sizes(ws,:);
    normalizing_type = normalizing_types(1, :);
    statistic_type = statistic_types(1, :); % ENRG

    disp(sprintf('>>>%d : window_size=%d',ws, window_size));
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
        figure(1+ 100*ws);
      subplot(3,3,i); imshow(temporary); title([filter_types_3x3(i,:) ' , ' statistic_type ' , ' normalizing_type]);
    end
    fig_name=sprintf('feature-f3x3 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    print(gcf, '-dpng', fig_name);

    [label, model] = kmeans(D,k);
    fig_name=sprintf('f-3x3 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    figure('name', fig_name),imshow(label, 'Border','tight', 'Colormap',jet(255));
    print(gcf, '-dpng', fig_name);

    for i = 1 :  size(filter_types_5x5,1),
    	temporary = Laws_mask(imageG, filter_types_5x5(i,:), window_size, statistic_type, normalizing_type);
      % The result matrix, D, concatnates the Laws_mask result matrixs, M * N, to a 3D matrix.
      % D is M * N * 9, if filter is 3x3
      if(i == 1),
        D = cat(3,temporary); %merge 2D Mi to 3D matrix along 3rd dimension, 3x3 or 5x5. Each M is indexed by the 3rd dimension
      else
        D = cat(3,D,temporary);
      end
    	figure(2+ 100*ws); subplot(5,5,i); imshow(temporary); title([filter_types_5x5(i,:) ' , ' statistic_type ' , ' normalizing_type]);
    end
    fig_name=sprintf('feature-f5x5 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    print(gcf, '-dpng', fig_name);

    [label, model] = kmeans(D,k);
    fig_name=sprintf('f-5x5 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    figure('name', fig_name),imshow(label, 'Border','tight', 'Colormap',jet(255));
    print(gcf, '-dpng', fig_name);  %save figure
    %%%%%%%%%%%%
    statistic_type = statistic_types(2, :); % ABSM
    for i = 1 : size(filter_types_3x3,1),
    	temporary = Laws_mask(imageG, filter_types_3x3(i,:), window_size, statistic_type, normalizing_type);
      % The result matrix, D, concatnates the Laws_mask result matrixs, M * N, to a 3D matrix.
      % D is M * N * 9, if filter is 3x3
      if(i == 1),
        D = cat(3,temporary);%merge 2D M to 3D matrix. Each M is indexed by the 3rd dimension
      else
        D = cat(3,D,temporary);
      end

    	figure(3+ 100*ws); subplot(3,3,i); imshow(temporary); title([filter_types_3x3(i,:) ' , ' statistic_type ' , ' normalizing_type]);
    end
    fig_name=sprintf('feature-f3x3 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    print(gcf, '-dpng', fig_name);

    [label, model] = kmeans(D,k);
    fig_name=sprintf('f-3x3 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    figure('name', fig_name),imshow(label, 'Border','tight', 'Colormap',jet(255));
    print(gcf, '-dpng', fig_name);  %save figure

    for i = 1 : size(filter_types_5x5,1),
    	temporary = Laws_mask(imageG, filter_types_5x5(i,:), window_size, statistic_type, normalizing_type);
      % The result matrix, D, concatnates the Laws_mask result matrixs, M * N, to a 3D matrix.
      % D is M * N * 9, if filter is 3x3
      if(i == 1),
        D = cat(3,temporary);%merge 2D M to 3D matrix. Each M is indexed by the 3rd dimension
      else
        D = cat(3,D,temporary);
      end

    	figure(4+ 100*ws); subplot(5,5,i); imshow(temporary); title([filter_types_5x5(i,:) ' , ' statistic_type ' , ' normalizing_type]);
    end
    fig_name=sprintf('feature-f5x5 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    print(gcf, '-dpng', fig_name);

    [label,  model] = kmeans(D,k);
    fig_name=sprintf('f-5x5 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    figure('name', fig_name),imshow(label, 'Border','tight', 'Colormap',jet(255));
    print(gcf, '-dpng', fig_name);  %save figure

    %%%%%%%%%%%%
    statistic_type = statistic_types(3, :); % STDD
    for i = 1 : size(filter_types_3x3,1),
    	temporary = Laws_mask(imageG, filter_types_3x3(i,:), window_size, statistic_type, normalizing_type);
      % The result matrix, D, concatnates the Laws_mask result matrixs, M * N, to a 3D matrix.
      % D is M * N * 9, if filter is 3x3
      if(i == 1),
        D = cat(3,temporary);%merge 2D M to 3D matrix. Each M is indexed by the 3rd dimension
      else
        D = cat(3,D,temporary);
      end

    	figure(5+ 100*ws); subplot(3,3,i); imshow(temporary); title([filter_types_3x3(i,:) ' , ' statistic_type ' , ' normalizing_type]);
    end
    fig_name=sprintf('feature-f3x3 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    print(gcf, '-dpng', fig_name);

    [label, model] = kmeans(D,k);
    fig_name=sprintf('f-3x3 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    figure('name', fig_name),imshow(label, 'Border','tight', 'Colormap',jet(255));
    print(gcf, '-dpng', fig_name);  %save figure

    for i = 1 : size(filter_types_5x5,1),
    	temporary = Laws_mask(imageG, filter_types_5x5(i,:), window_size, statistic_type, normalizing_type);
      % The result matrix, D, concatnates the Laws_mask result matrixs, M * N, to a 3D matrix.
      % D is M * N * 9, if filter is 3x3
      if(i == 1),
        D = cat(3,temporary);%merge 2D M to 3D matrix. Each M is indexed by the 3rd dimension
      else
        D = cat(3,D,temporary);
      end

    	figure(6+ 100*ws); subplot(5,5,i); imshow(temporary); title([filter_types_5x5(i,:) ' , ' statistic_type ' , ' normalizing_type]);
    end
    fig_name=sprintf('feature-f5x5 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    print(gcf, '-dpng', fig_name);

    [label, model] = kmeans(D,k);
    fig_name=sprintf('f-5x5 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    figure('name', fig_name),imshow(label, 'Border','tight', 'Colormap',jet(255));
    print(gcf, '-dpng', fig_name);  %save figure

    %%%%%%%%%%%%
    statistic_type = statistic_types(4, :); % ENRG
    for i = 1 : size(filter_types_3x3,1),
    	temporary = Laws_mask(imageG, filter_types_3x3(i,:), window_size, statistic_type, normalizing_type);
      % The result matrix, D, concatnates the Laws_mask result matrixs, M * N, to a 3D matrix.
      % D is M * N * 9, if filter is 3x3
      if(i == 1),
        D = cat(3,temporary);%merge 2D M to 3D matrix. Each M is indexed by the 3rd dimension
      else
        D = cat(3,D,temporary);
      end

    	figure(7 + 100*ws); subplot(3,3,i); imshow(temporary); title([filter_types_3x3(i,:) ' , ' statistic_type ' , ' normalizing_type]);
    end
    fig_name=sprintf('feature-f3x3 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    print(gcf, '-dpng', fig_name);

    [label, model] = kmeans(D,k);
    fig_name=sprintf('f-3x3 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    figure('name', fig_name),imshow(label, 'Border','tight', 'Colormap',jet(255));
    print(gcf, '-dpng', fig_name);  %save figure

    for i = 1 : size(filter_types_5x5,1),
    	temporary = Laws_mask(imageG, filter_types_5x5(i,:), window_size, statistic_type, normalizing_type);
      % The result matrix, D, concatnates the Laws_mask result matrixs, M * N, to a 3D matrix.
      % D is M * N * 9, if filter is 3x3
      if(i == 1),
        D = cat(3,temporary);%merge 2D M to 3D matrix. Each M is indexed by the 3rd dimension
      else
        D = cat(3,D,temporary);
      end

    	figure(8 + 100*ws ); subplot(5,5,i); imshow(temporary); title([filter_types_5x5(i,:) ' , ' statistic_type ' , ' normalizing_type]);
    end
    fig_name=sprintf('feature-f5x5 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    print(gcf, '-dpng', fig_name);

    [label, model] = kmeans(D,k);
    fig_name=sprintf('f-5x5 win_size-%dx%d %s-%s', window_size, window_size, statistic_type, normalizing_type);
    figure('name', fig_name),imshow(label, 'Border','tight', 'Colormap',jet(255));
    print(gcf, '-dpng', fig_name);  %save figure

  disp(sprintf('<<<%d : window_size=%d',ws, window_size));
  end
