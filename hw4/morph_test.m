function morph_test(row,col,raw_image)
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

    se=strel('disk',4,0);%Structuring element
    %F=imerode(S1,se);%Erode the image by structuring element
    F=bmorph('erode', S1, se.getnhood, 5, 5);
    %Difference between binary image and Eroded image
    Boundary=S1-F;
    figure('name','morphology'),imshow(Boundary);title('Boundary extracted Image');

    output=bmorph(method, inimg, se, origRow, origCol);
end