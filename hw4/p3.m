function p1(row, col, raw_image)
    close all;
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
    S1=S1';%image is in row-major, but matlab uses col0-major
    figure;
    imshow(S1);title(raw_image);

    %h = histogram(S1,256, 'BinMethod','sturges');
    figure;
    h = histogram(S1,256);
    %h.NumBins
    %bin counts
    h.Values
    %otsu threshold
    S1(S1 <236 )=0;
    %S1(S1 <= 4)=0;
    figure;
    imshow(S1);title('binary threshold');

    rng(123123);
    in = rand(7);
    out1 = DFT2D(in);
    out2 = fft2(in);
    out1
    out2
end
