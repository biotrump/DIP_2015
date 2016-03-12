/**  @brief helper functions for opencv to draw and display
 * 
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

/** @brief Draw the histograms
 * input 
 * hist_table[] : histogram table 
 * h_size : level of histogram table, ie, 256 grey levels
 */
void draw_hist(unsigned *hist_table, int h_size, const string &win_name, int wx, int wy,
	int flag, Scalar color)
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
                     color/*Scalar( 255, 0, 0)*/, 2, 8, 0  );
	//string win_name("Histogram: ");
	//win_name = win_name + t_name;
	namedWindow(win_name, flag );
	moveWindow(win_name, wx,wy);
	imshow(win_name, histImage );
}

/** @brief  output a format text by opencv
 * 
 */
void cvPrintf(IplImage* img, const char *text, CvPoint TextPosition, CvFont Font1,
			  CvScalar Color)
{
  //char text[] = "Funny text inside the box";
  //int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
  //double fontScale = 2;

  // center the text
  //Point textOrg((img.cols - textSize.width)/2,
	//			(img.rows + textSize.height)/2);

  //double Scale=2.0;
  //int Thickness=2;
  //CvScalar Color=CV_RGB(255,0,0);
  //CvPoint TextPosition=cvPoint(400,50);
  //CvFont Font1=cvFont(Scale,Thickness);
  // then put the text itself
  cvPutText(img, text, TextPosition, &Font1, Color);
  //cvPutText(img, text, TextPosition, &Font1, CV_RGB(0,255,0));
}

/** @brief show the image by opencv gui
 * buf[] : image buffer
 * width, height : dimension of the buffer
 * x,y : window position to show
 * wname : window name
 * depth : color depth
 * channel : 1 single channel or 3, RGB
 */
IplImage* cvDisplay(uint8_t *buf, int width, int height, int x, int y, string wname,
	int flag, int depth, int channel)
{
	//show image H, the histogram equlization of image D
	IplImage* img = cvCreateImageHeader(cvSize(width, height), depth, channel);
	cvSetData(img, buf, width);
	namedWindow( wname, flag );	// Create a window for display.
	moveWindow( wname, x, y);
	cvShowImage( wname.c_str(), img );     // Show our image inside it.
	return img;
}