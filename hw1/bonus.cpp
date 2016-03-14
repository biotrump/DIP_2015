/** @brief BONUS #1
 * ./build/bin/bonus -l 200 -n 3 -r /path/to/BONUS_0bonus.raw
 * 
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
#include "helper.h"
//MAX kernel matrix dimension
#define	MAX_DIM			(33)
#define	MAX_LHE_DIM		(255)	//min(WIDTH,HEIGHT)
#define	WIN_GAP_X		(280)
#define	WIN_GAP_Y		(300)

char raw_file[1024];
char problem[100]="2a";
int kernel_dim=3;
int lhe_dim=50;	//dimension of local histogram equalization
const string track_bar_name("dimension");
IplImage* imgBar=NULL;

static void usage(FILE *fp, int argc, char **argv)
{
	fprintf(fp,
		 "Usage: %s [options]\n\n"
		 "Options:\n"
		 "-h | --help        Print this message\n"
		 "-n | --dim n       nxn kernel dim\n"
		 "-l | --local n     local histogram equalization dimxdim\n"
		 "-r | --raw	     The full path of the raw file \n"
		 "-o | --offset	 mxn The screen offset for dual screen\n"
		 "",
		 argv[0]);
}

static const char short_options[] = "hl:n:o:p:r:";

static const struct option
long_options[] = {
	{ "help",   	no_argument,       NULL, 'h' },
	{ "local",		required_argument, NULL, 'l' },
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
			kernel_dim = atoi(optarg);
			printf("kernel_dim=%d\n", kernel_dim);
			if (errno){
				r=-1;
			}
			break;
	case 'l':
			errno = 0;
			lhe_dim = atoi(optarg);
			printf("lhe_dim=%d\n", lhe_dim);
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

IplImage *imgMedian = NULL;
IplImage *imgMDHE=NULL;
IplImage *imgMean=NULL;
IplImage *imgHE = NULL;
void ProcessDim(int pos, void *userdata)
{
	int cvFlag=/*CV_WINDOW_AUTOSIZE*/WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED;
	
	uint8_t *bufRaw = (uint8_t *)userdata;

	printf(">>%s:pos=%d, kernel_dim=%d\n", __func__, pos, kernel_dim);
	if(pos < 3 ) {
		pos=3;
	}
	if((pos%2)==0) {
		pos--;//only odd dim is allowed!
	}
	kernel_dim = pos;
	printf("<<%s:pos=%d, kernel_dim=%d\n", __func__, pos, kernel_dim);

	char dim_str[100];
	sprintf(dim_str, "dim:%d x %d", kernel_dim, kernel_dim);
	cvSetZero(imgBar);
	cvPrintf(imgBar, dim_str, cvPoint(1, 30),
								cvFont(1.0, 1.0), CV_RGB(255,255,255));
	cvShowImage( track_bar_name.c_str(), imgBar);
		
	IplImage* imgP2a = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	//IplImage* imgP2b = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	//string winP2="P";
	uint8_t *buf_worka=NULL, *buf_workb=NULL;
	buf_worka= (uint8_t *)malloc( WIDTH * HEIGHT);
	buf_workb= (uint8_t *)malloc( WIDTH * HEIGHT);

	{
		//1. perform median filter by kernel_dim x kernel_dim
		median(bufRaw, buf_worka, WIDTH, HEIGHT, kernel_dim);
		
		//write output
		int fd=-1;
		char  out_file[100];
		sprintf(out_file, "bonus_median_%dx%d.raw",pos,pos );
		//create output file
		if( (fd = open(out_file, O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR) ) != -1 ){
			int s=write(fd, buf_worka, WIDTH * HEIGHT);
			cout << "write:" << s << endl;
			close(fd);
		}else{
			cout << out_file << " open failed!"  << endl;
		}
	
		string wname_median = "bonus : median";
		
		printf("imgMedian=%p, &imgMedian=%p\n",imgMedian,&imgMedian);
		imgMedian = cvDisplay(&imgMedian, buf_worka, WIDTH, HEIGHT, WIN_GAP_X*3+SCR_X_OFFSET,
									WIN_GAP_Y+SCR_Y_OFFSET, wname_median, cvFlag);

		//show histogram of image
		unsigned hist_tableMD[(MAX_GREY_LEVEL+1)];
		uint8_t histeq_mapMD[(MAX_GREY_LEVEL+1)];
		hist(hist_tableMD, (MAX_GREY_LEVEL+1), buf_worka, WIDTH, HEIGHT);
		string wname_mdHist("bonus median : hist");
		draw_hist(hist_tableMD, (MAX_GREY_LEVEL+1), wname_mdHist, 
				WIN_GAP_X*4+SCR_X_OFFSET, WIN_GAP_Y+SCR_Y_OFFSET, cvFlag);

		//2. perform histogram equlization of Image
		uint8_t *buf_mdhe= (uint8_t *)malloc( WIDTH * HEIGHT);
		unsigned cdf_table[(MAX_GREY_LEVEL+1)];
		hist_eq(buf_worka, buf_mdhe,  WIDTH * HEIGHT, hist_tableMD, cdf_table,
				(MAX_GREY_LEVEL+1),	histeq_mapMD);

		sprintf(out_file,"bonus_median_he_%dx%d.raw",kernel_dim,kernel_dim);
		if( (fd = open(out_file, O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR) ) != -1 ){
			int s=write(fd, buf_mdhe, WIDTH * HEIGHT);
			cout << "write:" << s << endl;
			close(fd);
		}else{
			cout << out_file << " open failed!"  << endl;
		}

		//show cdf of image
		string wname_mdCDF("bonus median : cdf");
		draw_hist(cdf_table, (MAX_GREY_LEVEL+1), wname_mdCDF, WIN_GAP_X*5+SCR_X_OFFSET,
				WIN_GAP_Y*1+SCR_Y_OFFSET);

		//show histogram equlization of the mean image
		string wnameMDHE("median : Hist eq");
		imgMDHE = cvDisplay(&imgMDHE, buf_mdhe, WIDTH, HEIGHT, WIN_GAP_X*3+SCR_X_OFFSET,
								   WIN_GAP_Y*2+SCR_Y_OFFSET, wnameMDHE, cvFlag);

		//PSNR
		char psnr_str[100];
		float psnr = PSNR(bufRaw, buf_mdhe, WIDTH, HEIGHT);
		sprintf(psnr_str, "PSNR of median=%f", psnr);
		cvPrintf(imgBar, psnr_str, cvPoint(1, 50));
		cvShowImage( track_bar_name.c_str(), imgBar);
	}
	
	{
		//perform mean filter by kernel_dim x kernel_dim
		mean(bufRaw, buf_workb, WIDTH, HEIGHT, kernel_dim);
		//write output
		int fd=-1;
		char  out_file[100];
		sprintf(out_file, "bonus_mean_%dx%d.raw",pos,pos );
		//create output file
		if( (fd = open(out_file, O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR) ) != -1 ){
			int s=write(fd, buf_workb, WIDTH * HEIGHT);
			cout << "write:" << s << endl;
			close(fd);
		}else{
			cout << out_file << " open failed!"  << endl;
		}
		//show image after mean filter
		string wname_mean = "bonus : mean";
		imgMean = cvDisplay(&imgMean, buf_workb, WIDTH, HEIGHT, WIN_GAP_X*2+SCR_X_OFFSET,
									WIN_GAP_Y+SCR_Y_OFFSET, wname_mean, cvFlag);

		//show histogram of image
		unsigned hist_tableMn[(MAX_GREY_LEVEL+1)];
		uint8_t histeq_mapMn[(MAX_GREY_LEVEL+1)];
		hist(hist_tableMn, (MAX_GREY_LEVEL+1), buf_workb, WIDTH, HEIGHT);
		string wname_mnHist("bonus mean : hist");
		draw_hist(hist_tableMn, (MAX_GREY_LEVEL+1), wname_mnHist,
				  WIN_GAP_X+SCR_X_OFFSET, WIN_GAP_Y+SCR_Y_OFFSET, cvFlag);

		//histogram equlization of Image
		uint8_t *buf_he= (uint8_t *)malloc( WIDTH * HEIGHT);
		unsigned cdf_table[(MAX_GREY_LEVEL+1)];
		hist_eq(buf_workb, buf_he,  WIDTH * HEIGHT, hist_tableMn, cdf_table,
				(MAX_GREY_LEVEL+1),	histeq_mapMn);

		//show cdf of image
		string  wname_mnCDF("bonus mean : cdf ");
		draw_hist(cdf_table, (MAX_GREY_LEVEL+1), wname_mnCDF, 0+SCR_X_OFFSET,
				WIN_GAP_Y*1+SCR_Y_OFFSET);

		//show histogram equlization of the mean image
		string wnameHE("bonus mean : Hist eq");
		imgHE = cvDisplay(&imgHE, buf_he, WIDTH, HEIGHT, WIN_GAP_X*2+SCR_X_OFFSET,
								   WIN_GAP_Y*2+SCR_Y_OFFSET, wnameHE, cvFlag);

		//PSNR
		char psnr_str[100];
		float psnr = PSNR(bufRaw, buf_he, WIDTH, HEIGHT);
		sprintf(psnr_str, "PSNR of mean=%f", psnr);
		cvPrintf(imgBar, psnr_str, cvPoint(1, 80));
		cvShowImage( track_bar_name.c_str(), imgBar);
	}
}

