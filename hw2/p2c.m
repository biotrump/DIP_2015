clear all;
row=512;  col=512;
gourd=[64 224 224];

%load the stitching 512x512
raw_image='stitch-512x512.raw';
fin=fopen(raw_image,'r');
I=fread(fin,row*col,'uint8=>uint8');
fclose(fin);
imageG=reshape(I,row,col);
imageG=imageG';
figure('name',raw_image),imshow(imageG,'Border','tight');

%[x y] = meshgrid(-224/2:224/2, -224/2:224/2);
%imagesc(x);
%imagesc(sqrt(x.^2 + y.^2));
%imagesc(sqrt((x-64).^2 + y.^2));

%the first block : rectangle

%The 2nd block : circle
%center of circle
cx=gourd(2)/2;
cy=gourd(2)/2;
%center of the 2nd part of the stitching image
csx=col/2;
csy=gourd(1)+ row/2 + 1;
gourd_circle=zeros(gourd(2),gourd(2));
for i=1: gourd(2),
  for j=1: gourd(2),
    dy=(i-cy);
    dx=(j-cx);
    rr = dx^2 + dy^2;
    r = sqrt(rr);
    if r >= 1
      [theta, deg]=circle_2pi(dx, dy);
      %stitching x
      if dy >= 0,
        sy = r;
      else
        sy = -r;
      end
      sx=cot(theta)*sy;
    else
    %center
      disp('center');
    end
    sx=int16(sx);
    sy=int16(sy);
    gx=sx+csx;
    gy=sy+csy;
    if gx < 1, 
      gx=1;
    end
    if gx > col, 
      gx=col;
    end
    if gy <= gourd(1), 
      gy=gourd(1)+1;
    end
    if gy > (gourd(1) + gourd(1) ) , 
      gx=(gourd(1) + gourd(1) );    
    end
    
    %translate to stitching coordination
    %gourd_circle(i,j)= imageG(gy, gx);
  end
end
figure('name','circle'),imshow(gourd_circle,'Border','tight');

%The 3rd block: oval