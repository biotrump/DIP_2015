/** @brief WARM-UP: SIMPLE MANIPULATIONS
 * flip an image horizontally and vertically
 * usage : ./build/bin/flip /path/to/sample1.raw
 * output image : 
 *  I_h.raw (flipped horizontally)
 *  I_v.raw (flipped vertically)
 *
 * @author <Thomas Tsai, d04922009@ntu.edu.tw, thomas@life100.cc>
 * 
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#define	WIDTH	(256)
#define	HEIGHT	(256)

using namespace cv;
using namespace std;

#include "dip.h"

const char org_display[]="Original Display";
const char flip_v[]="vertically flipped";
const char flip_h[]="horizontally flipped";

/**
 * flip raw_file
 */
int main( int argc, char** argv )
{
    int fd=-1;
	if( argc < 2) {
		cout <<" Usage: \""<< argv[0] << " raw_file\" to  load a raw file to flip vertically and horizontally." << endl;
		return -1;
    }

	if( (fd = open(argv[1], O_RDONLY)) != -1 ){
		uint8_t *u8= (uint8_t *)malloc( WIDTH * HEIGHT);
		ssize_t s=read(fd, u8, WIDTH * HEIGHT);
		close(fd);
		cout << "request size = " << WIDTH * HEIGHT << ", read size=" << s << endl;

		IplImage* org_img8u = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
		cvSetData(org_img8u, u8, WIDTH);

		//show the Original image
		cvNamedWindow( org_display, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
		cvMoveWindow(org_display, 100,100);
		cvShowImage( org_display, org_img8u );                   // Show our image inside it.

		//flipping
		uint8_t *u8_fh= (uint8_t *)malloc( WIDTH * HEIGHT);
		uint8_t *u8_fv= (uint8_t *)malloc( WIDTH * HEIGHT);
		flip(u8, u8_fh, 'h');//horizontally
		flip(u8, u8_fv, 'v');//vertically
	
		//show horizontally flipped image
		IplImage* fh_img8u = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
		cvSetData(fh_img8u, u8_fh, WIDTH);
		cvNamedWindow( flip_h, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
		cvMoveWindow(flip_h, 400,100);
		cvShowImage( flip_h, fh_img8u );                   // Show our image inside it.

		//show vertically flipped image
		IplImage* fv_img8u = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
		cvSetData(fv_img8u, u8_fv, WIDTH);
		cvNamedWindow( flip_v, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
		cvMoveWindow(flip_v,100, 400);
		cvShowImage( flip_v, fv_img8u );                   // Show our image inside it.

		//write flipped file
		if( (fd = open("I_h.raw", O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR) ) != -1 ){
			s=write(fd, u8_fh, WIDTH * HEIGHT);
			cout << "write:" << s << endl;
			close(fd);
		}else{
			cout << "I_h.raw open failed!"  << endl;
		}
		if((fd = open("I_v.raw", O_CREAT| O_WRONLY, S_IRUSR|S_IWUSR)) != -1 ){
			s=write(fd, u8_fv, WIDTH * HEIGHT);
			cout << "write:" << s << endl;
			close(fd);
		}else{
			cout << "I_v.raw open failed!"  << endl;
		}
		cout << "press any key to quit..." << endl;
		waitKey(0);                                          // Wait for a keystroke in the window

		cvDestroyWindow(org_display);
		cvDestroyWindow(flip_h);
		cvDestroyWindow(flip_v);
		cvReleaseImageHeader(&org_img8u);
		cvReleaseImageHeader(&fh_img8u);
		cvReleaseImageHeader(&fv_img8u);
		
		free(u8);
		free(u8_fh);
		free(u8_fv);
	}else{
		cout << argv[1] << " opening failure!" << endl;
	}
    return 0;
}
