/** @brief DIP program to flip an image
 * ./bin/p1e -n 100 -o 0x600 -D ../../assignment/hw1/sample2.raw
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
#define	POWER_MAX	(300)
#define	LOGT_MAX	(400)
#define	ILOGT_MAX	(400)
#define	POWER_DIV	(100.0f)
#define	LOGT_DIV	(50.0f)
#define	ILOGT_DIV	(50.0f)

#define	WIN_GAP_X	(280)
#define	WIN_GAP_Y	(300)

char raw_win_nameD[1024]="sample2.raw";
char raw_fileI[1024]="sample1.raw";
int mask_dim=3;

static void usage(FILE *fp, int argc, char **argv)
{
	fprintf(fp,
		 "Usage: %s [options]\n\n"
		 "Options:\n"
		 "-h | --help        Print this message\n"
		 "-D | --rawD        The full path of the raw file D \n"
		 "-I | --rawI        The full path of the raw file I \n"
		 "-n | --pow     n   power step [0-%d] mask\n"
		 "-o | --offset  mxn The screen offset for dual screen\n"
		 "",
		 argv[0], POWER_MAX);
}

static const char short_options[] = "D:hI:n:o:v";

static const struct option
long_options[] = {
	{ "rawD",		required_argument, NULL, 'D' },
	{ "dim",		required_argument, NULL, 'n' },
	{ "help",   	no_argument,       NULL, 'h' },
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
uint8_t *buf_powT=NULL;
uint8_t *bufDL=NULL;
uint8_t *bufDExp=NULL;
string wname_L;
void PowerLawTransform(int pos, void *userdata)
{
	uint8_t *bufD = (uint8_t *)userdata;
	string wname_D("D:");
	int cvFlag=CV_WINDOW_AUTOSIZE/*WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED*/;
	printf(">>%s:pos=%d, mask_dim=%d\n", __func__, pos, mask_dim);

	mask_dim = pos;
	printf("<<%s:pos=%d, mask_dim=%d\n", __func__, pos, mask_dim);

	//1. performing power law transform
	buf_powT= (uint8_t *)realloc( buf_powT, WIDTH * HEIGHT);
	power_transform(bufD, buf_powT,  WIDTH * HEIGHT,	pos/POWER_DIV, MAX_GREY_LEVEL);

	//show it
	string wname_Dlhe=wname_D + " power law transform ";
	imgL= cvDisplay(buf_powT, WIDTH, HEIGHT, WIN_GAP_X*2+SCR_X_OFFSET,WIN_GAP_Y+SCR_Y_OFFSET, 
						wname_Dlhe, cvFlag);

	//show histogram
	unsigned hist_table[(MAX_GREY_LEVEL+1)],cdf_table[(MAX_GREY_LEVEL+1)];
	hist(hist_table, (MAX_GREY_LEVEL+1), buf_powT, WIDTH, HEIGHT);
	string wname_Lhist =  wname_D + " PLT : Hist";
	draw_hist(hist_table, (MAX_GREY_LEVEL+1), wname_Lhist, WIN_GAP_X+SCR_X_OFFSET,WIN_GAP_Y+SCR_Y_OFFSET,
		cvFlag);

	//show cdf
	hist_cdf(hist_table, cdf_table, (MAX_GREY_LEVEL+1) , WIDTH * HEIGHT);
	string wname_Lcdf = wname_D + " PLT : cdf ";
	draw_hist(cdf_table, (MAX_GREY_LEVEL+1), wname_Lcdf, 0+SCR_X_OFFSET ,WIN_GAP_Y+SCR_Y_OFFSET,
		cvFlag,Scalar( 0, 0 , 255) );

	int fd=-1;
	//create output file
	if( (fd = open("p1e_powl.raw", O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR) ) != -1 ){
		int s=write(fd, buf_powT, WIDTH * HEIGHT);
		cout << "write:" << s << endl;
		close(fd);
	}else{
		cout << "p1e_powl.raw open failed!"  << endl;
	}
}

