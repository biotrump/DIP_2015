r=5.0;
for i=0:360,
  dy = r * sin(i * pi/180.0);
  dx = r * cos(i * pi/180.0);
  [theta, deg]=circle_2pi(dx,dy);
end
return;
