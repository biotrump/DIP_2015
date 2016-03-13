/** @brief DIP program to flip an image
 * @author <Thomas Tsai, thomas@life100.cc>
 *
 * ./bin/p2 -p m+M -n 3 -r ../../assignment/hw1/sample3.raw
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

#define	WIN_GAP_X	(280)
#define	WIN_GAP_Y	(300)

string wname_tune("noise_tune");
char raw_file[1024]="sample3.raw";
int mask_dim=3;
int add_noise=0;
int method = 0 ;

int median_filter=0;
int mean_filter=0;

static void usage(FILE *fp, int argc, char **argv)
{
	fprintf(fp,
		 "Usage: %s [options]\n\n"
		 "Options:\n"
		 "-h | --help                 Print this message\n"
		 "-a | --add                  Adding noise to raw image\n"
		 "-n | --dim     n            nxn kernel mask\n"
		 "-o | --offset  mxn          The screen offset for dual screen\n"
		 "-p | --perform [M, m, M+m]  M: median, m:mean, M+m:both\n"
		 "-r | --raw	              The full path of the raw file \n"
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
	{ "perform",	required_argument, NULL, 'p' },
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
			if(strcmp(optarg,"M") == 0)
				median_filter=1;
			if(strcmp(optarg,"m") == 0)
				mean_filter=1;
			if(strcmp(optarg,"M+m") == 0 || strcmp(optarg,"m+M") == 0){
				median_filter=1;
				mean_filter=1;
			}
			printf("%s:Median=%d, mean=%d\n", optarg, median_filter, mean_filter);
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


IplImage *imgBar=NULL;
IplImage* imgImpAdd=NULL, *imgImpFiltered=NULL, *imgMedian=NULL;
int black_thr=3,white_thr=252;
char psnr_mean_str[100], psnr_median_str[100];

void display_PSNR(char *median_str, char *mean_str)
{
	if(median_str)
		strcpy(psnr_median_str, median_str);
	if(mean_str)
		strcpy(psnr_mean_str, mean_str);
	cvSetZero(imgBar);
	
	cvPrintf(imgBar, psnr_median_str, cvPoint(1, 40));
	cvPrintf(imgBar, psnr_mean_str, cvPoint(1, 80));
	cvShowImage( wname_tune.c_str(), imgBar);	
}

void show_impulse_noise(uint8_t *imp, int width, int height)
{
	IplImage* imgImPulse = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 1);
	cvSetData(imgImPulse, imp, width);
	//show the raw image
	namedWindow( "impulse noise", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("impulse noise", WIN_GAP_X + SCR_X_OFFSET,SCR_Y_OFFSET);
	cvShowImage( "impulse noise", imgImPulse );                   // Show our image inside it.
}

uint8_t *imp_buf=NULL, *buf_addImp=NULL, *buff_Iwork=NULL;
void impulse_bchange(int pos, void *userdata)
{
	black_thr=pos;

	imp_buf=(uint8_t *)realloc(imp_buf, WIDTH * HEIGHT);
	impulse_noise_gen(imp_buf, HEIGHT, WIDTH, black_thr, white_thr);
	show_impulse_noise(imp_buf, WIDTH, HEIGHT);
	//add impulse noise to image
	uint8_t *bufRaw=(uint8_t *)userdata;
	buf_addImp= (uint8_t *)realloc(buf_addImp, WIDTH * HEIGHT);
	memcpy(buf_addImp,bufRaw, WIDTH * HEIGHT);
	if(add_noise) {
		impulse_noise_add(imp_buf,buf_addImp, HEIGHT, WIDTH);

		cvSetData(imgImpAdd, buf_addImp, WIDTH);
		namedWindow("impulse noise add", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
		moveWindow("impulse noise add", WIN_GAP_X*2 + SCR_X_OFFSET, SCR_Y_OFFSET);
		cvShowImage("impulse noise add", imgImpAdd );                   // Show our image inside it.
	}
	
	//median filter
	buff_Iwork=(uint8_t *)realloc(buff_Iwork, WIDTH * HEIGHT);
	//median
	median(buf_addImp, buff_Iwork, WIDTH, HEIGHT, mask_dim);
	//show the median filtered image
	cvSetData(imgMedian, buff_Iwork, WIDTH);
	namedWindow( "median", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("median", WIN_GAP_X*3 + SCR_X_OFFSET,SCR_Y_OFFSET);
	cvShowImage( "median", imgMedian );                   // Show our image inside it
	
	//PSNR
	char psnr_str[100];
	float psnr = PSNR(bufRaw, buff_Iwork, WIDTH, HEIGHT);
	sprintf(psnr_str, "R1: PSNR of median=%f", psnr);
	display_PSNR(psnr_str, NULL);
}

void impulse_wchange(int pos, void *userdata)
{
	white_thr=pos;
	imp_buf=(uint8_t *)realloc(imp_buf, WIDTH * HEIGHT);
	impulse_noise_gen(imp_buf, HEIGHT, WIDTH, black_thr, white_thr);
	show_impulse_noise(imp_buf, WIDTH, HEIGHT);
	uint8_t *bufRaw=(uint8_t *)userdata;
	//add impulse noise to image
	buf_addImp= (uint8_t *)realloc(buf_addImp, WIDTH * HEIGHT);
	memcpy(buf_addImp,bufRaw, WIDTH * HEIGHT);
	if(add_noise){
		impulse_noise_add(imp_buf,buf_addImp, HEIGHT, WIDTH);

		cvSetData(imgImpAdd, buf_addImp, WIDTH);
		namedWindow("impulse noise add", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
		moveWindow("impulse noise add", WIN_GAP_X*2 + SCR_X_OFFSET, SCR_Y_OFFSET);
		cvShowImage("impulse noise add", imgImpAdd );                   // Show our image inside it.
	}

	//median filter
	buff_Iwork=(uint8_t *)realloc(buff_Iwork, WIDTH * HEIGHT);
	median(buf_addImp, buff_Iwork, WIDTH, HEIGHT, mask_dim);

	cvSetData(imgMedian, buff_Iwork, WIDTH);
	//show the median filtered image
	namedWindow( "median", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("median", WIN_GAP_X*3 + SCR_X_OFFSET, SCR_Y_OFFSET);
	cvShowImage( "median", imgMedian );                   // Show our image inside it

	//PSNR
	char psnr_str[100];
	float psnr = PSNR(bufRaw, buff_Iwork, WIDTH, HEIGHT);
	sprintf(psnr_str, "PSNR of median=%f", psnr);
	display_PSNR(psnr_str, NULL);
}

int whiten_thr=254;
IplImage* imgWhiteAdd=NULL, *imgWhiteFiltered=NULL, *imgMean=NULL;
void show_white_noise(uint8_t *white, int width, int height)
{
	IplImage* imgWhiteN = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 1);
	cvSetData(imgWhiteN, white, width);
	//show the white noise
	namedWindow( "white noise", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("white noise", WIN_GAP_X + SCR_X_OFFSET,WIN_GAP_Y+SCR_Y_OFFSET);
	cvShowImage( "white noise", imgWhiteN );                   // Show our image inside it.
}

uint8_t *white_buf=NULL, *buf_addWhite= NULL, *buff_wwork=NULL;
void whiten_wchange(int pos, void *userdata)
{
	white_buf=(uint8_t *)realloc(white_buf, WIDTH * HEIGHT);
	whiten_thr=pos;
	//generate white noise
	white_noise_gen(white_buf, HEIGHT, WIDTH, whiten_thr);
	//show it
	show_white_noise(white_buf, WIDTH, HEIGHT);

	//add white noise to image
	uint8_t *bufRaw=(uint8_t *)userdata;
	buf_addWhite= (uint8_t *)realloc(buf_addWhite, WIDTH * HEIGHT);
	memcpy(buf_addWhite,bufRaw, WIDTH * HEIGHT);

	//adding noise to image I
	if(add_noise){
		white_noise_add(white_buf, buf_addWhite, HEIGHT, WIDTH);

		cvSetData(imgWhiteAdd, buf_addWhite, WIDTH);
		namedWindow("white noise add", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
		moveWindow("white noise add", WIN_GAP_X*2 + SCR_X_OFFSET,WIN_GAP_Y+SCR_Y_OFFSET);
		cvShowImage("white noise add", imgWhiteAdd );                   // Show our image inside it.
	}

	buff_wwork=(uint8_t *)realloc(buff_wwork, WIDTH * HEIGHT);
	//mean filter
	mean(buf_addWhite, buff_wwork, WIDTH, HEIGHT, mask_dim);
	//show it
	cvSetData(imgMean, buff_wwork, WIDTH);
	namedWindow( "mean", WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow("mean", WIN_GAP_X*3 + SCR_X_OFFSET,WIN_GAP_Y+SCR_Y_OFFSET);
	cvShowImage( "mean", imgMean );                   // Show our image inside it.
	
	//PSNR
	char psnr_str[100];
	float psnr = PSNR(bufRaw, buff_wwork, WIDTH, HEIGHT);
	sprintf(psnr_str, "PSNR of mean=%f", psnr);
	display_PSNR(NULL, psnr_str);
}

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
	int cvFlag=WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED;

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

	//loading raw file
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
	namedWindow( winRaw, cvFlag);	// Create a window for display.
	moveWindow(winRaw, SCR_X_OFFSET,SCR_Y_OFFSET);
	cvShowImage( winRaw.c_str(), imgRaw );                   // Show our image inside it.

	////////////////////////////////////////////
	//GUI preparation
	imgImpAdd = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	imgImpFiltered= cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	imgWhiteAdd = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	imgWhiteFiltered = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	imgMedian = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	imgMean = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);

	cv::namedWindow(wname_tune, CV_WINDOW_AUTOSIZE);
	imgBar = cvCreateImage(cvSize(512, 200), IPL_DEPTH_8U, 3);
	cvSetZero(imgBar);
	if(median_filter || mean_filter)
		cv::createTrackbar("kernel dimension", wname_tune, &mask_dim, MAX_DIM, ProcessDim, 
						bufRaw);
	if(median_filter){
		cv::createTrackbar("imp: black threshold <", wname_tune, &black_thr, 
						   MAX_GREY_LEVEL,  impulse_bchange, bufRaw);
	
		cv::createTrackbar("imp: white threshold >", wname_tune, &white_thr, 
						   MAX_GREY_LEVEL,   impulse_wchange, bufRaw);
		impulse_wchange(white_thr, bufRaw);
	}
	if(mean_filter){
		cv::createTrackbar("white noise: threshold >", wname_tune, &white_thr, 
						   MAX_GREY_LEVEL, whiten_wchange, bufRaw);
		whiten_wchange(white_thr, bufRaw);
	}
	moveWindow(wname_tune, WIN_GAP_X*4 + SCR_X_OFFSET,WIN_GAP_Y+SCR_Y_OFFSET);
	cvShowImage( wname_tune.c_str(), imgBar);
	////////////////////////////////////////////

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
	cvReleaseImageHeader(&imgMedian);
	//free(buf_work);

	//destroyWindow(winP2D);
	cvReleaseImageHeader(&imgMean);
	//free(buf_diff);

    return 0;
}
