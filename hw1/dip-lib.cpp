/** @brief All dip implementations are collected in this DIP library
 * @author : <Thomas Tsai, d04922009@ntu.edu.tw>
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>             /* getopt_long() */
#include <errno.h>
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>
#include <algorithm>    // std::sort
#include <vector>       // std::vector

using namespace cv;
using namespace std;

#include "dip.h"

int SCR_X_OFFSET=0, SCR_Y_OFFSET=0;

/** @brief flipping the image
 * org : input
 * flipped : output
 * fv : 'v' for vertically, otherwise for horizontally
 */
int flip(uint8_t *org, uint8_t *flipped, char fv)
{
	if(fv == 'v'){//vertically
		for(int i = 0 ; i < HEIGHT ; i ++)
			for(int j = 0 ; j < WIDTH ; j ++)
				flipped[(HEIGHT -1 - i) * WIDTH + j]=org[ i * WIDTH + j];
	}else{//swap 
		for(int i = 0 ; i < HEIGHT ; i ++)
			for(int j = 0 ; j < WIDTH ; j ++)
				flipped[i * WIDTH + j]=org[ i * WIDTH + (WIDTH - 1 - j)];
	}
	return 0;
}

/** @brief expanding src image to a new image by padding some rows and columns
 * around the border.
 *
 */
uint8_t *expand_by_pad(uint8_t *src, int width, int height, int pad)
{
	assert(src);

	int pw=width + 2*pad;
	int ph=height + 2*pad;
	uint8_t *pad_buf=(uint8_t *)malloc( pw * ph);
	memset(pad_buf, 0, pw * ph);

	//padding row border
	for(int y = 1; y <= pad ; y++)
		for(int x = 0; x < width;x++){
			//padding upper row border
			pad_buf[x+pad + (pad-y) * pw]=src[x + y * width];
			//padding lower row border
			pad_buf[x+pad + (height-1+y+pad) * pw]=src[x + (height -1 - y) * width];
		}
	//padding column borders
	for(int y = 0; y < height ; y++)
		for(int x = 1; x <= pad;x++){
			//padding left column border
			pad_buf[pad-x + (pad+y)*pw]=src[x + y * width];
			//padding right column border
			pad_buf[width - 1 + x + pad + (pad + y ) * pw]=src[width -1 - x + y * width];
		}

	//copy source buffer to working buffer
	for(int y = 0 ; y < height; y++)
		for(int x= 0 ; x < width ;x++)
			pad_buf[ x+pad +(y+pad)*pw]=src[x+y*width];

	//show the padded image
	IplImage* imgMedia = cvCreateImageHeader(cvSize(pw, ph), IPL_DEPTH_8U, 1);
	cvSetData(imgMedia, pad_buf, pw);
	//show the median filtered image
	namedWindow("expanding border", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("expanding border", 0,300+SCR_Y_OFFSET);
	cvShowImage( "expanding border", imgMedia );                   // Show our image inside it.

	return pad_buf;
}

void img_diff(uint8_t *s1, uint8_t *s2, uint8_t *diff, int width, int height)
{
	for(int y = 0 ; y < height; y++)
		for(int x = 0 ; x < width;x++){
			int d = s1[x+y*width] - s2[x+y*width];
			if(d*d > 100)
				diff[x+y*width]= 255;
			else 
				diff[x+y*width]= 0;
		}
}

/** @brif media filter
 * dim : dim x dim mask kernel
 *
 */
void median(uint8_t *src, uint8_t *dst, int width, int height, int dim)
{
	//padding border
	uint8_t window[dim*dim];
	int pad = (dim / 2);
	uint8_t *pad_buf=NULL;
	//expand source image by padding borders
	pad_buf = expand_by_pad(src, width, height, pad);

	for(int y = pad;  y < (height+pad); y++)
       for(int x = pad ; x < (width+pad); x++){
           int i = 0;
           for(int fx = 0; fx < dim;fx++)
               for(int fy = 0; fy < dim; fy++){
                   window[i++] = pad_buf[(x + fx - pad) + (y + fy - pad)*(width + 2*pad)];
//				   printf("[%d]=%d ", i-1, window[i-1]);
			   }
			//sorting window
			std::vector<uint8_t> myvector (window, window+dim*dim);
			// using default comparison (operator <):
			std::sort (myvector.begin(), myvector.end());
//			for(int i=0;i<dim*dim;i++)
//				printf("[%d]=%d ", i, myvector[i]);
//			printf("\nm=%d\n", myvector[ (dim * dim) / 2]);
//			cout << endl;
			//using the median element in the sorting list
			dst[(y-pad)*width+(x-pad)] = myvector[ (dim * dim) / 2];
	   }

	 free(pad_buf);
}

/** @brif media filter
 * dim : dim x dim mask kernel
 *
 */
void mean(uint8_t *src, uint8_t *dst, int width, int height, int dim)
{
	//padding border
	uint8_t window[dim*dim];
	int pad = (dim / 2);
	uint8_t *pad_buf=NULL;
	//expand source image by padding borders
	pad_buf = expand_by_pad(src, width, height, pad);

	for(int fx = 0; fx < dim;fx++)
		for(int fy = 0; fy < dim; fy++){
			window[fx + fy * dim] = 1;
        }

	for(int y = pad;  y < (height+pad); y++)
       for(int x = pad ; x < (width+pad); x++){
		   unsigned sum=0;
           for(int fx = 0; fx < dim;fx++)
               for(int fy = 0; fy < dim; fy++){
                   sum += window[fx + fy*dim] * pad_buf[(x + fx - pad) + (y + fy - pad)*(width + 2*pad)];
			   }
			sum /= dim*dim;
			//using the median element in the sorting list
			dst[(y-pad)*width+(x-pad)] = sum;
	   }

	 free(pad_buf);
}

/** @brief flipping the image
 * image : 8bit grey image
 * width : width of the image
 * height : height of the image
 */
int hist(unsigned *hist_table, int h_size, uint8_t *image, int width, int height)
{
	memset((void *)hist_table, 0, MAX_GREY_LEVEL * sizeof(unsigned));
	for(int i = 0 ; i < height*width ; i ++){
		//printf("0x%x ", image[i]);
		hist_table[image[i]]++;
	}
#ifdef DEBUG
	for(int i = 0 ; i < MAX_GREY_LEVEL;i++){
		printf("%3d: %u\n", i, hist_table[i]);
	}
#endif
}

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
			 unsigned *cdf_table, int h_size, uint8_t *histeq_map, string &name)
{
	unsigned cdf=0;
	//calculate CDF without normalization from 0 to 1.0, but in 0 to pixels
	for(int i=0;i < h_size ; i++ ){
		cdf += hist_table[i];
		cdf_table[i] = cdf;
		histeq_map[i] = (255 * cdf)  / pixels ;//mapping grey level i to new grey level
		//printf("%d->%d\n", i, histeq_map[i]);
	}

	//histogram equlization
	for(int i = 0; i < pixels ; i++)
		dst[i] = histeq_map[src[i]];	//mapping original grey level to new level
}

