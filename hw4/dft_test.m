    close all;
    clear;
    canvas=zeros(256,256,'uint8');
    canvas(124:132,120:136)=ones(132-124+1,136-120+1,'uint8');
    %centering before DFT
    for i=1:256,
        for j=1:256,
            if mod((i+j), 2) ~=0,
                canvas(i,j)=-canvas(i,j);
            end
         end
    end   
    figure;
    imshow(canvas);title('square image');
    dft_out = DFT2D(canvas);
    dft2d_I = log(abs(dft_out));
    figure;
    imshow(dft2d_I,[0 10],'InitialMagnification','fit');title('DFT2D');
    %colormap(jet); colorbar;
    
    %matlab centering
    fft2_out=fft2(canvas);
    fft2_out=fftshift(fft2_out);%centering
    fft2_I = log(abs(fft2_out));
    figure;
    imshow(fft2_I,[0 10],'InitialMagnification','fit');title('matlab fft2  spectrum');
    
    rng(123123);
    in = rand(5);

    dft_out = DFT2D(in);
    iin=real(IDFT2D(dft_out));
    norm(iin-in);
    fprintf('erro : in - iDFT:%f\n',norm(iin-in));

    fft_out = fft2(in);
    ifft_out2=ifft2(fft_out);
    norm(dft_out - fft_out);    %error : fft - dft
