%http://stackoverflow.com/questions/29315365/2d-discrete-fourier-transform-implementation-in-matlab
% signal is a matrix of MxN size
%Matrix Form of 2D DFT
%http://fourier.eng.hmc.edu/e161/lectures/fourier/node11.html
%DFT : X = W_M' * x * W_N'
%IDFT : x = W_M * X * W_N
% where W, fourier Transform matrix, is a symmetric unitary matrix: W^-1= W'=W
% https://en.wikipedia.org/wiki/Unitary_matrix
% U is invertible with U^?1 = U'. U' : conjugate transpose for complex
% matrix
%http://www.katjaas.nl/fourier/fourier.html => fourier Transform Matrix
function res=DFT2D(signal)
    signal=double(signal);

    N=size(signal,2);%M*N
    x=[1:N];%1,2,3...M

    %x=u'
    [x, u]=meshgrid(x,x);%meshgrid(x,x) = [1 2 ..M;1 2 ..M;1 2..M;...], M*M 

    %http://www.mathworks.com/help/matlab/ref/exp.html
    %Y = exp(1i*pi) = -1.0000 + 0.0000i
    Wn=exp(1i*(-2*pi)./N); %basis : sample harmonic signal
    %http://fourier.eng.hmc.edu/e161/lectures/fourier/node11.html
    pre_dft=(Wn.^((u-1).*(x-1)));

    post_dft = pre_dft.'; %".'" only transpose matrix, but ' is "complex conjugate transform!!! 
    %1D to 2D
    res = pre_dft*signal*post_dft;
end
