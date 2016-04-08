% @brrief find a bounding box to contain some object in the canvas
% @param O a canvas with some object inside
% @return BBox bouding box (y0,x0,y1,x1)
function BBox=bbox(O)
  [y,x] = ind2sub(size(O), find(O));
  coord = [y, x];
  BBox=[min(coord) max(coord)];   %% [1 2 4 4];	%bouding box (y0,x0,y1,x1)
end
