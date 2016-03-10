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

/** @brief boundary extension of the src image to a new image by padding some rows and columns
 * around the border.
 * pad : odd extension
 */
uint8_t *boundary_ext(uint8_t *src, int width, int height, int pad, const string ename)
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
/*
	//show the padded image
	IplImage* imgMedia = cvCreateImageHeader(cvSize(pw, ph), IPL_DEPTH_8U, 1);
	cvSetData(imgMedia, pad_buf, pw);
	//show the median filtered image
	namedWindow(ename, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow(ename, 1000,400+SCR_Y_OFFSET);
	cvShowImage(ename.c_str(), imgMedia );                   // Show our image inside it.
	cout << ename <<endl;
	*/
	return pad_buf;
}

/* M x N matrix
 * impulse noise : pepper and salt noise generator
 * black_thr : black threshold <
 * white_thr : white threshold >
 */
void impulse_noise_gen(uint8_t *imp, int M, int N, uint8_t black_thr, uint8_t white_thr)
{
	//generate random matrix
	int b=0,w=0;
	srand(time(NULL));
	memset(imp, 127, M*N);
	printf("%s:black_thr=%u, white_thr=%u\n", __func__, black_thr,white_thr);
	for(int i = 0; i < M*N; i++) {
		int r =  rand()%256;//0-255
		//printf("r=%d\n",r);
		if(r < black_thr){
			imp[i]=0;//black
			b++;
		}else if(r > white_thr){
			imp[i] = 255;//white
			w++;
		}
	}
	printf("%s:black=%d, white=%d\n", __func__,b,w);
}

void impulse_noise_add(uint8_t *imp, uint8_t *image, int M, int N)
{
	for(int i = 0; i < M*N; i++) {
		if(imp[i]==0){
			image[i]=0;//black
		}else if(imp[i] == 255){
			image[i] = 255;//white
		}
	}
}

/* M x N matrix
 * white noise : uniform white noise generator
 * white_thr : white threshold >
 */
void white_noise_gen(uint8_t *imp, int M, int N, uint8_t white_thr)
{
	//generate random matrix
	int w=0;
	srand(time(NULL));
	memset(imp, 0, M*N);
	printf("%s:white_thr=%u\n", __func__, white_thr);
	for(int i = 0; i < M*N; i++) {
		int r =  rand()%256;//0-255
		if(r >= white_thr){
			imp[i] = r;//white
			w++;
		}
	}
	printf("%s: white=%d\n", __func__,w);
}

void white_noise_add(uint8_t *white, uint8_t *image, int M, int N )
{
	for(int i = 0; i < M*N; i++) {
		if(white[i])
			image[i] = 255;//white
	}

}

/** @brief power 2 of the diff two images : (s1[]-s2[]) * (s1[]-s2[]) = diff[]
 * 
 */
void img_diff(uint8_t *s1, uint8_t *s2, uint8_t *diff, int width, int height)
{
	for(int y = 0 ; y < height; y++)
		for(int x = 0 ; x < width;x++){
			int d = s1[x+y*width] - s2[x+y*width];
			diff[x+y*width]= d*d;
		}
}

/** @brief power 2 of the diff two images : (s1[]-s2[]) * (s1[]-s2[]) = diff[]
 * I : orignal image
 * P : processed image
 */
double img_MSE(uint8_t *I, uint8_t *P, int width, int height)
{
	double mse=0.0;
	for(int y = 0 ; y < height; y++)
		for(int x = 0 ; x < width;x++){
			int d = I[x+y*width] - P[x+y*width];
			mse += d*d;
		}
	printf("%s:mse=%f, MSE=%f\n",__func__, mse, mse/(height * width));
	return mse/(height * width);
}

/** Peak Signal to Noise Ratio
 * L : bits of the pixel
 */
float PSNR(uint8_t *I, uint8_t *P, int width, int height, int L)
{
	float mse = img_MSE(I, P, width, height);
	unsigned level = (0x1 << L ) -1;
	
	printf("%s:L=%d, level=%d\n",__func__, L, level);
	float psnr = 10.0 * logf( (level * level) / mse );
	printf("%s:psnr=%f\n",__func__, psnr);
	
	return psnr;
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
	pad_buf = boundary_ext(src, width, height, pad, "median");

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
 * dim : dim x dim mask kernel, dim is "odd"
 *
 */
void mean(uint8_t *src, uint8_t *dst, int width, int height, int dim)
{
	//padding border
	uint8_t window[dim*dim];
	int pad = (dim / 2);
	uint8_t *pad_buf=NULL;
	//expand source image by padding borders
	pad_buf = boundary_ext(src, width, height, pad, "mean");

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

/* opencv
 * 
 */
void cvPrintf(IplImage* img, const char *text, CvPoint TextPosition, CvFont Font1,
			  CvScalar Color)
{
  //char text[] = "Funny text inside the box";
  //int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
  //double fontScale = 2;

  // center the text
  //Point textOrg((img.cols - textSize.width)/2,
	//			(img.rows + textSize.height)/2);

  //double Scale=2.0;
  //int Thickness=2;
  //CvScalar Color=CV_RGB(255,0,0);
  //CvPoint TextPosition=cvPoint(400,50);
  //CvFont Font1=cvFont(Scale,Thickness);
  // then put the text itself
  cvPutText(img, text, TextPosition, &Font1, Color);
  //cvPutText(img, text, TextPosition, &Font1, CV_RGB(0,255,0));
}