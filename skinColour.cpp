//
// Utilises the openCV library. Please see license http://opencv.org/license.html
//

#include "stdafx.h"
#include "cv.h" 
#include "highgui.h" 

IplImage *frame, *frame2, *out, *bg, *output;
int pixel_step, channels, pixel_step_out, channels_out, y=0, x=0, c=0, checkcxt;
uchar *data, *data_out;
double Result , Result2;
int x_out, y_out;								//Define Variables
CvSeq* first_contour, *contours2, *hull, *defect;
CvMemStorage* storage = cvCreateMemStorage(), *dftStorage=0, *minStorage=0;	
CvRect rect;
CvBox2D box;
CvCapture *webcam, *webcam2;

int main() {

	webcam=cvCaptureFromCAM(0);				//Setup Capture Variables
    	webcam2=cvCaptureFromCAM(1);

	cvNamedWindow("Webcam", CV_WINDOW_AUTOSIZE);		//Setup output Windows
	cvNamedWindow("Webcam2", CV_WINDOW_AUTOSIZE);

	while(c != 27) {
		frame=cvQueryFrame(webcam);			//Grab Frames
		frame2=cvQueryFrame(webcam2);
		cvShowImage("Webcam",frame);			//Put Frame in Window
		out = cvCreateImage( cvGetSize(frame), 8, 1 );	//Create oputput frame varibale
		pixel_step = frame->widthStep;
		channels = frame->nChannels;			//Setup frame parameters
		pixel_step_out = out->widthStep;
		channels_out = out->nChannels;
		data = (uchar *)frame->imageData;
		data_out = (uchar *)out->imageData;

		//----Scan Frame for Skin Coloured Pixels and saturate them. Otherwise Zero them---//
		for(y=0; y<(frame->height); y++) {
			for(x=0 ; x<(frame->width); x++) {
				if(((data[y*pixel_step+x*channels+2]) > (10+data[y*pixel_step+x*channels])) && ((data[y*pixel_step+x*channels+2]) > (10+data[y*pixel_step+x*channels+1]))) {
					data_out[y*pixel_step_out+x*channels_out]=255;
				}
				else {
					data_out[y*pixel_step_out+x*channels_out]=0;
				}
			}
		}
		//--------------------------------------------------------------------------------//

		cvShowImage("Webcam2",out);			//Output frame to Window

		bg = cvCreateImage(cvGetSize(out), 8, 3);
		
		//---Find Contours in the image----//
		first_contour = NULL;
		cvFindContours(out, storage, &first_contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
		//---------------------------------//
		
		contours2 = NULL;
        	Result = 0;
		Result2 = 0;


		while(first_contour) { 
           		Result = fabs(cvContourArea(first_contour, CV_WHOLE_SEQ));		//Find the largest contour in the image
		   	if(Result>Result2) { 
                 		Result2 = Result;
		 		contours2 = first_contour; 
		   	} 
           		first_contour  =  first_contour->h_next; 
		}

		//----Draw a rectangle around the largest contour found----------//
		if( contours2 ) { 
           		rect = cvBoundingRect(contours2, 0); 
           		cvRectangle( bg, cvPoint(rect.x, rect.y + rect.height), cvPoint(rect.x + rect.width, rect.y), CV_RGB(10, 100, 255), 1, 8, 0 ); 
		   	checkcxt = cvCheckContourConvexity( contours2 ); 
           		hull = cvConvexHull2( contours2, 0, CV_CLOCKWISE, 0 ); 
           		defect = cvConvexityDefects( contours2, hull, dftStorage ); 
		   	box = cvMinAreaRect2( contours2, minStorage );
		 }
		 //--------------------------------------------------------------//

		 //---Use rectangle size to find a point where the hand is and output X and Y to console---//
		 x_out=abs(rect.x)+(abs(rect.width)/2);
		 y_out=abs(frame->height)-abs(rect.y)+(abs(rect.height)/2);
		 printf("X: %d , Y: %d \n", x_out, y_out);
		 cvDrawContours(bg, contours2,  CV_RGB( 255,255,255), CV_RGB( 0, 0, 0), 1, -1, 8, cvPoint(0,0));
		 output=cvCreateImage(cvGetSize(frame), 8, 1 );
	     	 cvNamedWindow("bg",CV_WINDOW_AUTOSIZE);
	     	 cvShowImage("bg",bg);
		 //----------------------------------------------------------------------------------------//

	 	 c = cvWaitKey(10);				//Check to see if user presses 'esc'
	}
    	cvDestroyAllWindows();				//Close Windows
	return 0;					//End Algorithm
}
