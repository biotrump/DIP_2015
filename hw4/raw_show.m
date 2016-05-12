function raw_show(row, col, raw_image)
    if nargin < 3
        fprintf('raw_show syntax : row col raw_image\n');
        return;
    end
    %Sample1.raw : 256x256
    %Sample2.raw : 256x256
    %Sample3.raw : 256x256
    %TrainingSet.raw : 450 x 248
    %row=324;  col=324;
    %raw_image='..\raw\sample4.raw';
    fin=fopen(raw_image,'r');
    I=fread(fin,row*col,'uint8=>uint8');
    fclose(fin);
    S1=reshape(I,row,col);
    S1=S1';
    figure;
    imshow(S1);
    
    %k=figure,imshow(S1);
    %corners1 = detectFASTFeatures(S1);
    %figure,imshow(S1); hold on;
    %plot(corners1.selectStrongest(50));

end
