/** @brief All dip implementations are collected in this DIP library
 * @author : <Thomas Tsai, d04922009@ntu.edu.tw>
 *
 */
#ifndef _H_DIP_LIB_H
#define	_H_DIP_LIB_H

#define	MAX_GREY_LEVEL	(1<<8)

#define	WIDTH	(256)
#define	HEIGHT	(256)

#define	HIST_WIN_WIDTH 	(256)
#define	HIST_WIN_HEIGHT	(256)

//http://stackoverflow.com/questions/3071665/getting-a-directory-name-from-a-filename
//split path string to substring folder and file
void SplitFilename (const string& str, string &folder, string &file);

/** @brief expanding src image to a new image by padding some rows and columns
 * around the border, so the matrix kernel can operate on the border of the Original
 * image.
 */
uint8_t *expand_border(uint8_t *src, int width, int height, int pad);

/** @brif media filter
 * dim : dim x dim mask kernel
 *
 */
void median(uint8_t *src, uint8_t *dst, int width, int height, int dim=3);

/** @brif media filter
 * dim : dim x dim mask kernel
 *
 */
void mean(uint8_t *src, uint8_t *dst, int width, int height, int dim=3);

/** @brief flipping the image
 * image : 8bit grey image
 * width : width of the image
 * height : height of the image
 */
int hist(unsigned *hist_table, int h_size, uint8_t *image, int width, int height);


/** histogram equlization
 * mapping original hist_table to new table histeq_map
 * input :
 * src[] : 8 bit grey image
 * dst[] : histogram equlized destination buffer
 * pixels : total pixels of the 8 bit grey image
 * hist_table[] : histogram
 * cdf_table[] : cdf of the hist_table
 * h_size : size of histogram, it's usually 256 for 8 bit grey image
 * histeq_map : the histogram equalization mapping table
 * name : name of window to show
 */
void hist_eq(uint8_t *src, uint8_t *dst, int pixels, unsigned *hist_table,
			 unsigned *cdf_table, int h_size, uint8_t *histeq_map, string &name);
#endif	//_H_DIP_LIB_H