//http://stackoverflow.com/questions/3071665/getting-a-directory-name-from-a-filename
void SplitFilename (const string& str, string &folder, string &file)
{
	size_t found;
	cout << "Splitting: " << str << endl;
	found=str.find_last_of("/\\");
	folder = str.substr(0,found);
	file = str.substr(found+1);
	cout << " folder: " << str.substr(0,found) << endl;
	cout << " file: " << str.substr(found+1) << endl;
}

/** @brief Draw the histograms
 * input 
 * hist_table[] : histogram table 
 * h_size : level of histogram table, ie, 256 grey levels
 */
void draw_hist(unsigned *hist_table, int h_size, const string &t_name, int wx, int wy)
{
	float ht[MAX_GREY_LEVEL];
	for(int i = 0; i < h_size; i ++)
		ht[i] = hist_table[i];
	//create a openCV matrix from an array : 1xh_size, floating point array
	Mat b_hist=Mat(1, h_size, CV_32FC1, ht);
	//calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );

	int hist_w = HIST_WIN_WIDTH, hist_h = HIST_WIN_HEIGHT;
	int bin_w = cvRound( (double) hist_w/h_size );

	Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
	
	// Normalize the result to [ 0, histImage.rows ]
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
	for( int i = 1; i < h_size; i++ )
		line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                     Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                     Scalar( 255, 0, 0), 2, 8, 0  );
	string win_name("Histogram: ");
	win_name = win_name + t_name;

	namedWindow(win_name, CV_WINDOW_AUTOSIZE );
	moveWindow(win_name, wx,wy);
	imshow(win_name, histImage );
}