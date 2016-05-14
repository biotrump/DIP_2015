%image : rowxcol uint8
function centered=dft_centering(image)
    [row,col]=size(image);
    centered=int16(image);
        for i=1:row,
            for j=1:col,
                if mod((i+j), 2) ~=0,%dd index to toggle
                    centered(i,j)=-centered(i,j);%f(i,j)*(-1)^(i+j)
                end
             end
        end
end