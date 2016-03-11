/** @brief DIP program to flip an image
 * @author <Thomas Tsai, thomas@life100.cc>
 *
 * ./bin/p2 -n 3 -o 0x600 -p 2a -r ../../assignment/hw1/sample1.raw
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
#include "helper.h"
//MAX kernel matrix dimension
#define	MAX_DIM		(33)

const char org_display[]="Original Display";
const char flip_v[]="vertically flipped";
const char flip_h[]="horizontally flipped";

char raw_fileD[1024]="sample2.raw";
char raw_file[1024]="sample3.raw";
char problem[100]="2a";
int mask_dim=3;
int add_noise=0;

static void usage(FILE *fp, int argc, char **argv)
{
	fprintf(fp,
		 "Usage: %s [options]\n\n"
		 "Options:\n"
		 "-h | --help        Print this message\n"
		 "-a | --add         Adding noise to raw image\n"
		 "-n | --dim     n   nxn mask\n"
		 "-o | --offset  mxn The screen offset for dual screen\n"
		 "-p | --problem 2a  The problem 2a,2b,2c... to solve\n"
		 "-r | --raw	     The full path of the raw file \n"
		 "",
		 argv[0]);
}

static const char short_options[] = "ahn:o:p:r:";

static const struct option
long_options[] = {
	{ "add",   		no_argument,       NULL, 'a' },
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
		case 'a':
			add_noise=1;
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

IplImage* imgImpAdd=NULL, *imgImpFiltered=NULL, *imgP2a=NULL;
int black_thr=3,white_thr=252;
void show_impulse_noise(uint8_t *imp, int width, int height)
{
	IplImage* imgImPulse = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 1);
	cvSetData(imgImPulse, imp, width);
	//show the raw image
	namedWindow( "impulse noise", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("impulse noise", 300,0+SCR_Y_OFFSET);
	cvShowImage( "impulse noise", imgImPulse );                   // Show our image inside it.
}

uint8_t *imp_buf=NULL, *buf_addImp=NULL, *buff_Iwork=NULL;
void impulse_bchange(int pos, void *userdata)
{
	uint8_t *src=(uint8_t *)userdata;
	black_thr=pos;

	imp_buf=(uint8_t *)realloc(imp_buf, WIDTH * HEIGHT);
	impulse_noise_gen(imp_buf, HEIGHT, WIDTH, black_thr, white_thr);
	show_impulse_noise(imp_buf, WIDTH, HEIGHT);
	//add impulse noise to image
	uint8_t *bufRaw=(uint8_t *)userdata;
	buf_addImp= (uint8_t *)realloc(buf_addImp, WIDTH * HEIGHT);
	memcpy(buf_addImp,bufRaw, WIDTH * HEIGHT);
	if(add_noise) impulse_noise_add(imp_buf,buf_addImp, HEIGHT, WIDTH);

	cvSetData(imgImpAdd, buf_addImp, WIDTH);
	namedWindow("impulse noise add", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("impulse noise add", 600,0+SCR_Y_OFFSET);
	cvShowImage("impulse noise add", imgImpAdd );                   // Show our image inside it.
	
	//Problem 2a : median filter
	buff_Iwork=(uint8_t *)realloc(buff_Iwork, WIDTH * HEIGHT);
	//median
	p2a(buf_addImp, buff_Iwork, WIDTH, HEIGHT, mask_dim);
	//show the median filtered image
	cvSetData(imgP2a, buff_Iwork, WIDTH);
	namedWindow( "median", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("median", 1000,0+SCR_Y_OFFSET);
	cvShowImage( "median", imgP2a );                   // Show our image inside it
}

void impulse_wchange(int pos, void *userdata)
{
	uint8_t *src=(uint8_t *)userdata;
	white_thr=pos;
	imp_buf=(uint8_t *)realloc(imp_buf, WIDTH * HEIGHT);
	impulse_noise_gen(imp_buf, HEIGHT, WIDTH, black_thr, white_thr);
	show_impulse_noise(imp_buf, WIDTH, HEIGHT);
	uint8_t *bufRaw=(uint8_t *)userdata;
	//add impulse noise to image
	buf_addImp= (uint8_t *)realloc(buf_addImp, WIDTH * HEIGHT);
	memcpy(buf_addImp,bufRaw, WIDTH * HEIGHT);
	if(add_noise)	impulse_noise_add(imp_buf,buf_addImp, HEIGHT, WIDTH);

	cvSetData(imgImpAdd, buf_addImp, WIDTH);
	namedWindow("impulse noise add", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("impulse noise add", 600,0+SCR_Y_OFFSET);
	cvShowImage("impulse noise add", imgImpAdd );                   // Show our image inside it.

	//Problem 2a : median filter
	buff_Iwork=(uint8_t *)realloc(buff_Iwork, WIDTH * HEIGHT);
	p2a(buf_addImp, buff_Iwork, WIDTH, HEIGHT, mask_dim);

	cvSetData(imgP2a, buff_Iwork, WIDTH);
	//show the median filtered image
	namedWindow( "median", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("median", 1000,0+SCR_Y_OFFSET);
	cvShowImage( "median", imgP2a );                   // Show our image inside it

}

int whiten_thr=254;
IplImage* imgWhiteAdd=NULL, *imgWhiteFiltered=NULL, *imgP2b=NULL;
void show_white_noise(uint8_t *white, int width, int height)
{
	IplImage* imgWhiteN = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 1);
	cvSetData(imgWhiteN, white, width);
	//show the white noise
	namedWindow( "white noise", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("white noise", 300,300+SCR_Y_OFFSET);
	cvShowImage( "white noise", imgWhiteN );                   // Show our image inside it.
}

uint8_t *white_buf=NULL, *buf_addWhite= NULL, *buff_wwork=NULL;
void whiten_wchange(int pos, void *userdata)
{
	white_buf=(uint8_t *)realloc(white_buf, WIDTH * HEIGHT);
	whiten_thr=pos;
	white_noise_gen(white_buf, HEIGHT, WIDTH, whiten_thr);
	show_white_noise(white_buf, WIDTH, HEIGHT);

	//add white noise to image
	uint8_t *bufRaw=(uint8_t *)userdata;
	buf_addWhite= (uint8_t *)realloc(buf_addWhite, WIDTH * HEIGHT);
	memcpy(buf_addWhite,bufRaw, WIDTH * HEIGHT);

	//adding noise to image I
	if(add_noise) white_noise_add(white_buf, buf_addWhite, HEIGHT, WIDTH);

	cvSetData(imgWhiteAdd, buf_addWhite, WIDTH);
	namedWindow("white noise add", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("white noise add", 600,300+SCR_Y_OFFSET);
	cvShowImage("white noise add", imgWhiteAdd );                   // Show our image inside it.

	//Problem 2b : mean filter
	buff_wwork=(uint8_t *)realloc(buff_wwork, WIDTH * HEIGHT);
	//mean filter
	p2b(buf_addWhite, buff_wwork, WIDTH, HEIGHT, mask_dim);
	//show it
	cvSetData(imgP2b, buff_wwork, WIDTH);
	namedWindow( "mean", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("mean", 1000,300+SCR_Y_OFFSET);
	cvShowImage( "mean", imgP2b );                   // Show our image inside it.
}

IplImage *imgBar=NULL;
void ProcessDim(int pos, void *userdata)
{
	printf(">>%s:pos=%d, mask_dim=%d\n", __func__, pos, mask_dim);
	if(pos < 3 ) {
		pos=3;
	}
	if((pos%2)==0) {
		pos--;//only odd dim is allowed!
	}
	mask_dim = pos;
	printf("<<%s:pos=%d, mask_dim=%d\n", __func__, pos, mask_dim);
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

	////////////////////////////////////////////
	//GUI preparation
	imgImpAdd = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	imgImpFiltered= cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	imgWhiteAdd = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	imgWhiteFiltered = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	imgP2a = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	imgP2b = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);

	//imp_buf=(uint8_t *)realloc(imp_buf, WIDTH * HEIGHT);
	//show_impulse_noise(imp_buf, WIDTH, HEIGHT);
	cv::namedWindow("noise_tune", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
	imgBar = cvCreateImage(cvSize(512, 512), IPL_DEPTH_8U, 3);
	cvSetZero(imgBar);
	cv::createTrackbar("kernel dimension", "noise_tune", &mask_dim, MAX_DIM, ProcessDim, 
						bufRaw);
	cv::createTrackbar("imp black threshold <", "noise_tune", &black_thr, MAX_GREY_LEVEL, 
					   impulse_bchange, bufRaw);
	cv::createTrackbar("imp white threshold >", "noise_tune", &white_thr, MAX_GREY_LEVEL, 
					   impulse_wchange, bufRaw);
	//white_buf=(uint8_t *)realloc(white_buf, WIDTH * HEIGHT);
	//show_white_noise(white_buf, WIDTH, HEIGHT);
	cv::createTrackbar("white noise threshold >", "noise_tune", &white_thr, MAX_GREY_LEVEL, 
					   whiten_wchange, bufRaw);
	moveWindow("noise_tune", 1300,300+SCR_Y_OFFSET);
	cvShowImage( "noise_tune", imgBar);
	////////////////////////////////////////////

	string winP2="P", winP2D="P";
	
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
	
	//destroyWindow("median");
	cvReleaseImageHeader(&imgP2a);
	//free(buf_work);

	//destroyWindow(winP2D);
	cvReleaseImageHeader(&imgP2b);
	//free(buf_diff);

    return 0;
}
