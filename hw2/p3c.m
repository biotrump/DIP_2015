[x y] = meshgrid(-224/2:224/2, -224/2:224/2);

%imagesc(x);
imagesc(sqrt(x.^2 + y.^2));
%imagesc(sqrt((x-64).^2 + y.^2));
