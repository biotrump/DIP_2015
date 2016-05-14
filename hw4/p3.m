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
    S=reshape(I,row,col);
    S1=S;
    S1=S1';%image is in row-major, but matlab uses col0-major
    
    %centering before DFT
    S1_centered=int16(S1);
    for i=1:row,
        for j=1:col,
            if mod((i+j), 2) ~=0,
                S1_centered(i,j)=-S1_centered(i,j);
            end
         end
    end   

    figure;
    imshow(S1);title(raw_image);

    figure('name','DFT2D');
    dft2d=DFT2D(S1_centered);
    %
    dft2d_I = log(abs(dft2d));
    imshow(dft2d_I,[0 20],'InitialMagnification','fit');title('DFT2D');
    %colormap(jet); colorbar;

    %IDFT2D: only real part is valid in iDFT
    iS1=int16(real(IDFT2D(dft2d)));%result is centered image

    %reverse centering for iDFT
    for i=1:row,
        for j=1:col,
            if mod((i+j), 2) ~=0,
                iS1(i,j)=-iS1(i,j);
            end
         end
    end
    iS1=uint8(iS1);
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    figure('name','inverse DFT2D');
    imshow(iS1);title('IDFT2D');
    %check error between original and inverse
    norm(single(iS1-S1_centered))
    
    %matlab verification
    %fft2_out=fft2(S);
    %rS=uint8(real(ifft2(fft2_out)));
    %figure;imshow(S);
    %figure;imshow(rS);title('ifft2');
    %norm(single(rS-S))
    
end
