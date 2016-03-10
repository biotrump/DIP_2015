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

void p2a(uint8_t *src, uint8_t *dst, int width, int height, int dim=3)
{
	median(src, dst, width, height, dim);
}

void p2b(uint8_t *src, uint8_t *dst, int width, int height, int dim=3)
{
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

	//Problem solution
	IplImage* imgP2 = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	IplImage* imgP2D = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	string winP2="P", winP2D="P";
	uint8_t *buf_work=NULL, *buf_diff=NULL;
	buf_work= (uint8_t *)malloc( WIDTH * HEIGHT);
	buf_diff= (uint8_t *)malloc( WIDTH * HEIGHT);

	if( strcmp(problem, "2a") == 0){
		//Problem 2a : median filter
		p2a(bufRaw, buf_work, WIDTH, HEIGHT, mask_dim);

		cvSetData(imgP2, buf_work, WIDTH);
		//show the median filtered image
		char temp[100];
		sprintf(temp, "%s%s median :%dx%d", winP2.c_str(), problem, mask_dim, mask_dim);
		winP2 = temp;
		namedWindow( winP2, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
		moveWindow(winP2, 300,0);
		cvShowImage( winP2.c_str(), imgP2 );                   // Show our image inside it.
		
	}else if(strcmp(problem, "2b") == 0){
		//Problem 2b : mean filter
		p2a(bufRaw, buf_work, WIDTH, HEIGHT, mask_dim);

		cvSetData(imgP2, buf_work, WIDTH);
		//show the median filtered image
		char temp[100];
		sprintf(temp, "%s%s mean :%dx%d", winP2.c_str(), problem, mask_dim, mask_dim);
		winP2 = temp;
		namedWindow( winP2, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
		moveWindow(winP2, 300,0);
		cvShowImage( winP2.c_str(), imgP2 );                   // Show our image inside it.
	}
	//diff src and filtered image
	img_diff(bufRaw, buf_work, buf_diff, WIDTH, HEIGHT);
	cvSetData(imgP2D, buf_diff, WIDTH);
	//show the diff image
	winP2D = (winP2D + problem) + " diff";
	namedWindow( winP2D, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
	moveWindow(winP2D, 300,0);
	cvShowImage( winP2D.c_str(), imgP2D);                   // Show our image inside it.
	
	//////////////////////////////////////////
	//bonus
	//show histogram of image
	unsigned hist_tableI[MAX_GREY_LEVEL];
	uint8_t histeq_mapI[MAX_GREY_LEVEL];
	hist(hist_tableI, MAX_GREY_LEVEL, buf_work, WIDTH, HEIGHT);
	draw_hist(hist_tableI, MAX_GREY_LEVEL, winP2, 250, 250);

	//histogram equlization of Image
	uint8_t *bufII= (uint8_t *)malloc( WIDTH * HEIGHT);
	unsigned cdf_table[MAX_GREY_LEVEL];
	hist_eq(buf_work, bufII,  WIDTH * HEIGHT, hist_tableI, cdf_table,
			MAX_GREY_LEVEL,	histeq_mapI, winP2);

	//show cdf of image I
	string t_name(winP2 + " cdf ");
	draw_hist(cdf_table, MAX_GREY_LEVEL, t_name/*, int wx=300, int wy=300*/);
	
	//show the image II, the histogram equlization of Image I
	IplImage* imgII = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	cvSetData(imgII, bufII, WIDTH);
	namedWindow( winP2 + "Hist eq", CV_WINDOW_AUTOSIZE );	// Create a window for display.
	moveWindow( winP2 + "Hist eq", 250,250);
	string winP2I = winP2 + "Hist eq";
	char win_hname[100];
	strcpy(win_hname, winP2I.c_str());
	cvShowImage( win_hname, imgII );                   // Show our image inside it.

	
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
	
	destroyWindow(winP2);
	cvReleaseImageHeader(&imgP2);
	free(buf_work);

	destroyWindow(winP2D);
	cvReleaseImageHeader(&imgP2D);
	free(buf_diff);

    return 0;
}
