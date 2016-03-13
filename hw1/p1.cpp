/** @brief DIP program to flip an image
 * ./bin/p1 -n 17 -I ../../assignment/hw1/sample1.raw -D ../../assignment/hw1/sample2.raw 
 *
 * @author <Thomas Tsai, thomas@life100.cc>
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
#include "helper.h"

//MAX kernel matrix dimension
#define	MAX_DIM		(255)

#define	WIN_GAP_X	(280)
#define	WIN_GAP_Y	(300)

char raw_win_nameD[1024]="sample2.raw";
char raw_fileI[1024]="sample1.raw";
int mask_dim=3;
int local_histeq_only=0;

static void usage(FILE *fp, int argc, char **argv)
{
	fprintf(fp,
		 "Usage: %s [options]\n\n"
		 "Options:\n"
		 "-h | --help        Print this message\n"
		 "-D | --rawD        The full path of the raw file D \n"
		 "-l | --Local       local histogram on D only \n"
		 "-I | --rawI        The full path of the raw file I \n"
		 "-n | --dim     n   nxn mask\n"
		 "-o | --offset  mxn The screen offset for dual screen\n"
		 "",
		 argv[0]);
}

static const char short_options[] = "D:hI:ln:o:v";

static const struct option
long_options[] = {
	{ "rawD",		required_argument, NULL, 'D' },
	{ "dim",		required_argument, NULL, 'n' },
	{ "help",   	no_argument,       NULL, 'h' },
	{ "local",   	no_argument,       NULL, 'h' },
	{ "offset",		required_argument, NULL, 'o' },
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
		case 'l':
			local_histeq_only=1;
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
		default:
			usage(stderr, argc, argv);
			r=-1;
		}
	}
	//printf("-%s:%d\n",__func__, r);
	return r;
}

/** @brief performing local histogram equalization
 * 
 * 
 */
IplImage * imgL=NULL;
uint8_t *buf_lhe=NULL;
string wname_L;
void ProcessDim(int pos, void *userdata)
{
	uint8_t *bufD = (uint8_t *)userdata;
	string wname_D(":");
	int cvFlag=CV_WINDOW_AUTOSIZE/*WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED*/;
	printf(">>%s:pos=%d, mask_dim=%d\n", __func__, pos, mask_dim);
	if(pos < 3 ) {
		pos=3;
	}
	if((pos%2)==0) {
		pos--;//only odd dim is allowed!
	}
	mask_dim = pos;
	printf("<<%s:pos=%d, mask_dim=%d\n", __func__, pos, mask_dim);

	//local histogram equlization to Image D
	buf_lhe= (uint8_t *)realloc( buf_lhe, WIDTH * HEIGHT);
	string wname_Dlhe=wname_D + " lhe ";
	local_hist_eq(bufD, buf_lhe,  WIDTH, HEIGHT,
			MAX_GREY_LEVEL,	mask_dim, wname_Dlhe);

	//show image L, the local histogram equalization of image D
	wname_L =  wname_D + " L: local Hist eq";
	if(local_histeq_only)
		imgL= cvDisplay(&imgL, buf_lhe, WIDTH, HEIGHT, WIN_GAP_X*2+SCR_X_OFFSET,
						WIN_GAP_Y+SCR_Y_OFFSET, wname_L, cvFlag);
	else
		imgL= cvDisplay(&imgL, buf_lhe, WIDTH, HEIGHT, WIN_GAP_X*2+SCR_X_OFFSET,
						WIN_GAP_Y*2+SCR_Y_OFFSET, wname_L, cvFlag);

	//show histogram of image L
	unsigned hist_tableL[(MAX_GREY_LEVEL+1)],cdf_tableL[(MAX_GREY_LEVEL+1)];
	hist(hist_tableL, (MAX_GREY_LEVEL+1), buf_lhe, WIDTH, HEIGHT);
	string wname_Lhist =  wname_D + " L: Hist";
	if(local_histeq_only)
		draw_hist(hist_tableL, (MAX_GREY_LEVEL+1), wname_Lhist, WIN_GAP_X+SCR_X_OFFSET,
				  WIN_GAP_Y+SCR_Y_OFFSET,	cvFlag);
	else
		draw_hist(hist_tableL, (MAX_GREY_LEVEL+1), wname_Lhist, WIN_GAP_X+SCR_X_OFFSET,
			  WIN_GAP_Y*2+SCR_Y_OFFSET,	cvFlag);

	//show cdf of image L
	hist_cdf(hist_tableL, cdf_tableL, (MAX_GREY_LEVEL+1) , WIDTH * HEIGHT);
	string wname_Lcdf = wname_D + " L: cdf ";
	if(local_histeq_only)
		draw_hist(cdf_tableL, (MAX_GREY_LEVEL+1), wname_Lcdf, 0+SCR_X_OFFSET ,
				  WIN_GAP_Y+SCR_Y_OFFSET,	cvFlag,Scalar( 0, 0 , 255) );
	else
		draw_hist(cdf_tableL, (MAX_GREY_LEVEL+1), wname_Lcdf, 0+SCR_X_OFFSET ,
				  WIN_GAP_Y*2+SCR_Y_OFFSET,	cvFlag,Scalar( 0, 0 , 255) );

	int fd=-1;
	//create output file L
	if( (fd = open("L.raw", O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR) ) != -1 ){
		int s=write(fd, buf_lhe, WIDTH * HEIGHT);
		cout << "write:" << s << endl;
		close(fd);
	}else{
		cout << "L.raw open failed!"  << endl;
	}
}