void LogTransform(int pos, void *userdata)
{
	uint8_t *bufD = (uint8_t *)userdata;
	string wname_D("D:");
	int cvFlag=CV_WINDOW_AUTOSIZE/*WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED*/;
	printf(">>%s:pos=%d, mask_dim=%d\n", __func__, pos, mask_dim);

	mask_dim = pos;
	printf("<<%s:pos=%d, mask_dim=%d\n", __func__, pos, mask_dim);

	//2. perform log transform
	bufDL=(uint8_t *)realloc(bufDL, WIDTH * HEIGHT);
	log_transform(bufD, bufDL, WIDTH * HEIGHT, pos/LOGT_DIV);
	string wname_DL = wname_D + ": log transform";
	IplImage * imgDL= cvDisplay(bufDL, WIDTH, HEIGHT, WIN_GAP_X*2+SCR_X_OFFSET,
								SCR_Y_OFFSET+SCR_Y_OFFSET, wname_DL, cvFlag);

	//show histogram of image
	unsigned cdf_tableDL[(MAX_GREY_LEVEL+1)];
	unsigned hist_tableDL[(MAX_GREY_LEVEL+1)];
	//get histogram
	hist(hist_tableDL, (MAX_GREY_LEVEL+1), bufDL, WIDTH, HEIGHT);
	//draw the histogram
	string wname_DLhist = wname_D + " DL: hist ";
	draw_hist(hist_tableDL, (MAX_GREY_LEVEL+1), wname_DLhist, WIN_GAP_X+SCR_X_OFFSET ,
			  SCR_Y_OFFSET+SCR_Y_OFFSET,cvFlag);

	//show cdf of image
	string wname_DLcdf = wname_D + " DL: cdf ";
	hist_cdf(hist_tableDL, cdf_tableDL, (MAX_GREY_LEVEL+1) , WIDTH * HEIGHT);
	draw_hist(cdf_tableDL, (MAX_GREY_LEVEL+1), wname_DLcdf, 0+SCR_X_OFFSET ,
			  SCR_Y_OFFSET+SCR_Y_OFFSET,cvFlag, Scalar( 0, 0 , 255) );

	int fd=-1;
	//create output file
	if( (fd = open("p1e_log.raw", O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR) ) != -1 ){
		int s=write(fd, buf_powT, WIDTH * HEIGHT);
		cout << "write:" << s << endl;
		close(fd);
	}else{
		cout << "p1e_log.raw open failed!"  << endl;
	}
}

