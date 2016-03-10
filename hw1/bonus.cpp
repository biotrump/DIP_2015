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
using namespace cv;
using namespace std;

#include "dip.h"

char raw_file[1024];
char problem[100]="2a";
int mask_dim=3;
const string track_bar_name("kernel dim x dim");
IplImage* imgBar=NULL;

static void usage(FILE *fp, int argc, char **argv)
{
	fprintf(fp,
		 "Usage: %s [options]\n\n"
		 "Options:\n"
		 "-h | --help        Print this message\n"
		 "-n | --dim n       nxn mask\n"
		 "-p | --problem 2x  The problem 2a,2b,2c... to solve\n"
		 "-r | --raw	     The full path of the raw file \n"
		 "-o | --offset	 mxn The screen offset for dual screen\n"
		 "",
		 argv[0]);
}

static const char short_options[] = "hn:o:p:r:";

static const struct option
long_options[] = {
	{ "help",   	no_argument,       NULL, 'h' },
	{ "dim",		required_argument, NULL, 'n' },
	{ "offset",		required_argument, NULL, 'o' },
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
		case 'o':
			if(optarg && strlen(optarg)){
				int i=0;
				printf("screen offset:%s\n",optarg);
				while(optarg[i] && optarg[i] !='x') i++;
				if(optarg[i] =='x') {
					optarg[i]=' ';//delimeter
					sscanf(optarg,"%d %d", &SCR_X_OFFSET, &SCR_Y_OFFSET);
					printf("SCR_X_OFFSET=%d, SCR_Y_OFFSET=%d\n", SCR_X_OFFSET, SCR_Y_OFFSET);
				}
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

void p2a(uint8_t *src, uint8_t *dst, int width, int height, int dim=3)
{
	median(src, dst, width, height, dim);
}

void p2b(uint8_t *src, uint8_t *dst, int width, int height, int dim=3)
{
	mean(src, dst, width, height, dim);
}

void ProcessDim(int pos, void *userdata)
{
	uint8_t *bufRaw = (uint8_t *)userdata;

	printf(">>%s:pos=%d, mask_dim=%d\n", __func__, pos, mask_dim);
	if(pos < 3 ) {
		pos=3;
	}
	if((pos%2)==0) {
		pos--;//only odd dim is allowed!
	}
	mask_dim = pos;
	printf("<<%s:pos=%d, mask_dim=%d\n", __func__, pos, mask_dim);

	char dim_str[100];
	sprintf(dim_str, "dim:%d x %d", mask_dim, mask_dim);
	cvSetZero(imgBar);
	cvPrintf(imgBar, dim_str, cvPoint(1, 30),
								cvFont(1.0, 1.0), CV_RGB(255,255,255));
	cvShowImage( track_bar_name.c_str(), imgBar);
		
	IplImage* imgP2a = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	IplImage* imgP2b = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	string winP2="P";
	uint8_t *buf_worka=NULL, *buf_workb=NULL;
	buf_worka= (uint8_t *)malloc( WIDTH * HEIGHT);
	buf_workb= (uint8_t *)malloc( WIDTH * HEIGHT);
	//if( strcmp(problem, "2a") == 0)
	{
		//Problem 2a : median filter
		p2a(bufRaw, buf_worka, WIDTH, HEIGHT, mask_dim);

		cvSetData(imgP2a, buf_worka, WIDTH);
		//show the median filtered image
		//char temp[100];
		//sprintf(temp, "%s%s median :%dx%d", winP2.c_str(), problem, mask_dim, mask_dim);
		//sprintf(temp, "%s%s median", winP2.c_str(), problem);
		//sprintf(temp, "%s median", winP2.c_str());
		winP2 = "median";
		namedWindow( winP2, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
		moveWindow(winP2,300,0 + SCR_Y_OFFSET);
		cvShowImage( winP2.c_str(), imgP2a );                   // Show our image inside it.
		
		//////////////////////////////////////////
		//bonus
		//show histogram of image
		unsigned hist_tableI[MAX_GREY_LEVEL];
		uint8_t histeq_mapI[MAX_GREY_LEVEL];
		hist(hist_tableI, MAX_GREY_LEVEL, buf_worka, WIDTH, HEIGHT);
		draw_hist(hist_tableI, MAX_GREY_LEVEL, winP2, 600,0+SCR_Y_OFFSET);

		//histogram equlization of Image
		uint8_t *buf_he= (uint8_t *)malloc( WIDTH * HEIGHT);
		unsigned cdf_table[MAX_GREY_LEVEL];
		hist_eq(buf_worka, buf_he,  WIDTH * HEIGHT, hist_tableI, cdf_table,
				MAX_GREY_LEVEL,	histeq_mapI, winP2);

		//show cdf of image
		string t_name(winP2 + " cdf ");
		draw_hist(cdf_table, MAX_GREY_LEVEL, t_name,800,0+SCR_Y_OFFSET);
		
		//show the image , the histogram equlization of Image I
		IplImage* imgHE = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
		cvSetData(imgHE, buf_he, WIDTH);
		namedWindow( winP2 + "Hist eq", CV_WINDOW_AUTOSIZE );	// Create a window for display.
		moveWindow( winP2 + "Hist eq", 1050, 0 + SCR_Y_OFFSET);
		string winP2I = winP2 + "Hist eq";
		char win_hname[100];
		strcpy(win_hname, winP2I.c_str());
		cvShowImage( win_hname, imgHE );                   // Show our image inside it.
		
		//PSNR
		char psnr_str[100];
		float psnr = PSNR(bufRaw, buf_he, WIDTH, HEIGHT);
		sprintf(psnr_str, "PSNR of median=%f", psnr);
		cvPrintf(imgBar, psnr_str, cvPoint(1, 50),
									cvFont(1.0, 1.0), CV_RGB(255,255,255));
		cvShowImage( track_bar_name.c_str(), imgBar);
	}
	//else if(strcmp(problem, "2b") == 0)
	{
		//Problem 2b : mean filter
		p2b(bufRaw, buf_workb, WIDTH, HEIGHT, mask_dim);

		cvSetData(imgP2b, buf_workb, WIDTH);
		//show the median filtered image
		//char temp[100];
		//sprintf(temp, "%s%s mean :%dx%d", winP2.c_str(), problem, mask_dim, mask_dim);
		//sprintf(temp, "%s%s mean", winP2.c_str(), problem);
		//sprintf(temp, "%s mean", winP2.c_str());
		winP2 = "mean";
		namedWindow( winP2, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
		moveWindow(winP2, 300,300+SCR_Y_OFFSET);
		cvShowImage( winP2.c_str(), imgP2b );                   // Show our image inside it.

		//////////////////////////////////////////
		//bonus
		//show histogram of image
		unsigned hist_tableI[MAX_GREY_LEVEL];
		uint8_t histeq_mapI[MAX_GREY_LEVEL];
		hist(hist_tableI, MAX_GREY_LEVEL, buf_workb, WIDTH, HEIGHT);
		draw_hist(hist_tableI, MAX_GREY_LEVEL, winP2, 600, 300+SCR_Y_OFFSET);

		//histogram equlization of Image
		uint8_t *buf_he= (uint8_t *)malloc( WIDTH * HEIGHT);
		unsigned cdf_table[MAX_GREY_LEVEL];
		hist_eq(buf_workb, buf_he,  WIDTH * HEIGHT, hist_tableI, cdf_table,
				MAX_GREY_LEVEL,	histeq_mapI, winP2);

		//show cdf of image
		string t_name(winP2 + " cdf ");
		draw_hist(cdf_table, MAX_GREY_LEVEL, t_name, 800, 300+SCR_Y_OFFSET);
		
		//show the image, the histogram equlization of Image I
		IplImage* imgHE = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
		cvSetData(imgHE, buf_he, WIDTH);
		namedWindow( winP2 + "Hist eq", CV_WINDOW_AUTOSIZE );	// Create a window for display.
		moveWindow( winP2 + "Hist eq", 1050,300+SCR_Y_OFFSET);
		string winP2I = winP2 + "Hist eq";
		char win_hname[100];
		strcpy(win_hname, winP2I.c_str());
		cvShowImage( win_hname, imgHE );                   // Show our image inside it.
		
		//PSNR
		char psnr_str[100];
		float psnr = PSNR(bufRaw, buf_he, WIDTH, HEIGHT);
		sprintf(psnr_str, "PSNR of mean=%f", psnr);
		cvPrintf(imgBar, psnr_str, cvPoint(1, 80),
									cvFont(1.0, 1.0), CV_RGB(255,255,255));
		cvShowImage( track_bar_name.c_str(), imgBar);
	}
#if 0	
	//diff src and filtered image
	img_diff(bufRaw, buf_work, buf_diff, WIDTH, HEIGHT);
	cvSetData(imgP2D, buf_diff, WIDTH);
	//show the diff image
	winP2D = (winP2D + problem) + " diff";
	namedWindow( winP2D, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow(winP2D, 300,0);
	cvShowImage( winP2D.c_str(), imgP2D);                   // Show our image inside it.
#endif

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

	/////////////////////////////////////
	//tracking bar to set dimension of kernel matrix
	imgBar = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3);
	cvSetZero(imgBar);
	cv::namedWindow(track_bar_name, WINDOW_AUTOSIZE);
	cv::createTrackbar("dim", track_bar_name, &mask_dim, MAX_DIM, ProcessDim, 
						bufRaw);
	//CvFont Font1=cvFont(1.5,1.0);
	//cvPutText(imgBar, "text", cvPoint(10, 50), &Font1, CV_RGB(250,255,255));

	moveWindow(track_bar_name, 1300,0+SCR_Y_OFFSET);
	cvShowImage( track_bar_name.c_str(), imgBar);
	//////////////////////////////////
	
	//load raw file
	string folder, winRaw;
	SplitFilename (raw_file, folder, winRaw);
	IplImage* imgRaw = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	cvSetData(imgRaw, bufRaw, WIDTH);
	//show the raw image
	namedWindow( winRaw, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow(winRaw, 0,0+SCR_Y_OFFSET);
	cvShowImage( winRaw.c_str(), imgRaw );                   // Show our image inside it.

	//Problem solution
	ProcessDim(mask_dim, bufRaw);
	
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

#if 0
	destroyWindow(winP2);
	cvReleaseImageHeader(&imgP2);
	free(buf_work);

	destroyWindow(winP2D);
	cvReleaseImageHeader(&imgP2D);
	free(buf_diff);
#endif

    return 0;
}