/**
 *
 */
int main( int argc, char** argv )
{
    int fd=-1;
	int cvFlag=CV_WINDOW_AUTOSIZE/*WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED*/;
	
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

	//loading file I and file D
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

	//show raw file I
	string folder, fileI;
	string wname_I, wname_Ip;
	SplitFilename (raw_fileI, folder, wname_I);
	wname_Ip = wname_I + ": I";
	IplImage * imgI= NULL;
	if(!local_histeq_only)
		cvDisplay(&imgI, bufI, WIDTH, HEIGHT, WIN_GAP_X*3+SCR_X_OFFSET,0+SCR_Y_OFFSET, 
						wname_Ip, cvFlag);

	//show histogram of image I
	unsigned hist_tableI[(MAX_GREY_LEVEL+1)];
	uint8_t histeq_mapI[(MAX_GREY_LEVEL+1)];
	hist(hist_tableI, (MAX_GREY_LEVEL+1), bufI, WIDTH, HEIGHT);
	string wname_Ihist = wname_I + " I: hist ";
	if(!local_histeq_only)
		draw_hist(hist_tableI, (MAX_GREY_LEVEL+1), wname_Ihist, WIN_GAP_X*4, 
				  0+SCR_Y_OFFSET, cvFlag);

	//histogram equlization of Image I
	uint8_t *bufII= (uint8_t *)malloc( WIDTH * HEIGHT);
	unsigned cdf_table[(MAX_GREY_LEVEL+1)];
	hist_eq(bufI, bufII,  WIDTH * HEIGHT, hist_tableI, cdf_table,
			(MAX_GREY_LEVEL+1),	histeq_mapI);

	//show cdf of image I
	string wname_Icdf = wname_I + " I: cdf ";
	if(!local_histeq_only)
		draw_hist(cdf_table, (MAX_GREY_LEVEL+1), wname_Icdf,WIN_GAP_X*5+SCR_X_OFFSET, 
				  0+SCR_Y_OFFSET,cvFlag, Scalar( 0, 0 , 255)	);
	
	//show the image II, the histogram equlization of Image I
	string wname_II;
	wname_II = wname_I + " II: hist eq";
	IplImage * imgII= NULL;
	if(!local_histeq_only)
		cvDisplay(&imgII, bufII, WIDTH, HEIGHT, WIN_GAP_X*3+SCR_X_OFFSET,
				  WIN_GAP_Y+SCR_Y_OFFSET, wname_II, cvFlag);

	//show raw image D
	string wname_D, wname_Dp;
	SplitFilename (raw_win_nameD, folder, wname_D);
	wname_Dp = wname_D + ": D";
	IplImage * imgD= NULL;
	if(!local_histeq_only)
		cvDisplay(&imgD, bufD, WIDTH, HEIGHT, WIN_GAP_X*2+SCR_X_OFFSET,0+SCR_Y_OFFSET, 
						wname_Dp, cvFlag);
	
	//show histogram of image D
	unsigned hist_tableD[(MAX_GREY_LEVEL+1)];
	uint8_t histeq_mapH[(MAX_GREY_LEVEL+1)];
	//get histogram of D
	hist(hist_tableD, (MAX_GREY_LEVEL+1), bufD, WIDTH, HEIGHT);
	//draw the histogram
	string wname_Dhist = wname_D + " D: hist ";
	if(!local_histeq_only)
		draw_hist(hist_tableD, (MAX_GREY_LEVEL+1), wname_Dhist, WIN_GAP_X+SCR_X_OFFSET ,
				  0+SCR_Y_OFFSET,cvFlag);

	//histogram equlization of image D
	bufH= (uint8_t *)malloc( WIDTH * HEIGHT);
	hist_eq(bufD, bufH,  WIDTH * HEIGHT, hist_tableD, cdf_table,
			(MAX_GREY_LEVEL+1),	histeq_mapH);

	//create output file H
	if( (fd = open("H.raw", O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR) ) != -1 ){
		int s=write(fd, bufH, WIDTH * HEIGHT);
		cout << "write:" << s << endl;
		close(fd);
	}else{
		cout << "H.raw open failed!"  << endl;
	}

	//show cdf of image D
	string wname_Dcdf = wname_D + " D: cdf ";
	if(!local_histeq_only)
		draw_hist(cdf_table, (MAX_GREY_LEVEL+1), wname_Dcdf, 0+SCR_X_OFFSET ,
				  0+SCR_Y_OFFSET, cvFlag, Scalar( 0, 0 , 255) );

	//show image H, the histogram equlization of image D
	string wname_H =  wname_D + " H: Hist eq";
	IplImage *imgH= NULL;
	if(!local_histeq_only)
		cvDisplay(&imgH, bufH, WIDTH, HEIGHT, WIN_GAP_X*2+SCR_X_OFFSET,
				  WIN_GAP_Y+SCR_Y_OFFSET, wname_H, cvFlag);
	//show histogram of image H
	unsigned hist_tableH[(MAX_GREY_LEVEL+1)], cdf_tableH[(MAX_GREY_LEVEL+1)];
	hist(hist_tableH, (MAX_GREY_LEVEL+1), bufH, WIDTH, HEIGHT);
	string wname_Hhist =  wname_D + " H: Hist";
	if(!local_histeq_only)
		draw_hist(hist_tableH, (MAX_GREY_LEVEL+1), wname_Hhist, WIN_GAP_X*1+SCR_X_OFFSET,
				  WIN_GAP_Y+SCR_Y_OFFSET,cvFlag);

	//show cdf of image H
	hist_cdf(hist_tableH, cdf_tableH, (MAX_GREY_LEVEL+1) , WIDTH * HEIGHT);
	string wname_Hcdf = wname_D + " H: cdf ";
	if(!local_histeq_only)
		draw_hist(cdf_tableH, (MAX_GREY_LEVEL+1), wname_Hcdf, 0+SCR_X_OFFSET ,
				  WIN_GAP_Y+SCR_Y_OFFSET,cvFlag, Scalar( 0, 0 , 255) );

	////////////////////////////////////////////
	// dim of local histogram equalization
	// performing local histogram equalization
	string wname_dim("local histogram equalization");
	cv::namedWindow(wname_dim, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
	cv::createTrackbar("dimension", wname_dim, &mask_dim, MAX_DIM, ProcessDim, 
						bufD);
	if(local_histeq_only)
		moveWindow(wname_dim, WIN_GAP_X*3+SCR_X_OFFSET,WIN_GAP_Y+SCR_Y_OFFSET);
	else
		moveWindow(wname_dim, WIN_GAP_X*3+SCR_X_OFFSET,WIN_GAP_Y*2+SCR_Y_OFFSET);
	ProcessDim(mask_dim, bufD);//init before use interactive
	////////////////////////////////////////////

	cout << "press any key to quit..." << endl;
	waitKey(0);                                          // Wait for a keystroke in the window
	if(imgL){
		cvReleaseImageHeader(&imgL);
		destroyWindow(wname_L);
		free(buf_lhe);
	}
	if(imgH){
		cvReleaseImageHeader(&imgH);
		destroyWindow(wname_H);
		free(bufH);
	}

	destroyWindow(fileI);
	cvReleaseImageHeader(&imgI);
	free(bufI);

	if(imgD){
		destroyWindow(wname_Dp);
		cvReleaseImageHeader(&imgD);
		free(bufD);
	}
	
	if(imgI){
		destroyWindow(wname_Ip);
		cvReleaseImageHeader(&imgI);
		free(bufD);
	}
	if(imgII){
		destroyWindow(wname_II);
		cvReleaseImageHeader(&imgII);
		free(bufII);		
	}

	destroyWindow(wname_H);
	cvReleaseImageHeader(&imgH);
	free(bufH);
	
    return 0;
}