void InvLogTransform(int pos, void *userdata)
{
	uint8_t *bufD = (uint8_t *)userdata;
	string wname_D("D:");
	int cvFlag=CV_WINDOW_AUTOSIZE/*WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED*/;
	printf(">>%s:pos=%d, mask_dim=%d\n", __func__, pos, mask_dim);

	//3. perform inverse/exponential log transform
	bufDExp=(uint8_t *)realloc(bufDExp, WIDTH * HEIGHT);
	invLog_transform(bufD, bufDExp, WIDTH * HEIGHT, pos/ILOGT_DIV);
	string wname_DExp = wname_D + ": Inv log transform";
	IplImage * imgDExp= cvDisplay(bufDExp, WIDTH, HEIGHT, WIN_GAP_X*3+SCR_X_OFFSET,
								SCR_Y_OFFSET+SCR_Y_OFFSET, wname_DExp, cvFlag);

	//show histogram of image
	unsigned cdf_tableDExp[(MAX_GREY_LEVEL+1)];
	unsigned hist_tableDExp[(MAX_GREY_LEVEL+1)];
	//get histogram
	hist(hist_tableDExp, (MAX_GREY_LEVEL+1), bufDExp, WIDTH, HEIGHT);
	//draw the histogram
	string wname_DExphist = wname_D + " D InvLog: hist ";
	draw_hist(hist_tableDExp, (MAX_GREY_LEVEL+1), wname_DExphist, WIN_GAP_X*4+SCR_X_OFFSET ,
			  SCR_Y_OFFSET+SCR_Y_OFFSET,cvFlag);

	//show cdf of image
	string wname_DExpcdf = wname_D + " D InvLog: cdf ";
	hist_cdf(hist_tableDExp, cdf_tableDExp, (MAX_GREY_LEVEL+1) , WIDTH * HEIGHT);
	draw_hist(cdf_tableDExp, (MAX_GREY_LEVEL+1), wname_DExpcdf, WIN_GAP_X*5+SCR_X_OFFSET ,
			  SCR_Y_OFFSET+SCR_Y_OFFSET,cvFlag, Scalar( 0, 0 , 255) );

	int fd=-1;
	//create output file
	if( (fd = open("p1e_ilog.raw", O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR) ) != -1 ){
		int s=write(fd, buf_powT, WIDTH * HEIGHT);
		cout << "write:" << s << endl;
		close(fd);
	}else{
		cout << "p1e_ilog.raw open failed!"  << endl;
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
	uint8_t *bufD=NULL;	
	if( (fd = open(raw_win_nameD, O_RDONLY)) != -1 ){
		bufD= (uint8_t *)malloc( WIDTH * HEIGHT);
		ssize_t s=read(fd, bufD, WIDTH * HEIGHT);
		close(fd);
		cout << "request size = " << WIDTH * HEIGHT << ", read size=" << s << endl;
	}else{
		cout << raw_win_nameD << " opening failure!:"<< errno << endl;
		exit(EXIT_FAILURE);
	}

	//show raw image D
	string folder;
	string wname_D, wname_Dp;
	SplitFilename (raw_win_nameD, folder, wname_D);
	wname_Dp = wname_D + ": D";
	IplImage * imgD= cvDisplay(bufD, WIDTH, HEIGHT, WIN_GAP_X*2+SCR_X_OFFSET,0+SCR_Y_OFFSET, 
						wname_Dp, cvFlag);
	
	//show histogram of image D
	unsigned cdf_table[(MAX_GREY_LEVEL+1)];
	unsigned hist_tableD[(MAX_GREY_LEVEL+1)];
	//get histogram of D
	hist(hist_tableD, (MAX_GREY_LEVEL+1), bufD, WIDTH, HEIGHT);
	//draw the histogram
	string wname_Dhist = wname_D + " D: hist ";
	draw_hist(hist_tableD, (MAX_GREY_LEVEL+1), wname_Dhist, WIN_GAP_X+SCR_X_OFFSET ,0+SCR_Y_OFFSET,
		cvFlag);

	//show cdf of image D
	string wname_Dcdf = wname_D + " D: cdf ";
	hist_cdf(hist_tableD, cdf_table, (MAX_GREY_LEVEL+1) , WIDTH * HEIGHT);
	draw_hist(cdf_table, (MAX_GREY_LEVEL+1), wname_Dcdf, 0+SCR_X_OFFSET ,0+SCR_Y_OFFSET,
		cvFlag, Scalar( 0, 0 , 255) );

	////////////////////////////////////////////
	// performing power law transform
	string wname_contrastT("Contrast transform");
	cv::namedWindow(wname_contrastT, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
	cv::createTrackbar("power law step/100", wname_contrastT, &mask_dim, POWER_MAX, PowerLawTransform, 
						bufD);
	cv::createTrackbar("log tranform step/50", wname_contrastT, &mask_dim, LOGT_MAX, LogTransform, 
						bufD);
	cv::createTrackbar("Inv log tranform step/50", wname_contrastT, &mask_dim, ILOGT_MAX, InvLogTransform, 
						bufD);
	moveWindow(wname_contrastT, WIN_GAP_X*3+SCR_X_OFFSET,WIN_GAP_Y+SCR_Y_OFFSET);
	PowerLawTransform(mask_dim, bufD);//init before use interactive
	LogTransform(mask_dim, bufD);//init before use interactive
	InvLogTransform(mask_dim, bufD);//init before use interactive
	////////////////////////////////////////////
	
	cout << "press any key to quit..." << endl;
	waitKey(0);                                          // Wait for a keystroke in the window
	if(imgL){
		cvReleaseImageHeader(&imgL);
		destroyWindow(wname_L);
		free(buf_powT);
	}
	
	if(imgD){
		destroyWindow(wname_Dp);
		cvReleaseImageHeader(&imgD);
		free(bufD);
	}
		
    return 0;
}
