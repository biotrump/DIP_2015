close all 
clear all 
 
image=imread('zhu1.tif');   % Read the Colour Image as the Input Inmage 
img=rgb2gray(image);        % Convert Colour Image to Grey-Level Image 
 
% Define Law masks 
%Law mask size 3x3 
mask_l3l3=[1 2 1;2 4 2; 1 2 1]; 
mask_l3e3=[-1 0 1;-2 0 2;-1 0 1]; 
mask_l3s3=[-1 2 -1;-2 4 -2;-1 2 -1]; 
mask_e3l3=[-1 -2 -1; 0 0 0;1 2 1]; 
mask_e3e3=[1 0 -1;0 0 0;-1 0 1]; 
mask_e3s3=[1 -2 1;0 0 0;-1 2 -1]; 
mask_s3l3=[-1 -2 -1;2 4 2;-1 -2 -1]; 
mask_s3e3=[1 0 -1;-2 0 2;1 0 -1]; 
mask_s3s3=[1 -2 1;-2 4 -2;1 -2 1]; 
 
%Law mask size 5x5 
mask_e5l5=[-1 -4 -6 -4 -1;-2 -8 -12 -8 -2;0 0 0 0 0;2 8 12 8 2;1 4 6 4 1]; 
mask_r5r5=[1 -4 6 -4 1;-4 16 -24 16 -4;6 -24 36 -24 6;-4 16 -24 16 -4;1 -4 6 -4 1]; 
mask_e5s5=[-1 0 2 0 -1;-2 0 4 0 -2;0 0 0 0 0;2 0 -4 0 2;1 0 -2 0 1]; 
mask_l5s5=[-1 0 2 0 -1;-4 0 8 0 -4;-6 0 12 0 -6;-4 0 8 0 -4;-1 0 2 0 -1]; 
 
mask={mask_l3l3,mask_l3e3,mask_l3s3,mask_e3l3,mask_e3e3,mask_e3s3,mask_s3l3,mask_s3e3,mask_s3s3,mask_e5l5,mask_r5r5,mask_e5s5,mask_l5s5}; 
 
% Define the window size for claculating the statistic energy 
window_size=input('The window size is \n'); 
 
for n=1:13 
    conv_img=filter2(mask{n},img);      % Convolution between Mask and the texture Image 
    img_size=size(conv_img); 
    width=img_size(1); 
    height=img_size(2); 
        % Compute Mean 
        for i = 1:width-window_size 
            for j = 1:height-window_size 
                sub_img = conv_img(i : i+ window_size -1 , j : j + window_size -1 ) ;  
                mean_img(i,j)= mean2(sub_img); 
            end 
        end 
                  
        figure(1), 
        subplot(4,4,n),title(''),imagesc(double(mean_img)) 
 
        % Compute Absolute Mean 
        for i = 1:width-window_size 
            for j = 1:height-window_size 
                sub_img = conv_img(i : i+ window_size -1 , j : j + window_size -1 ) ;  
                sub_img_ = abs(sub_img); 
                absmean_img(i,j)= mean2(sub_img_); 
            end 
        end 
        %% Get the total value to normalize the image 
        totabsmean_img = sum(sum(absmean_img)); 
        %% Normalization of the data 
        absmean_img = absmean_img/totabsmean_img; 
        figure(2), 
        subplot(4,4,n),title(''),imagesc(double(absmean_img)) 
         
        % Compute SD 
        for i = 1:width-window_size 
            for j = 1:height-window_size 
                sub_img = conv_img(i : i+ window_size -1 , j : j + window_size -1 ) ;  
                std_img(i,j)= std2(sub_img); 
            end 
        end 
        %% Get the minimum value to shift all the value 
        minival = min(min(std_img)); 
        %% Get the total value to normalize the image 
        totalstd_img = sum(sum(std_img)); 
        %% Shift all the data to become positive 
        std_img = std_img + minival; 
        %% Normalization of the data 
        std_img = std_img / totalstd_img; 
        std_img = std_img *255; 
        figure(3), 
        subplot(4,4,n),title(''),imagesc(double(std_img)) 
end 