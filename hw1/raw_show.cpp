/** @brief show a raw file
 * @author <Thomas Tsai, thomas@life100.cc>
 * ./build/bin/raw_show path/to/file.raw
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
#include "helper.h"

const char org_display[]="raw Display";

int main( int argc, char** argv )
{
    int fd=-1;
	if( argc < 2) {
		cout <<" Usage: \""<< argv[0] << " raw_file\" to  show a raw file" << endl;
		return -1;
    }

	if( (fd = open(argv[1], O_RDONLY)) != -1 ){
		uint8_t *u8= (uint8_t *)malloc( WIDTH * HEIGHT);
		ssize_t s=read(fd, u8, WIDTH * HEIGHT);
		close(fd);
		cout << "request size = " << WIDTH * HEIGHT << ", read size=" << s << endl;

		string folder, wname;
		SplitFilename (argv[1], folder, wname);

		IplImage* org_img8u = cvCreateImageHeader(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 1);
		cvSetData(org_img8u, u8, WIDTH);

		//show the Original image
		namedWindow(wname, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );	// Create a window for display.
		moveWindow(wname, 100,100);
		cvShowImage(wname.c_str(), org_img8u );                   // Show our image inside it.

		cout << "press any key to quit..." << endl;
		waitKey(0);                                          // Wait for a keystroke in the window

		destroyWindow(wname);
		cvReleaseImageHeader(&org_img8u);
		
		free(u8);
	}else{
		cout << argv[1] << " opening failure!" << endl;
	}
    return 0;
}
