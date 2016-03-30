%p1a : LOG 2nd order edge detection
% sample files are under "./raw" folder.
%sample1.raw, sample2.raw, sample3.raw : 256x256 gray-scale
row=256;  col=256;
method='log';
direction='both';%'horizontal' | 'vertical'
%load image 
raw_image='raw\sample1.raw';
fin=fopen(raw_image,'r');
I=fread(fin,row*col,'uint8=>uint8');
fclose(fin);
S1=reshape(I,row,col);
S1=S1';
%figure,imshow(S1, 'Border','tight');
[BW1,threshOut] = edge(S1,method);
threshold = threshOut;
BW1 = edge(S1,method,threshold,direction);
%figure,imshow(BW1, 'Border','tight');
figure('name',[raw_image '+' method]),imshowpair(S1,BW1,'montage');
%
raw_image='raw\sample2.raw';
fin=fopen(raw_image,'r');
I=fread(fin,row*col,'uint8=>uint8');
fclose(fin);
S2=reshape(I,row,col);
S2=S2';
%figure,imshow(S2, 'Border','tight');
BW2 = edge(S2,method,threshold,direction);
%figure,imshow(BW2, 'Border','tight');
figure('name',[raw_image '+' method]),imshowpair(S2,BW2,'montage');
%
raw_image='raw\sample3.raw';
fin=fopen(raw_image,'r');
I=fread(fin,row*col,'uint8=>uint8');
fclose(fin);
S3=reshape(I,row,col);
S3=S3';
%figure,imshow(S3, 'Border','tight');
BW3 = edge(S3,method,threshold,direction);
%figure,imshow(BW3, 'Border','tight');
figure('name',[raw_image '+' method]),imshowpair(S3,BW3,'montage');
%
%show all results
figure('name',[method ' Edge Detection']);
subplot(2,3,1),imshow(S1);
subplot(2,3,2),imshow(S2);
subplot(2,3,3),imshow(S3);
subplot(2,3,4),imshow(BW1);
subplot(2,3,5),imshow(BW2);
subplot(2,3,6),imshow(BW3);
