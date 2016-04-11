%return angle from 0-2*pi by dx, dy
function [theta, deg]=circle_2pi(dx,dy)
  %for i=0: 360,
  %    dy=sin(i*pi/180);
  %    dx=cos(i*pi/180);
      r = sqrt(dx^2+dy^2);
      theta = asin(dy/r);
      if dx <=0 && dy > 0,
        theta = -theta + pi;
      elseif dx <=0 && dy <= 0,
        theta = abs(theta) + pi;
      elseif dx >0 && dy < 0,
        theta = theta + 2*pi;
      end
      deg = theta * 180.0 / pi;
      %disp([deg theta]);
  %end
end