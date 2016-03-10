/** @brief All dip implementations are collected in this DIP library
 * @author : <Thomas Tsai, d04922009@ntu.edu.tw>
 *
 */
#ifndef _H_DIP_LIB_H
#define	_H_DIP_LIB_H
#include <iostream>
#include <string>
#include <algorithm>    // std::sort
#include <vector>       // std::vector

#define	DEFAULT_BIT_DEPTH	(8)
#define	MAX_GREY_LEVEL	((1<<DEFAULT_BIT_DEPTH) -1)

#define	WIDTH	(256)
#define	HEIGHT	(256)

#define	HIST_WIN_WIDTH 	(256)
#define	HIST_WIN_HEIGHT	(256)

//MAX kernel matrix dimension
#define	MAX_DIM		(33)

/** @brief flipping the image
 * org : input
 * flipped : output
 * fv : 'v' for vertically, otherwise for horizontally
 */
int flip(uint8_t *org, uint8_t *flipped, char fv);

//http://stackoverflow.com/questions/3071665/getting-a-directory-name-from-a-filename
//split path string to substring folder and file
void SplitFilename (const string& str, string &folder, string &file);

/* M x N matrix
 * impulse noise : pepper and salt noise generator
 * black_thr : black threshold <
 * white_thr : white threshold >
 */
void impulse_noise_gen(uint8_t *imp, int M, int N, uint8_t black_thr, uint8_t white_thr);
void impulse_noise_add(uint8_t *imp, uint8_t *image, int M, int N);

/* M x N matrix
 * white noise : uniform white noise generator
 * white_thr : white threshold >
 */
void white_noise_gen(uint8_t *imp, int M, int N, uint8_t white_thr);
void white_noise_add(uint8_t *white, uint8_t *image, int M, int N );
/** @brief expanding src image to a new image by padding some rows and columns
 * around the border, so the matrix kernel can operate on the border of the Original
 * image.
 */
uint8_t *expand_border(uint8_t *src, int width, int height, int pad);

void img_diff(uint8_t *s1, uint8_t *s2, uint8_t *diff, int width, int height);

/** @brief power 2 of the diff two images : (s1[]-s2[]) * (s1[]-s2[]) = diff[]
 * I : orignal image
 * P : processed image
 */
double img_MSE(uint8_t *I, uint8_t *P, int width, int height);

/** Peak Signal to Noise Ratio
 * L : bits of the pixel, default is 8 bits
 */
float PSNR(uint8_t *I, uint8_t *P, int width, int height, int L=8);

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

/** @brief Draw the histograms
 * input 
 * hist_table[] : histogram table 
 * h_size : level of histogram table, ie, 256 grey levels
 */
void draw_hist(unsigned *hist_table, int h_size, const string &t_name, int wx=300, int wy=300);
/* opencv
 * 
 */
void cvPrintf(IplImage* img, const char *text, CvPoint TextPosition, CvFont Font1,
			  CvScalar Color);

extern int SCR_X_OFFSET, SCR_Y_OFFSET;

#endif	//_H_DIP_LIB_H