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
	for(int i = 0 ; i < MAX_GREY_LEVEL;i++){
		printf("%3d: %u\n", i, hist_table[i]);
	}
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

/** histogram equlization
 * mapping original hist_table to new table histeq_map
 * input :
 * src[] : 8 bit grey image
 * dst[] : histogram equlized destination buffer
 * pixels : total pixels of the 8 bit grey image
 * hist_table[] : histogram
 * h_size : size of histogram, it's usually 256 for 8 bit grey image
 * histeq_map : the histogram equalization mapping table
 * name : name of window to show
 */
void hist_eq(uint8_t *src, uint8_t *dst, int pixels, unsigned *hist_table, int h_size,  
			 uint8_t *histeq_map, string &name)
{
	unsigned cdf_table[MAX_GREY_LEVEL], cdf=0;
	//calculate CDF without normalization from 0 to 1.0, but in 0 to pixels
	for(int i=0;i < h_size ; i++ ){
		cdf += hist_table[i];
		cdf_table[i] = cdf;
		histeq_map[i] = (255 * cdf)  / pixels ;//mapping grey level i to new grey level
		printf("%d->%d\n", i, histeq_map[i]);
	}
	//draw cdf
	string t_name("cdf ");
	draw_hist(cdf_table, h_size, t_name+name/*, int wx=300, int wy=300*/);

	//histogram equlization
	for(int i = 0; i < pixels ; i++)
		dst[i] = histeq_map[src[i]];	//mapping original grey level to new level
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

	//show histogram I
	unsigned hist_tableI[MAX_GREY_LEVEL];
	uint8_t histeq_mapI[MAX_GREY_LEVEL];
	hist(hist_tableI, MAX_GREY_LEVEL, bufI, WIDTH, HEIGHT);
	draw_hist(hist_tableI, MAX_GREY_LEVEL, fileI, 250, 250);

	//histogram equlization II
	uint8_t *bufII= (uint8_t *)malloc( WIDTH * HEIGHT);
	hist_eq(bufI, bufII,  WIDTH * HEIGHT, hist_tableI, MAX_GREY_LEVEL,
			histeq_mapI, fileI);

	//show image II
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
	//show the Original image
	namedWindow( win_nameD, CV_WINDOW_AUTOSIZE );	// Create a window for display.
	moveWindow(win_nameD, 250,250);
	cvShowImage( win_nameD.c_str(), org_imgD );                   // Show our image inside it.

	//show histogram D
	unsigned hist_tableD[MAX_GREY_LEVEL];
	uint8_t histeq_mapH[MAX_GREY_LEVEL];
	hist(hist_tableD, MAX_GREY_LEVEL, bufD, WIDTH, HEIGHT);
	draw_hist(hist_tableD, MAX_GREY_LEVEL, win_nameD, 350,250);

	//histogram equlization D
	bufH= (uint8_t *)malloc( WIDTH * HEIGHT);
	hist_eq(bufD, bufH,  WIDTH * HEIGHT, hist_tableD, MAX_GREY_LEVEL,
			histeq_mapH, win_nameD);

	//show image H
	IplImage* imgH = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
	cvSetData(imgH, bufH, WIDTH);
	string win_nameH =  win_nameD + "Hist eq";
	namedWindow( win_nameH, CV_WINDOW_AUTOSIZE );	// Create a window for display.
	moveWindow( win_nameH, 250,250);
	strcpy(win_hname, win_nameH.c_str());
	cvShowImage( win_hname, imgH );                   // Show our image inside it.
	//show histogram H
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
