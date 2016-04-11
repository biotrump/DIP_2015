clear;
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
rectangle('Position', [1 1 col gourd(1)-1], 'LineWidth',1, 'EdgeColor','r');
rectangle('Position', [1 gourd(1)+1  col gourd(2)-1], 'LineWidth',1, 'EdgeColor','g');
rectangle('Position', [1 gourd(1)+gourd(2)+1 col gourd(3)-1], 'LineWidth',1, 'EdgeColor','b');

%[x y] = meshgrid(-224/2:224/2, -224/2:224/2);
%imagesc(x);
%imagesc(sqrt(x.^2 + y.^2));
%imagesc(sqrt((x-64).^2 + y.^2));
%%%%%%%%%
%the first block : subsampling from 512x64 to 32x64 rectangle
%%%%%%%
w=32;
%gourd_head=int8(zeros(gourd(1),w));
for s=1:w
    gourd_head(1:gourd(1),s:s)=imageG(1:gourd(1), s*col/w:s*col/w);
end
figure('name','head'),imshow(gourd_head,'Border','tight');

%The 2nd block : circle
%center of circle
cx=gourd(2)/2;
cy=gourd(2)/2;
%center of the 2nd part of the stitching image
csx=col/2;
csy=gourd(1)+ gourd(2)/2 + 1;
%gourd_circle=zeros(gourd(2),gourd(2));
R=gourd(2)/2;
%CC=repmat([gourd(2) 1], 9, 1);
CC=repmat([csx csy], 8, 1);
%9 control point pairs, may be less
fixedPoints=[1 gourd(1)+1 ; col/2 gourd(1)+1 ; col gourd(1)+1 ;...
            1 gourd(1)+1+gourd(2)/2; col gourd(1)+1+gourd(2)/2;...
            1 gourd(1) + gourd(2) ; col/2 gourd(1) + gourd(2) ; col gourd(1) + gourd(2)]; %M*2
fixedPoints = fixedPoints - CC;
%on the circle
slice=pi/4;
R=gourd(2)/2;
movingPoints=[R*cos(3*slice) R*sin(3*slice) ; R*cos(2*slice) R*sin(2*slice) ;R*cos(1*slice) R*sin(1*slice) ;...
          R*cos(4*slice) R*sin(4*slice) ; R*cos(0*slice) R*sin(0*slice) ;...
          R*cos(5*slice) R*sin(5*slice)  ;R*cos(6*slice) R*sin(6*slice)  ; R*cos(7*slice) R*sin(7*slice) ];
degree=2;
tform = fitgeotrans(movingPoints, fixedPoints,'polynomial',degree);

%w=imwarp(imageG,tform);
%figure('name','circlexx'),imshow(w,'Border','tight');
angle = atan(R/(col/2));
% rectangle boundary: r/R = d/D, d = rD/R
D = sqrt((col/2)^2 + R^2);

for i=1: gourd(2),
  for j=1: col,
    dy=(cy-i);  %to cartesian , origin @ center
    dx=(j-cx);
    [theta, deg]=circle_2pi(dx,dy);
    rr = dx^2 + dy^2;
    r = sqrt(rr);
    d = r * D / R;  %
    l = d * cos(angle);
    if r <= R,  %(x,y) is inside a circle
       %[X,Y] = transformPointsInverse(tform,dx,dy);
       x=0;y=0;
       if theta >= angle && theta <= (pi-angle),
           y =r;
           x = r * cot(theta);
       elseif theta >= (pi+angle) && theta <= (2*pi-angle),
           y=-r;
           x = r * cot(theta);
       elseif theta > (pi-angle) && theta < (pi+angle),
           x = -l;
           y=l*tan(theta);
       elseif theta < angle || theta > (2*pi-angle),
           x = l;
           y=l*tan(theta);
       end
       gx=int16(x+csx)+1;
       gy=int16(csy-y)+1;
         %translate to stitching coordination
         if gx > col ,
             gx = col;
         end
         if gx < 1 ,
             gx = 1;
         end
         if gy >  gourd(1)+gourd(2),
             gy = gourd(1)+gourd(2);
         end
         if gy <= gourd(1),
             gy = gourd(1)+1;
         end
        gourd_circle(i,j)= imageG(gx, gy);
    end
  end
end
figure('name','circle'),imshow(gourd_circle,'Border','tight');

%The 3rd block: oval
rx=col/2;
ry=gourd(3)/2;
Rxy = (rx* ry)^2;

ocx=col/2;
ocy=gourd(3)/2;

for i=1: gourd(3),
  for j=1: col,
    dy=(ocy-i);
    dx=(j-ocx);
    rr = (ry * dx)^2 + (rx * dy)^2;
    %r = sqrt(rr);
    if rr <= Rxy,  %inside a circle
       [Y, X] = transformPointsInverse(tform,dx,dy);
       gx=int16(X+ocx);
       gy=int16(ocy-Y)+gourd(2)+gourd(1);
         %translate to stitching coordination
        gourd_oval(i,j)= imageG(gx, gy);
    end
  end
end
figure('name','oval'),imshow(gourd_oval,'Border','tight');

gourd_final=uint8(zeros(row,col));
gourd_final(1:gourd(1),(col-w)/2+1:(col-w)/2+w)=gourd_head;
gourd_final(gourd(1)+1:gourd(1)+gourd(2),col/2-gourd(2)/2:col/2+gourd(2)/2-1)=gourd_circle;
gourd_final(gourd(1)+gourd(2)+1:gourd(1)+gourd(2)+gourd(3),1:col)=gourd_oval;
figure('name','final'),imshowpair(imageG,gourd_final,'montage');
rectangle('Position', [1 1 col gourd(1)-1], 'LineWidth',1, 'EdgeColor','r');
rectangle('Position', [1 gourd(1)+1  col gourd(2)-1], 'LineWidth',1, 'EdgeColor','g');
rectangle('Position', [1 gourd(1)+gourd(2)+1 col gourd(3)-1], 'LineWidth',1, 'EdgeColor','b');
print(gcf, '-dpng', 'image-warp');
        