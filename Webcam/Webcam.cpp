/*
	My name is Hung Che and this is my code for my hand coordinates recognition
	For info about installing OpenCV, visit: https://medium.com/@subwaymatch/adding-opencv-4-2-0-to-visual-studio-2019-project-in-windows-using-pre-built-binaries-93a851ed6141
	*/
#include <sstream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int H_MIN = 18;
int H_MAX = 80;
int S_MIN = 94;
int S_MAX = 256;
int V_MIN = 165;
int V_MAX = 256;


//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT * FRAME_WIDTH / 1.5;


int main(int argc, char* argv[])
{
	Mat cameraFeed;
	Mat HSV;
	Mat threshold;

	//declare the drawing window
	Mat draw = Mat::zeros(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3);;

	int x = 0, y = 0;

	//capture from webcam
	VideoCapture capture;
	capture.open(0);

	capture.set(CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

	while (1) {
		capture.read(cameraFeed);

		//flip image
		flip(cameraFeed, cameraFeed, 1);

		cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);

		inRange(HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold);

		Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
		Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));
		erode(threshold, threshold, erodeElement);
		erode(threshold, threshold, erodeElement);
		dilate(threshold, threshold, dilateElement);
		dilate(threshold, threshold, dilateElement);

		Mat temp;
		threshold.copyTo(temp);
		vector< vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(temp, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

		double refArea = 0;
		bool objectFound = false;

		if (hierarchy.size() > 0) {
			int numObjects = hierarchy.size();
			//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
			if (numObjects < MAX_NUM_OBJECTS) {
				for (int index = 0; index >= 0; index = hierarchy[index][0]) {
					Moments moment = moments((cv::Mat)contours[index]);
					double area = moment.m00;

					if (area > MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea) {
						x = moment.m10 / area;
						y = moment.m01 / area;
						objectFound = true;
						refArea = area;
						cout << "X: " << x / 2 << " Y: " << y / 2 << "\n";
					}
					else objectFound = false;
				}
				//let user know you found an object
				if (objectFound == true) {
					putText(cameraFeed, "Tracking Object", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);

					/*
					//////////////// UNCOMMENT THIS TO DRAW THE TARGET ON CAMERAFEED ////////////////////////
					circle(cameraFeed, Point(x, y), 20, Scalar(0, 255, 0), 2);
					if (y - 25 > 0)
						line(cameraFeed, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
					else line(cameraFeed, Point(x, y), Point(x, 0), Scalar(0, 255, 0), 2);
					if (y + 25 < FRAME_HEIGHT)
						line(cameraFeed, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
					else line(cameraFeed, Point(x, y), Point(x, FRAME_HEIGHT), Scalar(0, 255, 0), 2);
					if (x - 25 > 0)
						line(cameraFeed, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
					else line(cameraFeed, Point(x, y), Point(0, y), Scalar(0, 255, 0), 2);
					if (x + 25 < FRAME_WIDTH)
						line(cameraFeed, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
					else line(cameraFeed, Point(x, y), Point(FRAME_WIDTH, y), Scalar(0, 255, 0), 2);
					*/
					//drawing method
					circle(draw, Point(x, y), 10, Scalar(255, 255, 255), -1);
				}

			}
			else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
		}

		//imshow("Original Image", cameraFeed); ///////// UNCOMMENT THIS TO SHOW THE WEBCAM FRAMES

		//show drawing windoes
		imshow("Draw", draw);

		//delay 30ms so that screen can refresh.
		//hit "esc" button to stop
		if (waitKey(30) == 27) {
			return 0;
		}
	}
}
