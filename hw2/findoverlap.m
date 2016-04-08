%0. Given 2 images Ia,Ib
% @param Ia, Ib images to stitch together and size(Ia) == size(Ib)
% @param startc, startr : the start position of stitching to save time
function sI = findoverlap(Ia, Ib, startc, startr, endc, endr)
	%default start origin
	[row, col]=size(Ia);
	if (nargin < 3)
		startc = 1;
		startr = 1;
		endc = col*2;
		endr = row*2;
	end

	%1. Ia is centered at a large canvas, Cia, with background 0
    Cia=padarray(Ia,[row col]);% Cia is row*3, col*3 with background 0 and Ia is in the center.
		r1=[col+1, row+1, col+col, row+row];	%x1,y1,x2,y2
	%	ratio of matched area in the intersection
    max_r=-1.0;
	%2. The other image Ib acts as a moving window over the canvas.
	for i=startr : 1 : endr, %step 2
		for j=startc : 1 : endc, %step 2
			%coordination of moving window r2
			r2=[j, i, j+col-1 ,i+row-1];	%x3,y3,x4,y4
			%Find the intersection rectangle rc to global coordination
			%BBOa and BBOb are the internal coordination inside the r1 and r2 respectively
			%rc,BBOa,BBOb as [x1 y1 x2 y2] for a rectangle
			[rc, BBOa, BBOb]=rect_int(r1, r2);	%find the intersection between r1 and r2

			%crop the intersection region
			Oa = Ia(BBOa(2):BBOa(4),BBOa(1):BBOa(3));
			Ob = Ib(BBOb(2):BBOb(4),BBOb(1):BBOb(3));
			%check if the Overlapped region has the better fit
			[or,oc] =size(Oa);	% or = BBOa(4)-BBOa(2)+1, oc=BBOa(3)-BBOa(1)+1

			%If O is larger than 10x10, the O may be a matched region
			if or * oc >= 100,	%lower bound of O is 10x10 pixels
				%Diff the Oa to Ob to get a D. The difference between the overlapped regions in A and B.
				D = abs(Oa-Ob);
				%A threshold T is given in D, such that Eij = 0 if Dij<T, otherwise Eij=Dij.
				T = 2;	% if pixel matches each other in the region, it's supposed to identical, so diff 3 is big enough
				D(D > T) = 0;
				D(D ~= 0) = 1;	%1 indicates pixels below thrshold
				%Calculate the ratio r : ( 1 elements in Dij)/(total elements of Dij)
				r = double(sum(sum(D)))/(or*oc);	%(numbers of 1 in O)/(area of O)
				%If r is the smallest, save the r, the overlapped regions coordination of Oa and Ob
				if r > max_r,
					max_r = r;
					max_BBa=BBOa;%the overlapped coordination of Ia
					max_BBb=BBOb;
                    max_org=[i j];%origin of the moving window
				end
			end
		%Goto 2
		end
	end
	%If the minimum r does exist, stitching A and B by overlapped region O.
	if max_r > 0.1,
		%disp the bounding box and draw the rectangle
		Cia(max_org(1):max_org(1)+row-1,max_org(2):max_org(2)+col-1)=Ib;
        sI=Cia;
	end
end
