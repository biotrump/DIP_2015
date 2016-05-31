% PROBLEM 3: Frequency-Domain Filtering
% Implementation: main program of Problem 3
% M-file name: p3.m
% Usage: p3
% Output image:
% Parameters: no
% p3 loads Sample3.raw, 256x256, image. It will do ideal low pass filter,
% gaussian low pass filtering, centering DFT, IDFT
function p3()
    close all;
    row=256;  col=256;
    raw_image='sample3.raw';
    fin=fopen(raw_image,'r');
    I=fread(fin,row*col,'uint8=>uint8');
    fclose(fin);
    S=reshape(I,row,col);
    S1=S;
    S1=S1';%image is in row-major, but matlab uses col-major
    figure('name','p3(a)');
    subplot(2,2,1);imshow(S1);title(strcat('I:',raw_image));
    colorbar;

    %centering before DFT
    S1_centered=dft_centering(S1);
    subplot(2,2,2);imshow(uint8(S1_centered));title('centering I');
    colorbar;

    %DFT2D
    dft2d=DFT2D(S1_centered);
    dft2d_I = log(abs(dft2d));%the dynamic range of frequency power spectra is too large,
    subplot(2,2,3); imshow(dft2d_I,[0 20],'InitialMagnification','fit');title('D: Log DFT2D');
    %colormap(jet);
    colorbar;
    %imwrite(dft2d_I, 'D.png');

    %IDFT2D: only real part is valid in iDFT
    iS1=int16(real(IDFT2D(dft2d)));%result is centered image
    %reverse centering for iDFT
    iS1=dft_centering(iS1);
    iS1=uint8(iS1);
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    %figure('name','inverse DFT2D');
    subplot(2,2,4); imshow(iS1);title('IDFT2D');colorbar;
    %check error between original and inverse
    fprintf('error between original and inverse DFT: %f\n',norm(single(iS1-S1)));

    figure('name','Problem 3a');imshow(dft2d_I,[0 20],'InitialMagnification','fit');title('D');

    %matlab verification
    %fft2_out=fft2(S);
    %rS=uint8(real(ifft2(fft2_out)));
    %figure;imshow(S);
    %figure;imshow(rS);title('ifft2');
    %norm(single(rS-S))

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %p3b : ideal low pass filter
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %D0=5
    lpD5=ideal_lp(dft2d, 5);
    lpD5_I = log(abs(lpD5));
    figure('name','P3(b)');
    subplot(2,2,1); imshow(lpD5_I,[0 20],'InitialMagnification','fit');title('L5');
    colorbar;

    %D0=30
    lpD30=ideal_lp(dft2d, 30);
    lpD30_I = log(abs(lpD30));
    subplot(2,2,2); imshow(lpD30_I,[0 20],'InitialMagnification','fit');title('L30');
    colorbar;

    %IDFT2D: only real part is valid in iDFT
    ilpD5=int16(real(IDFT2D(lpD5)));%result is centered image
    %reverse centering for iDFT
    ilpD5=dft_centering(ilpD5);
    ilpD5=uint8(ilpD5);
    subplot(2,2,3);imshow(ilpD5);title('inverse L5');
    colorbar;

    %IDFT2D: only real part is valid in iDFT
    ilpD30=int16(real(IDFT2D(lpD30)));%result is centered image
    %reverse centering for iDFT
    ilpD30=dft_centering(ilpD30);
    ilpD30=uint8(ilpD30);
    subplot(2,2,4);imshow(ilpD30);title('inverse L30');%rining effect
    colorbar;

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %p3c : Gaussian low pass filter
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %D0=5
    glpD5=glp(dft2d, 5);
    glpD5_I = log(abs(glpD5));
    figure('name','P3(c)');
    subplot(2,2,1); imshow(glpD5_I,[0 20],'InitialMagnification','fit');title('G5');
    colorbar;

    %D0=30
    glpD30=glp(dft2d, 30);
    glpD30_I = log(abs(glpD30));
    subplot(2,2,2); imshow(glpD30_I,[0 20],'InitialMagnification','fit');title('G30');
    colorbar;

    %IDFT2D: only real part is valid in iDFT
    iglpD5=int16(real(IDFT2D(glpD5)));%result is centered image
    %reverse centering for iDFT
    iglpD5=dft_centering(iglpD5);
    iglpD5=uint8(iglpD5);
    subplot(2,2,3);imshow(iglpD5);title('inverse G5');
    colorbar;

    %IDFT2D: only real part is valid in iDFT
    iglpD30=int16(real(IDFT2D(glpD30)));%result is centered image
    %reverse centering for iDFT
    iglpD30=dft_centering(iglpD30);
    iglpD30=uint8(iglpD30);
    subplot(2,2,4);imshow(iglpD30);title('inverse G30');%rining effect
    colorbar;
end
