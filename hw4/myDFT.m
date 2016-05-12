%http://stackoverflow.com/questions/29315365/2d-discrete-fourier-transform-implementation-in-matlab
function res=myDFT(signal)
    signal=double(signal);

    l=size(signal,1);
    x=[1:l];

    [x, u]=meshgrid(x,x);

    %// Error #1
    %http://www.mathworks.com/help/matlab/ref/exp.html
    %Y = exp(1i*pi) = -1.0000 + 0.0000i
    pre_dft=exp(1i*(-2*pi)./l); %// Change

    %// Error #2
    pre_dft=(pre_dft.^((u-1).*(x-1))); %// Change

    %// Error #3
    post_dft = pre_dft.'; %// Change

    res = pre_dft*signal*post_dft;
end
