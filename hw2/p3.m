
% Laws' method
row=512;  col=512;
raw_image='raw\sample8.raw';
fin=fopen(raw_image,'r');
I=fread(fin,row*col,'uint8=>uint8');
fclose(fin);
imageG=reshape(I,row,col);
imageG=imageG';

window_sizes = [15 ; 13];
filter_types_3x3 = ['L3L3'; 'L3E3'; 'L3S3';...
    'E3L3'; 'E3E3'; 'E3S3';...
    'S3L3'; 'S3E3'; 'S3S3'];
filter_types_5x5 = ['L5L5'; 'L5E5'; 'L5S5'; 'L5W5'; 'L5R5';...
    'E5L5'; 'E5E5'; 'E5S5'; 'E5W5'; 'E5R5';...
    'S5L5'; 'S5E5'; 'S5S5'; 'S5W5'; 'S5R5';...
    'W5L5'; 'W5E5'; 'W5S5'; 'W5W5'; 'W5R5';...
    'R5L5'; 'R5E5'; 'R5S5'; 'R5W5'; 'R5R5'];
statistic_types = ['MEAN'; 'ABSM'; 'STDD'; 'ENRG'];
normalizing_types = ['MINMAX'; 'FORCON'];
window_size = window_sizes(1,:);
normalizing_type = normalizing_types(1, :);
statistic_type = statistic_types(1, :); % MEAN

op_name= ['filter 3x3 , ' 'win size=' int2str(window_size) ' , ' statistic_type ' , ' normalizing_type ];
for i = 1 : size(filter_types_3x3,1),
	temporary = Laws_mask(imageG, filter_types_3x3(i,:), window_size, statistic_type, normalizing_type);
  % The result matrix, D, concatnates the Laws_mask result matrixs, M * N, to a 3D matrix.
  % D is M * N * 9, if filter is 3x3
  if(i == 1),
    D = cat(3,temporary);%merge 2D M to 3D matrix. Each M is indexed by the 3rd dimension
  else
    D = cat(3,D,temporary);
  end
    figure(1);
  subplot(3,3,i); imshow(temporary); title([filter_types_3x3(i,:) ' , ' statistic_type ' , ' normalizing_type]);
end
%print -f1 -depsc filter3_w15_mean
%get the feature vector of pixel (i,j). The dimension of the feature vector is 9(=3x3)
%squeeze(D(i,j,:));% feature vector as a column vector
% 3D matrix M * N * d => 2D matrix (M * N) * d => total M *N vectors with dimension d
[row, col, depth]=size(D);
X=reshape(D,[row*col depth]);  % feature vector is in row vector, there are total M * N vectors,
X=X';% row vector to colunm vector
k = 4; % 4 classes
[label, energy, model] = kmeans(X,k);
% reshape texture map 1xn to row * col mapping to the original image
map=reshape(label,[row col]);
map = map * 40;
%output_image = im2uint8(map);

%output_image = normalize(map);
%plot the classes on the original map
%plotclass(image, map, k);
%map=zeros(row,col,'int8');
figure,imshow(map,[0 255], 'Border','tight');

for i = 1 :  size(filter_types_5x5,1),
	temporary = Laws_mask(imageG, filter_types_5x5(i,:), window_size, statistic_type, normalizing_type);
	figure(2); subplot(5,5,i); imshow(temporary); title([filter_types_5x5(i,:) ' , ' statistic_type ' , ' normalizing_type]);
end
%print -f2 -depsc filter5_w15_mean
statistic_type = statistic_types(2, :); % ABSM
for i = 1 : size(filter_types_3x3,1),
	temporary = Laws_mask(imageG, filter_types_3x3(i,:), window_size, statistic_type, normalizing_type);
	figure(3); subplot(3,3,i); imshow(temporary); title([filter_types_3x3(i,:) ' , ' statistic_type ' , ' normalizing_type]);
end
%print -f3 -depsc filter3_w15_absm
for i = 1 : size(filter_types_5x5,1),
	temporary = Laws_mask(imageG, filter_types_5x5(i,:), window_size, statistic_type, normalizing_type);
	figure(4); subplot(5,5,i); imshow(temporary); title([filter_types_5x5(i,:) ' , ' statistic_type ' , ' normalizing_type]);
end
%print -f4 -depsc filter5_w5_absm
%
statistic_type = statistic_types(3, :); % STDD
for i = 1 : size(filter_types_3x3,1),
	temporary = Laws_mask(imageG, filter_types_3x3(i,:), window_size, statistic_type, normalizing_type);
	figure(5); subplot(3,3,i); imshow(temporary); title([filter_types_3x3(i,:) ' , ' statistic_type ' , ' normalizing_type]);
end
%print -f5 -depsc filter3_w5_stdd
for i = 1 : size(filter_types_5x5,1),
	temporary = Laws_mask(imageG, filter_types_5x5(i,:), window_size, statistic_type, normalizing_type);
	figure(6); subplot(5,5,i); imshow(temporary); title([filter_types_5x5(i,:) ' , ' statistic_type ' , ' normalizing_type]);
end
%	print -f6 -depsc filter5_w5_stdd
%
statistic_type = statistic_types(4, :); % STDD
for i = 1 : size(filter_types_3x3,1),
	temporary = Laws_mask(imageG, filter_types_3x3(i,:), window_size, statistic_type, normalizing_type);
	figure(7, 'name', 'filter3_w5_erergy'); subplot(3,3,i); imshow(temporary); title([filter_types_3x3(i,:) ' , ' statistic_type ' , ' normalizing_type]);
end
%print -f7 -depsc filter3_w5_erergy
for i = 1 : size(filter_types_5x5,1),
	temporary = Laws_mask(imageG, filter_types_5x5(i,:), window_size, statistic_type, normalizing_type);
	figure(8, 'name', 'filter5_w5_erergy' ); subplot(5,5,i); imshow(temporary); title([filter_types_5x5(i,:) ' , ' statistic_type ' , ' normalizing_type]);
end
%	print -f8 -depsc filter5_w5_erergy
	%save('feli.mat', 'feli');
