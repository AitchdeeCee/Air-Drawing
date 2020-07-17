/*
	My name is Hung Che and this is my code for my hand coordinates recognition
	For info about installing OpenCV, visit: https://medium.com/@subwaymatch/adding-opencv-4-2-0-to-visual-studio-2019-project-in-windows-using-pre-built-binaries-93a851ed6141
*/
//#include "stdafx.h"

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;
Mat src_gray;
int thresh = 100;
RNG rng(12345);
void thresh_callback(int, void*);

int largest_index = -1;
int largest_area = 0;
Rect bounding_rect;

int main(int argc, char** argv)
{
	VideoCapture cap(0);
	if (cap.isOpened() == false)
	{
		cout << "Cannot open the video camera" << endl;
		cin.get(); //wait for any key press
		return -1;
	}

	double dWidth = cap.get(CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	cout << "Resolution of the video : " << dWidth << " x " << dHeight << endl;

	string window_name = "My Camera Feed";
	namedWindow(window_name); //create a window called "My Camera Feed"
	while (true)
	{
		Mat frame;
		bool bSuccess = cap.read(frame); // read a new frame from video 

		//Breaking the while loop if the frames cannot be captured
		if (bSuccess == false)
		{
			cout << "Video camera is disconnected" << endl;
			cin.get(); //Wait for any key press
			break;  
		}
		flip(frame, frame, 1); //flip the images horizontally

		//show the frame in the created window
		imshow(window_name, frame);

		cvtColor(frame, src_gray, COLOR_BGR2GRAY);
		blur(src_gray, src_gray, Size(3, 3));

		imshow(window_name, frame);
		const int max_thresh = 255;
		createTrackbar("Canny thresh:", window_name, &thresh, max_thresh, thresh_callback);
		thresh_callback(0, 0);
		if (waitKey(10) == 27)
		{
			cout << "Esc key is pressed by user. Stoppig the video" << endl;
			break;
		}
	}

}

void thresh_callback(int, void*)
{
	Mat canny_output;
	Canny(src_gray, canny_output, thresh, thresh * 2);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	for (size_t i = 0; i < contours.size(); i++)
	{
		double a = contourArea(contours[i]);
		if (a > largest_area) {
			largest_area = a;
			largest_index = i;
			bounding_rect = boundingRect(contours[i]);
		}
		Scalar color = Scalar(255, 255, 255);
		drawContours(drawing, contours, largest_index, color, FILLED, 8, hierarchy);
		rectangle(canny_output, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);
	}
	imshow("Contours", drawing);
	Mat drawing1 = Mat::zeros(canny_output.size(), CV_8UC3);
	for (size_t i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(255, 255, 255);
		drawContours(drawing1, contours, i, color, FILLED, 8, hierarchy);
		rectangle(canny_output, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);
	}
	imshow("Contours", drawing1);
}
/*
void thresh_callback(int, void*)
{
	Mat canny_output;
	Canny(src_gray, canny_output, thresh, thresh * 2);
	vector<vector<Point> > contours;
	findContours(canny_output, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
	vector<vector<Point> >hull(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		convexHull(contours[i], hull[i]);
	}
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	for (size_t i = 0; i < contours.size(); i++)
	{
		double a = contourArea(contours[i]);
		if (a > largest_area) {
			largest_area = a;
			largest_index = i;
			bounding_rect = boundingRect(contours[i]);
		}
		Scalar color(255, 255, 255);
		drawContours(drawing, contours, i, color);
		drawContours(drawing, hull, i, color);
	}
	imshow("Hull demo", drawing);
}
*/

/*
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;
Mat hue;
int bins = 25;
void Hist_and_Backproj(int, void*);
int main(int argc, char* argv[])
{
	VideoCapture cap(0);
	if (cap.isOpened() == false)
	{
		cout << "Cannot open the video camera" << endl;
		cin.get(); //wait for any key press
		return -1;
	}
	double dWidth = cap.get(CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	cout << "Resolution of the video : " << dWidth << " x " << dHeight << endl;

	string window_name = "My Camera Feed";
	namedWindow(window_name); //create a window called "My Camera Feed"
	while (true)
	{
		Mat frame;
		bool bSuccess = cap.read(frame); // read a new frame from video 

		//Breaking the while loop if the frames cannot be captured
		if (bSuccess == false)
		{
			cout << "Video camera is disconnected" << endl;
			cin.get(); //Wait for any key press
			break;
		}
		flip(frame, frame, 1); //flip the images horizontally

		//show the frame in the created window
		imshow(window_name, frame);

		Mat hsv;
		cvtColor(frame, hsv, COLOR_BGR2HSV);
		hue.create(hsv.size(), hsv.depth());
		int ch[] = { 0, 0 };
		mixChannels(&hsv, 1, &hue, 1, ch, 1);
		const char* window_image = "Source image";
		namedWindow(window_image);
		createTrackbar("* Hue  bins: ", window_image, &bins, 180, Hist_and_Backproj);
		Hist_and_Backproj(0, 0);
		imshow(window_image, frame);

		if (waitKey(10) == 27)
		{
			cout << "Esc key is pressed by user. Stoppig the video" << endl;
			break;
		}
	}
	return 0;
}
void Hist_and_Backproj(int, void*)
{
	int histSize = MAX(bins, 2);
	float hue_range[] = { 0, 180 };
	const float* ranges = { hue_range };
	Mat hist;
	calcHist(&hue, 1, 0, Mat(), hist, 1, &histSize, &ranges, true, false);
	normalize(hist, hist, 0, 255, NORM_MINMAX, -1, Mat());
	Mat backproj;
	calcBackProject(&hue, 1, 0, hist, backproj, &ranges, 1, true);
	imshow("BackProj", backproj);
	int w = 400, h = 400;
	int bin_w = cvRound((double)w / histSize);
	Mat histImg = Mat::zeros(h, w, CV_8UC3);
	for (int i = 0; i < bins; i++)
	{
		rectangle(histImg, Point(i * bin_w, h), Point((i + 1) * bin_w, h - cvRound(hist.at<float>(i) * h / 255.0)),
			Scalar(0, 0, 255), FILLED);
	}
	imshow("Histogram", histImg);
}
*/