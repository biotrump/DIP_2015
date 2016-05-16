%
% Dilate a binary image with structuring element
% @param inimg The input image
% @param se The structuring element (s.e.)
% @param origRow The row coordinate of the origin of the s.e., default is 0
% @param origCol The column coordinate of the origin of the s.e., default is 0
% @return The dilated image
%
function output=bmorph(method, inimg, se, origRow, origCol)
	if(strcmp(method,'dilate'))
		output=bdilate(inimg, se, origRow, origCol);
	else
		output=berode(inimg, se, origRow, origCol);
	end
end

function temp=bdilate(inimg, se, origRow, origCol)
   [nrse,ncse] = size(se);
   [nr,nc] = size(inimg);

   if (origRow >= nrse || origCol >= ncse || origRow < 0 || origCol < 0)
    fprintf('bdilate: The origin of se needs to be inside se\n');
    return;
  end
  %L: the max value of n-bit
  if ( ~uint8(se(origRow,origCol)) )     % Modified on 02/15/06
    se(origRow,origCol) = 255;          % allow the origin of se to be zero
  end

  temp=zeros(nr, nc, 'uint8');

  for i=1:nr
    for j=1:nc
      if ( uint8(inimg(i,j)) )
        % if the foreground pixel is not zero, then fill in the pixel
        % covered by the s.e.
        for m=1:nrse
          for n=1:ncse
            if ( (i-origRow+m) >= 1 && (j-origCol+n) >=1 &&...
                (i-origRow+m) <= nr && (j-origCol+n) <= nc)
              if ( ~uint8(temp(i-origRow+m, j-origCol+n)) )
                  if(se(m,n))
                	temp(i-origRow+m, j-origCol+n) = 255 ;
                  else
                      temp(i-origRow+m, j-origCol+n) =  0 ;
                  end
              end
            end
          end
        end
      end
    end
  end

end

%
% Erode a binary image with structuring element
% @param inimg The input image
% @param se The structuring element (s.e.)
% @param origRow The row coordinate of the origin of the s.e., default is 0
% @param origCol The column coordinate of the origin of the s.e., default is 0
% @return The eroded image
%
function temp=berode(inimg, se, origRow, origCol)
  [nrse,ncse] = size(se);
  [nr,nc] = size(inimg);

  if (origRow >= nrse || origCol >= ncse || origRow <= 0 || origCol <= 0)
    fprintf('erode: The origin of se needs to be inside se\n');
    return;
  end
  %L: the max value of n-bit
  if (~uint8( se(origRow,origCol) ) )     % Modified on 02/15/06
    se(origRow,origCol) = 255;
  end

  temp=zeros(nr, nc,'uint8');

    % count the number of foreground pixels in the s.e.
	sum = 0;
	for i=1:nrse
		for j=1:ncse
      		if ( uint8(se(i,j)) )
        		sum=sum+1;
			end
		end
	end

  for i=1:nr
    for j=1:nc
      if ( uint8(inimg(i,j)) )     % if the foreground pixel is 1
        count = uint8(0);
        for m=1:nrse
          for n=1:ncse
            if ((i-origRow+m) >= 1 && (j-origCol+n) >=1 &&...
                (i-origRow+m) <= nr && (j-origCol+n) <= nc)
              count = count + inimg(i-origRow+m,j-origCol+n) && se(m,n);
            end
          end
        end
        % if all se foreground pixels are included in the foreground of
        % the current pixel's neighborhood, then enable this pixel in erosion
        if (sum == count),
        	temp(i,j) = 255;
        end
      end
    end
  end

end