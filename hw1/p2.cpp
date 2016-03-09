/** @brief DIP program to flip an image
 * @author <Thomas Tsai, thomas@life100.cc>
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

#define	WIDTH	(256)
#define	HEIGHT	(256)

#define	HIST_WIN_WIDTH 	(256)
#define	HIST_WIN_HEIGHT	(256)

#define	MAX_GREY_LEVEL	(1<<8)

using namespace cv;
using namespace std;

const char org_display[]="Original Display";
const char flip_v[]="vertically flipped";
const char flip_h[]="horizontally flipped";

char raw_fileD[1024]="sample2.raw";
char raw_file[1024]="sample3.raw";
char problem[100]="2a";
int mask_dim=3;

static void usage(FILE *fp, int argc, char **argv)
{
	fprintf(fp,
		 "Usage: %s [options]\n\n"
		 "Options:\n"
		 "-h | --help       Print this message\n"
		 "-n | --dim	n	nxn mask\n"
		 "-p | --problem 2a The problem 2a,2b,2c... to solve\n"
		 "-r | --raw	    The full path of the raw file \n"
		 "",
		 argv[0]);
}

static const char short_options[] = "hn:p:r:";

static const struct option
long_options[] = {
	{ "help",   	no_argument,       NULL, 'h' },
	{ "dim",		required_argument, NULL, 'n' },
	{ "problem",	required_argument, NULL, 'p' },
	{ "raw",		required_argument, NULL, 'r' },
	{ 0, 0, 0, 0 }
};

static int option(int argc, char **argv)
{
	int r=0;
	//printf("+%s:argc=%d\n",__func__, argc);

	for (;;) {
		int idx;
		int c;

		c = getopt_long(argc, argv,
				short_options, long_options, &idx);
		if (-1 == c)//no args now
			break;

		switch (c) {
		case 0: /* getopt_long() flag */
			break;
		case 'n':
			errno = 0;
			mask_dim = atoi(optarg);
			printf("mask_dim=%d\n", mask_dim);
			if (errno){
				r=-1;
			}
			break;
		case 'p':
			errno = 0;
			strncpy(problem, optarg, 100);
			printf("%s\n", problem);
			if (errno){
				r=-1;
			}
			break;
		case 'r':
			errno = 0;
			strncpy(raw_file, optarg, 1024);
			printf("%s\n", raw_file);
			if (errno){
				r=-1;
			}
			break;
		case 'h':
			usage(stdout, argc, argv);
			r=1;
			break;

		default:
			usage(stderr, argc, argv);
			r=-1;
		}
	}
	//printf("-%s:%d\n",__func__, r);
	return r;
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
	namedWindow( "pad", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("pad", 0,300);
	cvShowImage( "pad", imgMedia );                   // Show our image inside it.

	return pad_buf;
}

/** @brif media filter
 * dim : dim x dim mask kernel
 *
 */
void median(uint8_t *src, uint8_t *dst, int width, int height, int dim=3)
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
void mean(uint8_t *src, uint8_t *dst, int width, int height, int dim=3)
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

void p2a(uint8_t *src, uint8_t *dst, int width, int height, int dim=3)
{
	//median(src, dst, width, height, dim);
	mean(src, dst, width, height, dim);
}

/**
 *
 */
int main( int argc, char** argv )
{
    int fd=-1;

	if(argc==1){
		usage(stderr, argc, argv);
		exit(EXIT_FAILURE);
	}

	int r = option(argc,argv);
	if(r<0){
		cout << "Wrong args!!!\n";
		exit(EXIT_FAILURE);
	}else if(r > 0){
		cout << "Done!!!\n";
		exit(EXIT_FAILURE);
	}

	//opening file
	printf("%s\nD:%s\n", raw_file, raw_fileD);
	uint8_t *bufRaw=NULL;
	if( (fd = open(raw_file, O_RDONLY)) != -1 ){
		bufRaw= (uint8_t *)malloc( WIDTH * HEIGHT);
		ssize_t s=read(fd, bufRaw, WIDTH * HEIGHT);
		close(fd);
		cout << "request size = " << WIDTH * HEIGHT << ", read size=" << s << endl;
	}else{
		cout << raw_file << " opening failure!:"<< errno << endl;
		exit(EXIT_FAILURE);
	}

	//load raw file
	string folder, winRaw;
	SplitFilename (raw_file, folder, winRaw);
	IplImage* imgRaw = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	cvSetData(imgRaw, bufRaw, WIDTH);
	//show the raw image
	namedWindow( winRaw, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow(winRaw, 0,0);
	cvShowImage( winRaw.c_str(), imgRaw );                   // Show our image inside it.

	uint8_t *bufMedian= (uint8_t *)malloc( WIDTH * HEIGHT);
	p2a(bufRaw, bufMedian, WIDTH, HEIGHT, mask_dim);

	string winMedian;
	IplImage* imgMedia = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	cvSetData(imgMedia, bufMedian, WIDTH);
	//show the median filtered image
	winMedian = winRaw+" median";
	namedWindow( winMedian, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow(winMedian, 300,0);
	cvShowImage( winMedian.c_str(), imgMedia );                   // Show our image inside it.

/*

	//create output file H
	if( (fd = open("H.raw", O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR) ) != -1 ){
		int s=write(fd, bufH, WIDTH * HEIGHT);
		cout << "write:" << s << endl;
		close(fd);
	}else{
		cout << "H.raw open failed!"  << endl;
	}
*/

	cout << "press any key to quit..." << endl;
	waitKey(0);                                          // Wait for a keystroke in the window

	destroyWindow(winRaw);
	cvReleaseImageHeader(&imgRaw);
	free(bufRaw);

	destroyWindow(winMedian);
	cvReleaseImageHeader(&imgMedia);
	free(bufMedian);
    return 0;
}