void ProcessLHE(int pos, void *userdata)
{
	int cvFlag=/*CV_WINDOW_AUTOSIZE*/WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED;
	
	uint8_t *bufRaw = (uint8_t *)userdata;

	printf(">>%s:pos=%d, kernel_dim=%d\n", __func__, pos, kernel_dim);
	if(pos < 3 ) {
		pos=3;
	}
	if((pos%2)==0) {
		pos--;//only odd dim is allowed!
	}
	lhe_dim = pos;
	printf("<<%s:pos=%d, lhe_dim=%d, kernel_dim=%d\n", __func__, pos, lhe_dim, kernel_dim);

	char dim_str[100];
	sprintf(dim_str, "dim:%d x %d", kernel_dim, kernel_dim);
	cvSetZero(imgBar);
	cvPrintf(imgBar, dim_str, cvPoint(1, 30),
								cvFont(1.0, 1.0), CV_RGB(255,255,255));
	cvShowImage( track_bar_name.c_str(), imgBar);
		
	IplImage* imgP2a = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	//IplImage* imgP2b = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	//string winP2="P";
	uint8_t *buf_worka=NULL, *buf_workb=NULL;
	buf_worka= (uint8_t *)malloc( WIDTH * HEIGHT);
	buf_workb= (uint8_t *)malloc( WIDTH * HEIGHT);

	{
		//1. perform median filter by kernel_dim x kernel_dim
		median(bufRaw, buf_worka, WIDTH, HEIGHT, kernel_dim);
		
		//write output
		int fd=-1;
		char  out_file[100];
		sprintf(out_file, "bonus_median_%dx%d.raw",pos,pos );
		//create output file
		if( (fd = open(out_file, O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR) ) != -1 ){
			int s=write(fd, buf_worka, WIDTH * HEIGHT);
			cout << "write:" << s << endl;
			close(fd);
		}else{
			cout << out_file << " open failed!"  << endl;
		}
	
		string wname_median = "bonus : median";
		
		printf("imgMedian=%p, &imgMedian=%p\n",imgMedian,&imgMedian);
		imgMedian = cvDisplay(&imgMedian, buf_worka, WIDTH, HEIGHT, WIN_GAP_X*3+SCR_X_OFFSET,
									WIN_GAP_Y+SCR_Y_OFFSET, wname_median, cvFlag);

		//show histogram of image
		unsigned hist_tableMD[(MAX_GREY_LEVEL+1)];
		uint8_t histeq_mapMD[(MAX_GREY_LEVEL+1)];
		hist(hist_tableMD, (MAX_GREY_LEVEL+1), buf_worka, WIDTH, HEIGHT);
		string wname_mdHist("bonus median : hist");
		draw_hist(hist_tableMD, (MAX_GREY_LEVEL+1), wname_mdHist, 
				WIN_GAP_X*4+SCR_X_OFFSET, WIN_GAP_Y+SCR_Y_OFFSET, cvFlag);

		//2. perform local histogram equlization of Image
		uint8_t *buf_mdhe= (uint8_t *)malloc( WIDTH * HEIGHT);
		unsigned cdf_table[(MAX_GREY_LEVEL+1)];
		//hist_eq(buf_worka, buf_mdhe,  WIDTH * HEIGHT, hist_tableMD, cdf_table,
		//		(MAX_GREY_LEVEL+1),	histeq_mapMD);
		local_hist_eq(buf_worka, buf_mdhe,  WIDTH, HEIGHT,
			MAX_GREY_LEVEL,	lhe_dim);

		sprintf(out_file,"bonus_median_lhe_%dx%d_%d.raw",kernel_dim,kernel_dim, lhe_dim);
		if( (fd = open(out_file, O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR) ) != -1 ){
			int s=write(fd, buf_mdhe, WIDTH * HEIGHT);
			cout << "write:" << s << endl;
			close(fd);
		}else{
			cout << out_file << " open failed!"  << endl;
		}

		//show cdf of image
		string wname_mdCDF("bonus median : cdf");
		hist(hist_tableMD, (MAX_GREY_LEVEL+1), buf_mdhe, WIDTH, HEIGHT);
		hist_cdf(hist_tableMD, cdf_table, (MAX_GREY_LEVEL+1) , WIDTH * HEIGHT);
		draw_hist(cdf_table, (MAX_GREY_LEVEL+1), wname_mdCDF, WIN_GAP_X*5+SCR_X_OFFSET,
				WIN_GAP_Y*1+SCR_Y_OFFSET);

		//show histogram equlization of the mean image
		string wnameMDHE("median : Local Hist eq");
		imgMDHE = cvDisplay(&imgMDHE, buf_mdhe, WIDTH, HEIGHT, WIN_GAP_X*3+SCR_X_OFFSET,
								   WIN_GAP_Y*2+SCR_Y_OFFSET, wnameMDHE, cvFlag);

		//PSNR
		char psnr_str[100];
		float psnr = PSNR(bufRaw, buf_mdhe, WIDTH, HEIGHT);
		sprintf(psnr_str, "PSNR of median, lhe=%f", psnr);
		cvPrintf(imgBar, psnr_str, cvPoint(1, 90));
		cvShowImage( track_bar_name.c_str(), imgBar);
	}
	
	{
		//perform mean filter by kernel_dim x kernel_dim
		mean(bufRaw, buf_workb, WIDTH, HEIGHT, kernel_dim);
		//write output
		int fd=-1;
		char  out_file[100];
		sprintf(out_file, "bonus_mean_%dx%d.raw",pos,pos );
		//create output file
		if( (fd = open(out_file, O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR) ) != -1 ){
			int s=write(fd, buf_workb, WIDTH * HEIGHT);
			cout << "write:" << s << endl;
			close(fd);
		}else{
			cout << out_file << " open failed!"  << endl;
		}
		//show image after mean filter
		string wname_mean = "bonus : mean";
		imgMean = cvDisplay(&imgMean, buf_workb, WIDTH, HEIGHT, WIN_GAP_X*2+SCR_X_OFFSET,
									WIN_GAP_Y+SCR_Y_OFFSET, wname_mean, cvFlag);

		//show histogram of image
		unsigned hist_tableMn[(MAX_GREY_LEVEL+1)];
		uint8_t histeq_mapMn[(MAX_GREY_LEVEL+1)];
		hist(hist_tableMn, (MAX_GREY_LEVEL+1), buf_workb, WIDTH, HEIGHT);
		string wname_mnHist("bonus mean : hist");
		draw_hist(hist_tableMn, (MAX_GREY_LEVEL+1), wname_mnHist,
				  WIN_GAP_X+SCR_X_OFFSET, WIN_GAP_Y+SCR_Y_OFFSET, cvFlag);

		//histogram equlization of Image
		uint8_t *buf_he= (uint8_t *)malloc( WIDTH * HEIGHT);
		unsigned cdf_table[(MAX_GREY_LEVEL+1)];
		//hist_eq(buf_workb, buf_he,  WIDTH * HEIGHT, hist_tableMn, cdf_table,
		//		(MAX_GREY_LEVEL+1),	histeq_mapMn);
		local_hist_eq(buf_workb, buf_he,  WIDTH, HEIGHT,
			MAX_GREY_LEVEL,	lhe_dim);
	
		//show cdf of image
		string  wname_mnCDF("bonus mean : cdf ");
		hist(hist_tableMn, (MAX_GREY_LEVEL+1), buf_he, WIDTH, HEIGHT);
		hist_cdf(hist_tableMn, cdf_table, (MAX_GREY_LEVEL+1) , WIDTH * HEIGHT);
		draw_hist(cdf_table, (MAX_GREY_LEVEL+1), wname_mnCDF, 0+SCR_X_OFFSET,
				WIN_GAP_Y*1+SCR_Y_OFFSET);

		//show histogram equlization of the mean image
		string wnameHE("bonus mean :Local Hist Eq");
		imgHE = cvDisplay(&imgHE, buf_he, WIDTH, HEIGHT, WIN_GAP_X*2+SCR_X_OFFSET,
								   WIN_GAP_Y*2+SCR_Y_OFFSET, wnameHE, cvFlag);

		//PSNR
		char psnr_str[100];
		float psnr = PSNR(bufRaw, buf_he, WIDTH, HEIGHT);
		sprintf(psnr_str, "PSNR of mean, lhe=%f", psnr);
		cvPrintf(imgBar, psnr_str, cvPoint(1, 120));
		cvShowImage( track_bar_name.c_str(), imgBar);
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
	
	//////////////////////////////////////////////////////////////////////////
	//GUI: tracking bar to adjust the dimension of kernel matrix
	imgBar = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3);
	cvSetZero(imgBar);
	cv::namedWindow(track_bar_name, WINDOW_AUTOSIZE);
	cv::createTrackbar("mean/median kernel", track_bar_name, &kernel_dim, MAX_DIM, ProcessDim, 
						bufRaw);
	cv::createTrackbar("local hist eq", track_bar_name, &lhe_dim, MAX_LHE_DIM, ProcessLHE, 
						bufRaw);
	moveWindow(track_bar_name, 1300,0+SCR_Y_OFFSET);
	cvShowImage( track_bar_name.c_str(), imgBar);
	//////////////////////////////////////////////////////////////////////////
	
	//show bonus raw file
	string folder, winRaw;
	SplitFilename (raw_file, folder, winRaw);
	string wname_bonus = winRaw + ":bonus";
	IplImage *imgRaw = cvDisplay(bufRaw, WIDTH, HEIGHT, WIN_GAP_X*2+SCR_X_OFFSET,
								   0+SCR_Y_OFFSET, wname_bonus, cvFlag);

	//Problem solution
	ProcessDim(kernel_dim, bufRaw);

	cout << "press any key to quit..." << endl;
	waitKey(0);                                          // Wait for a keystroke in the window

	if(imgRaw){
		destroyWindow(wname_bonus);
		cvReleaseImageHeader(&imgRaw);
		free(imgRaw);
	}

    return 0;
}
