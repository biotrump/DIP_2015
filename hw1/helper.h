/** @brief The helper functions for display and show by
 * opencv gui.
 */
#ifndef _H_OPENCV_HELPER_H
#define	_H_OPENCV_HELPER_H

//http://stackoverflow.com/questions/3071665/getting-a-directory-name-from-a-filename
//split path string to substring folder and file
void SplitFilename (const string& str, string &folder, string &file);

/** @brief Draw the histograms
 * input 
 * hist_table[] : histogram table 
 * h_size : level of histogram table, ie, 256 grey levels
 */
void draw_hist(unsigned *hist_table, int h_size, const string &win_name, 
			   int wx=300, int wy=300, int flag=CV_WINDOW_AUTOSIZE, Scalar color=Scalar( 255, 0, 0)
			   );

/** @brief  output a format text by opencv
 * 
 */
void cvPrintf(IplImage* img, const char *text, CvPoint TextPosition, CvFont Font1,
			  CvScalar Color);

/** @brief show the image by opencv gui
 * buf[] : image buffer
 * width, height : dimension of the buffer
 * x,y : window position to show
 * wname : window name
 * depth : color depth
 * channel : 1 single channel or 3, RGB
 */
IplImage* cvDisplay(uint8_t *buf, int width, int height, int x, int y, string wname,
	int flag=CV_WINDOW_AUTOSIZE, int depth=IPL_DEPTH_8U, int channel=1);
#endif