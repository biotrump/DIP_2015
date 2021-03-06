%setup the bounding box for the test image
%manually select the coarse boundinb box and the fine tune
function gtruth_tbl=test_set(image)
%x0,y0,x1,y1
bbox=[8,8, 45,46 'x';...
64,23,94,54, 'x';...
102,10,122,35, 'x';...
130,   44,  167, 71, 'V';...
179,8,210,39, 'x';...
215,   36,  244, 74, '9';...
24,58,44,80, 'x';...
58,65,86,97, 'x';...
107,84,134,117, 'x';...
160,82,190,116 , 'x';...
210,86,248,124, 'x';...
11,100,62,153, 'x';...
70,126,102,156, 'x';...
111,144,141,179, 'x';...
155,132,187,170, 'x';...
193,131,228,178, 'x';...
50,160,84,200, 'x';...
13,199,45,228, 'x';...
61,211,93,244, 'x';...
106,195,168,253, 'x';...
170,192,206,230, 'x';...
210,198,235,230, 'x'];

gtruth_tbl=[    8    8   45   46   89;...
   64   23   94   54   80;...
  102   10  122   35   57;...
  130   44  167   71   86;...
  179    8  210   39   52;...
  215   36  244   74   57;...
   24   58   44   80   51;...
   58   65   86   97   55;...
  107   84  134  117   51;...
  160   82  190  116   84;...
  210   86  248  124   72;...
   11  100   62  153   67;...
   70  126  102  156   84;...
  111  144  141  179   67;...
  155  132  187  170   66;...
  193  131  228  178   51;...
   50  160   84  200   80;...
   13  199   45  228   72;...
   61  211   93  244   66;...
  106  195  168  253   84;...
  170  192  206  230   86;...
  210  198  235  230   53];
  
    bbox=uint8(bbox); 
    fig_bc=figure('name','bchar');
    for i=1:size(bbox,1)
        x0=bbox(i,1);y0=bbox(i,2);
        x1=bbox(i,3);y1=bbox(i,4);
        roi=image(y0:y1,x0:x1);
        image(y0:y1,x0:x1)=0;
        figure(fig_bc);
        imshow(roi);
        %gt = input('ground truth:','s');
        %bbox(i,5)=gt;
    end
    %figure;
    %imshow(image);
end