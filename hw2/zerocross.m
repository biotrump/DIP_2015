%Finds the zero crossings of the
%2-dimensional function f
function zc = zerocross(f, neighbors, threshold)

%hist(f, 50);
% Get threshold
if (nargin < 3)
  threshold = mean(abs(f(:)));
end

% Find zero crossings
  
if 0,
  f (abs(f) <= theshold) = 0;
  sf = sign(f);
  %z0 = sf==0;
  [R,C] = size(f);
  zc = zeros(R-2,C-2);           %generate "0" matrix
  for i=1:R-2
        for j=1:C-2
            if sf(i+1,j+1) == 0,
                if(neighbors == 8),
              zc(i,j) = (sf(i,j) ~= sf(i+2,j+2)) & (sf(i,j) + sf(i+2,j+2) == 0)  |...
               (sf(i,j+1) ~= sf(i+2,j+1)) & (sf(i,j+1) + sf(i+2,j+1) == 0) |...
               (sf(i+1,j) ~= sf(i+1,j+2)) &(sf(i+1,j) + sf(i+1,j+2) ==  0 )  |...
               (sf(i,j+2) ~= sf(i+2,j) & (sf(i,j+2) + sf(i+2,j)==0));
                else
                    zc(i,j) =  (sf(i,j+1) ~= sf(i+2,j+1)) & (sf(i,j+1) + sf(i+2,j+1) == 0) |...
               (sf(i+1,j) ~= sf(i+1,j+2)) &(sf(i+1,j) + sf(i+1,j+2) ==  0 );
                end
            end
        end
    end
else
    z0 = f<0;       % if the element of f < 0, z0 element is "1", otherwise z0 element is "0"
    [R,C] = size(f);
    z = zeros(R,C);           %generate "0" matrix
    % Are 4 neighbors different sign to center
    z(1:R-1,:) = z(1:R-1,:) | z0(2:R,:);  % Grow
    z(2:R,:) = z(2:R,:) | z0(1:R-1,:);
    z(:,1:C-1) =  z(:,1:C-1) | z0(:,2:C);
    z(:,2:C) = z(:,2:C) | z0(:,1:C-1);

    z1 = ~z0;         % z1 = f >=0
    z = z & z1;                  % "Positive zero-crossings"?
    zc = (abs(f) >= threshold) & z;
 end

end
