function bl=bbox(imgsrc)
	[height, width]=size(imgsrc);
	minY=-1;
	maxY=-2;
	l=1;
    minFound=0;
    maxFound=0;
	for i=1:height
		data=imgsrc(i,:);%get row i
		val= sum(data);
		if (~minFound && val ~=0),
			minY=i;
            minFound=1;
        end
        if (minFound && val == 0),
			maxY= i-1;
            maxFound=1;
        end
        
		if (minFound && maxFound),
			bl(l,:)=[minY maxY];
			l=l+1;
            minY=maxY+1;
            minFound=0;
            maxFound=0;
		end
    end
    bl
end