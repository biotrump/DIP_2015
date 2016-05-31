% @brief  perform image erosion 
function retval = imerode(im, se)
  ## Checkinput
  if (nargin != 2)
    print_usage();
  endif
  if (!ismatrix(im) || !isreal(im))
    error("imerode: first input argument must be a real matrix");
  endif
  if (!ismatrix(se) ||  !isreal(se))
    error("imerode: second input argument must be a real matrix");
  endif

  ## Perform filtering
  retval = ordfiltn(im, 1, se, 0);
endfunction
