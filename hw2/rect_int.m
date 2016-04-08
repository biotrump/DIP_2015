% @brief given r1,r2 rectangles and find the rectangle to the global
% coordination and also the local coordinations to r1 and r2
% @param r1 = [x1 y1 x2 y2]
% @param r2 = [x3 y3 x4 y4]
% and x1 < x2, y1 < y2, x3 < x4, y3 < y4
% @return rc[x5 y5 x6 y6] the intersection rectangle to the glogal coordination
% @return o1  the intersection rectangle to the r1 coordination
% @return o2  the intersection rectangle to the r2 coordination
function [rc,o1,o2]=rect_int(r1, r2)
  rc=[];
  x1=r1(1);
  y1=r1(2);
  x2=r1(3);
  y2=r1(4);

  x3=r2(1);
  y3=r2(2);
  x4=r2(3);
  y4=r2(4);

  x5 = max(x1,x3);
  y5 = max(y1,y3);
  x6 = min(x2, x4);
  y6 = min(y2,y4);
  %intersec rectangle by r1 coordination
  if x5 > x6 || y5 > y6, %no intersection
    rc=[0 0 0 0];
    o1=[];
    o2=[];
  else
    rc(1)=x5;
    rc(2)=y5;
    rc(3)=x6;
    rc(4)=y6;
    %the intersection rectangle coordination inside the rectangle1
    % equivalent to translate rectangle1 to origin and then find the intersection
    o1=[];
    o1(1)=x5-x1+1;
    o1(2)=y5-y1+1;
    o1(3)=x6-x1+1;
    o1(4)=y6-y1+1;
    o2=[];
    o2(1)=x5-x3+1;
    o2(2)=y5-y3+1;
    o2(3)=x6-x3+1;
    o2(4)=y6-y3+1;
  end

end
