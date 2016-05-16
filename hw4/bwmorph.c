/**
 * Dilate a binary image with structuring element
 * @param inimg The input image
 * @param se The structuring element (s.e.)
 * @param origRow The row coordinate of the origin of the s.e., default is 0
 * @param origCol The column coordinate of the origin of the s.e., default is 0
 * @return The dilated image
 */
Image bdilate(Image &inimg, Image &se, int origRow, int origCol) {
  Image temp;
  int nr, nc, nrse, ncse;
  int i, j, m, n;

  nrse = se.getRow();
  ncse = se.getCol();

  nr = inimg.getRow();
  nc = inimg.getCol();

  if (origRow >= nrse || origCol >= ncse || origRow < 0 || origCol < 0) {
    cout << "bdilate: The origin of se needs to be inside se\n";
    exit(3);
  }
  //L: the max value of n-bit
  if (!(int)se(origRow,origCol)) {    // Modified on 02/15/06
    se(origRow,origCol) = L;          // allow the origin of se to be zero
  }

  temp.createImage(nr, nc, nt);

  for (i=0; i<nr; i++)
    for (j=0; j<nc; j++) {
      if ((int)inimg(i,j)) {
        // if the foreground pixel is not zero, then fill in the pixel
        // covered by the s.e.
        for (m=0; m<nrse; m++)
          for (n=0; n<ncse; n++) {
            if ((i-origRow+m) >= 0 && (j-origCol+n) >=0 &&
                (i-origRow+m) < nr && (j-origCol+n) < nc)
              if (!(int)temp(i-origRow+m, j-origCol+n))
                temp(i-origRow+m, j-origCol+n) = ( (int)se(m,n) ? L : 0 );
          }
      }
    }

  return temp;
}

/**
 * Erode a binary image with structuring element
 * @param inimg The input image
 * @param se The structuring element (s.e.)
 * @param origRow The row coordinate of the origin of the s.e., default is 0
 * @param origCol The column coordinate of the origin of the s.e., default is 0
 * @return The eroded image
 */
Image berode(Image &inimg, Image &se, int origRow, int origCol) {
  Image temp;
  int nr, nc, nchan, nt, nrse, ncse, nchanse, ntse;
  int i, j, m, n, sum, count;

  nrse = se.getRow();
  ncse = se.getCol();

  nr = inimg.getRow();
  nc = inimg.getCol();

  if (origRow >= nrse || origCol >= ncse || origRow < 0 || origCol < 0) {
    cout << "erode: The origin of se needs to be inside se\n";
    exit(3);
  }
  //L: the max value of n-bit
  if (!(int)se(origRow,origCol)) {    // Modified on 02/15/06
    se(origRow,origCol) = L;
  }

  temp.createImage(nr, nc);

  // count the number of foreground pixels in the s.e.
  sum = 0;
  for (i=0; i<nrse; i++)
    for (j=0; j<ncse; j++)
      if ((int)se(i,j))
        sum++;

  for (i=0; i<nr; i++)
    for (j=0; j<nc; j++) {
      if ((int)inimg(i,j)) {     // if the foreground pixel is 1
        count = 0;
        for (m=0; m<nrse; m++)
          for (n=0; n<ncse; n++) {
            if ((i-origRow+m) >= 0 && (j-origCol+n) >=0 &&
                (i-origRow+m) < nr && (j-origCol+n) < nc)
              count += (int)inimg(i-origRow+m,j-origCol+n) && (int)se(m,n);
          }
        // if all se foreground pixels are included in the foreground of
        // the current pixel's neighborhood, then enable this pixel in erosion
        if (sum == count)
          temp(i,j) = L;
      }
    }

  return temp;
}
