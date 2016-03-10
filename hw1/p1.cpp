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

using namespace cv;
using namespace std;
#include "dip.h"

const char org_display[]="Original Display";
const char flip_v[]="vertically flipped";
const char flip_h[]="horizontally flipped";

char raw_win_nameD[1024]="sample2.raw";
char raw_fileI[1024]="sample1.raw";

static void usage(FILE *fp, int argc, char **argv)
{
	fprintf(fp,
		 "Usage: %s [options]\n\n"
		 "Options:\n"
		 "-h | --help       Print this message\n"
		 "-D | --rawD    The full path of the raw file D \n"
		 "-I | --rawI    The full path of the raw file I \n"
		 "",
		 argv[0]);
}

static const char short_options[] = "D:hI:v";

static const struct option
long_options[] = {
	{ "help",   	no_argument,       NULL, 'h' },
	{ "rawD",		required_argument, NULL, 'D' },
	{ "rawI",		required_argument, NULL, 'I' },
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
		case 'D':
			errno = 0;
			strncpy(raw_win_nameD, optarg, 1024);
			printf("%s\n", raw_win_nameD);
			if (errno){
				r=-1;
			}
			break;
		case 'I':
			errno = 0;
			strncpy(raw_fileI, optarg, 1024);
			printf("%s\n", raw_fileI);
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

/** @brief Draw the histograms
 * input 
 * hist_table[] : histogram table 
 * h_size : level of histogram table, ie, 256 grey levels
 */
void draw_hist(unsigned *hist_table, int h_size, const string &t_name, int wx=300, int wy=300)
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

	//opening file I and file D
	printf("I:%s\nD:%s\n", raw_fileI, raw_win_nameD);
	uint8_t *bufI=NULL, *bufD=NULL, *bufH=NULL, *bufL=NULL;
	if( (fd = open(raw_fileI, O_RDONLY)) != -1 ){
		bufI= (uint8_t *)malloc( WIDTH * HEIGHT);
		ssize_t s=read(fd, bufI, WIDTH * HEIGHT);
		close(fd);
		cout << "request size = " << WIDTH * HEIGHT << ", read size=" << s << endl;
	}else{
		cout << raw_fileI << " opening failure!:"<< errno << endl;
		exit(EXIT_FAILURE);
	}
	
	if( (fd = open(raw_win_nameD, O_RDONLY)) != -1 ){
		bufD= (uint8_t *)malloc( WIDTH * HEIGHT);
		ssize_t s=read(fd, bufD, WIDTH * HEIGHT);
		close(fd);
		cout << "request size = " << WIDTH * HEIGHT << ", read size=" << s << endl;
	}else{
		cout << raw_win_nameD << " opening failure!:"<< errno << endl;
		exit(EXIT_FAILURE);
	}

	//load raw file I
	string folder, fileI, win_nameD;
	SplitFilename (raw_fileI, folder, fileI);
	IplImage* org_imgI = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	cvSetData(org_imgI, bufI, WIDTH);
	//show the raw image I
	namedWindow( fileI, CV_WINDOW_AUTOSIZE );	// Create a window for display.
	moveWindow(fileI, 100,100);
	cvShowImage( fileI.c_str(), org_imgI );                   // Show our image inside it.

	//show histogram of image I
	unsigned hist_tableI[MAX_GREY_LEVEL];
	uint8_t histeq_mapI[MAX_GREY_LEVEL];
	hist(hist_tableI, MAX_GREY_LEVEL, bufI, WIDTH, HEIGHT);
	draw_hist(hist_tableI, MAX_GREY_LEVEL, fileI, 250, 250);

	//histogram equlization of Image I
	uint8_t *bufII= (uint8_t *)malloc( WIDTH * HEIGHT);
	unsigned cdf_table[MAX_GREY_LEVEL];
	hist_eq(bufI, bufII,  WIDTH * HEIGHT, hist_tableI, cdf_table,
			MAX_GREY_LEVEL,	histeq_mapI, fileI);

	//show cdf of image I
	string t_name(fileI + " cdf ");
	draw_hist(cdf_table, MAX_GREY_LEVEL, t_name/*, int wx=300, int wy=300*/);
	
	//show the image II, the histogram equlization of Image I
	IplImage* imgII = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	cvSetData(imgII, bufII, WIDTH);
	namedWindow( fileI + "Hist eq", CV_WINDOW_AUTOSIZE );	// Create a window for display.
	moveWindow( fileI + "Hist eq", 250,250);
	string fileII = fileI + "Hist eq";
	char win_hname[100];
	strcpy(win_hname, fileII.c_str());
	cvShowImage( win_hname, imgII );                   // Show our image inside it.
	
	//load raw file D
	SplitFilename (raw_win_nameD, folder, win_nameD);
	IplImage* org_imgD = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	cvSetData(org_imgD, bufD, WIDTH);
	//show the image D
	namedWindow( win_nameD, CV_WINDOW_AUTOSIZE );	// Create a window for display.
	moveWindow(win_nameD, 250,250);
	cvShowImage( win_nameD.c_str(), org_imgD );                   // Show our image inside it.

	//show histogram of image D
	unsigned hist_tableD[MAX_GREY_LEVEL];
	uint8_t histeq_mapH[MAX_GREY_LEVEL];
	hist(hist_tableD, MAX_GREY_LEVEL, bufD, WIDTH, HEIGHT);
	draw_hist(hist_tableD, MAX_GREY_LEVEL, win_nameD, 350,250);

	//histogram equlization of image D
	bufH= (uint8_t *)malloc( WIDTH * HEIGHT);
	hist_eq(bufD, bufH,  WIDTH * HEIGHT, hist_tableD, cdf_table,
			MAX_GREY_LEVEL,	histeq_mapH, win_nameD);

	//show cdf of image D
	t_name=win_nameD + " cdf ";
	draw_hist(cdf_table, MAX_GREY_LEVEL, t_name/*, int wx=300, int wy=300*/);

	//show image H, the histogram equlization of image D
	IplImage* imgH = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	cvSetData(imgH, bufH, WIDTH);
	string win_nameH =  win_nameD + "Hist eq";
	namedWindow( win_nameH, CV_WINDOW_AUTOSIZE );	// Create a window for display.
	moveWindow( win_nameH, 250,250);
	strcpy(win_hname, win_nameH.c_str());
	cvShowImage( win_hname, imgH );                   // Show our image inside it.
	//show histogram of image H
	unsigned hist_tableH[MAX_GREY_LEVEL];
	hist(hist_tableH, MAX_GREY_LEVEL, bufH, WIDTH, HEIGHT);
	draw_hist(hist_tableH, MAX_GREY_LEVEL, win_nameH, 350,250);

	//create output file H
	if( (fd = open("H.raw", O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR) ) != -1 ){
		int s=write(fd, bufH, WIDTH * HEIGHT);
		cout << "write:" << s << endl;
		close(fd);
	}else{
		cout << "H.raw open failed!"  << endl;
	}

	cout << "press any key to quit..." << endl;
	waitKey(0);                                          // Wait for a keystroke in the window

	cvDestroyWindow(fileI.c_str());
	cvReleaseImageHeader(&org_imgI);
	free(bufI);

	cvDestroyWindow(win_nameD.c_str());
	cvReleaseImageHeader(&org_imgD);
	free(bufD);

	destroyWindow(fileII);
	cvReleaseImageHeader(&imgII);
	free(bufII);

	destroyWindow(win_nameH);
	cvReleaseImageHeader(&imgH);
	free(bufH);
	
    return 0;
}
