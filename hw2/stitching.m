%0. Given 2 images Ia,Ib
% @param Ia image is always at the center of the canvas.
% @param Ib images to stitch to Ia, this acts as a moving window over the
% canvas
% @param startc, startr : the start position of stitching to save time
% @param endc, endr end of moving box to find stitching
% @return sI the stitching image in the canvas
% @return corner_a, corner_b : the left top corner of the image Ia and Ib to stitching
% corner_a, corner_b : (row, col)
function [sI, corner_a, corner_b, rect_oa, rect_ob] = stitching(Ia, Ib, startc, startr, endc, endr)
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
    corner_a=[row+1, col+1];    %left top corner of Ia
	%	ratio of matched area in the intersection
    max_r=-1.0;
	min_mse=100000;
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
				if 1,
				 	mse=immse(Oa,Ob);
					if mse < min_mse,
						min_mse = mse;
						rect_oa=BBOa;%the overlapped coordination of Ia
						rect_ob=BBOb;
						%origin of the moving window
						corner_b=[i,j]; %left top corner of Ib in the canvas
					end
				else
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
						rect_oa=BBOa;%the overlapped coordination of Ia
						rect_ob=BBOb;
	          corner_b=[i j];%origin of the moving window
					end
				end
			end
		%Goto 2
		end
	end
	%If the minimum r does exist, stitching A and B by overlapped region O.
	%if max_r > 0.001,
    if min_mse < 100000,
			%disp the bounding box and draw the rectangle
			Cia(corner_b(1):corner_b(1)+row-1,corner_b(2):corner_b(2)+col-1)=Ib;
			sI=Cia;
		end
end
