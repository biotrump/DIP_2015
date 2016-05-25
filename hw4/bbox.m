% @return bl = y0 y1 x0 x1; list to the bounding box
function bl=bbox(imgsrc)
	[height, width]=size(imgsrc);
	minY=0;
	maxY=0;
	l=1;
    minFound=0;
    maxFound=0;
    bl=[];
	for i=1:height
		data=imgsrc(i,:);%get row i
		val= sum(data);
		if (~minFound && val ~=0),
			minY=i;
            minFound=1;
        end
        if (minFound && val == 0 ) ,
			maxY= i-1;
            maxFound=1;
        elseif (minFound && i == height ),
            maxY= i;
            maxFound=1;
        end

		if (minFound && maxFound),
            bX=findX(imgsrc(minY:maxY,:), width, minY, maxY);
            for j=1:size(bX,1)
                bl(l,:)=[minY maxY bX(j,:)];
                l=l+1;
            end
            minFound=0;
            maxFound=0;
		end
    end
    %bl
end
function bX=findX(strip, width, minY, maxY)
	minX=1;
	maxX=1;
	l=1;
    minFound=0;
    maxFound=0;
	for i=1:width
		data=strip(:,i);%get row i
		val= sum(data);
		if (~minFound && val ~=0),
			minX=i;
            minFound=1;
        end
        if (minFound && val == 0),
			maxX= i-1;
            maxFound=1;
        elseif (minFound && i == width ),
            maxX= i;
            maxFound=1;
        end

		if (minFound && maxFound),
			bX(l,:)=[minX maxX];
			l=l+1;
            minFound=0;
            maxFound=0;
		end
    end
    %bX
end