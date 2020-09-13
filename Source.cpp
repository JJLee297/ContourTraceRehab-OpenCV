#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <iostream>
#include <windows.h>
#include "Header.h"
#include <iostream>
#include <time.h>
#include <chrono>
#include <thread>
#include <ctime>
#include <cstdlib>

class CircleGL1 //graphics circle
{
private:
	int xCo, yCo; //coordinates of center
	int radius;
	color fillcolor; //color
	fstyle fillstyle; //fill pattern

public: //sets circle attributes
	void set(int, int, int, color, fstyle);
	void draw(); //draws the circle
	void two(CircleGL1, CircleGL1);
};

void CircleGL1::set(int x, int y, int r, color fc, fstyle fs)
{
	xCo = x;
	yCo = y;
	radius = r;
	fillcolor = fc;
	fillstyle = fs;
}

void CircleGL1::draw() //draws the circle
{
	set_color(fillcolor); //set color
	set_fill_style(fillstyle); //set fill
	draw_circle(xCo, yCo, radius); //draw solid circle
}

bool isNear(int a, int b) {
	if (abs(a - b) <= 5) {
		return true;
	}
	return false; 
}
using namespace std;
using namespace cv; 


int main()
{
	init_graphics();
	
	srand((unsigned int)time(NULL));
	VideoCapture cam(0);
	if (!cam.isOpened()) {
		cout << "Camera not detected... " << endl;
		return -1;
	}
	Mat img;
	Mat img_threshold;
	Mat img_gray;
	Mat img_roi;
	namedWindow("Original_image", CV_WINDOW_AUTOSIZE);
	namedWindow("Gray_image", CV_WINDOW_AUTOSIZE);
	namedWindow("Thresholded_image", CV_WINDOW_AUTOSIZE);
	namedWindow("ROI", CV_WINDOW_AUTOSIZE);
	char a[40];
	int count = 0;
	CircleGL1 c1; //, c2;
	int y = rand() % 10 + 1;
	int r1 = 4;
	//int r2 = rand() % 3 + 3;
	int x1 = rand() % 100 + 20; // 20 < x1 < 119 
	//int x2 = rand() % 100 + 40; // 40 < x2 < 139
	int tripwire = 0;
	int lost = 0; 
	int hits = 0; 
	while (1) {
		bool b = cam.read(img);
		if (!b) {
			cout << "Cannot read camera image..." << endl;
			return -1;
		}
		Rect roi(340, 100, 270, 270);
		img_roi = img(roi);
		cvtColor(img_roi, img_gray, CV_RGB2GRAY);

		GaussianBlur(img_gray, img_gray, Size(5, 5), 0.0, 0);
		threshold(img_gray, img_threshold, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);

		vector<vector<Point>> theContours;
		vector<Vec4i> hierarchy;
		findContours(img_threshold, theContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point());
		if (theContours.size() > 0) {
			int contSize = theContours.size();
			size_t indexOfBiggestContour = -1;
			size_t sizeOfBiggestContour = 0;

			for (size_t i = 0; i < theContours.size(); i++) {
				if (theContours[i].size() > sizeOfBiggestContour) {
					sizeOfBiggestContour = theContours[i].size();
					indexOfBiggestContour = i;
				}
			}
			vector<vector<int>> hull(contSize);
			vector<vector<Point>> hullPoint(contSize);
			vector<vector<Vec4i>> defects(contSize);
			vector<vector<Point>> defectPoint(contSize);
			vector<vector<Point>> contours_poly(contSize);
			Point2f rect_point[4];
			vector<RotatedRect>minRect(contSize);
			vector<Rect> boundRect(contSize);
			for (size_t i = 0; i < contSize; i++) {
				if (contourArea(theContours[i]) > 5000) {
					convexHull(theContours[i], hull[i], 1);
					convexityDefects(theContours[i], hull[i], defects[i]);
					if (indexOfBiggestContour == i) {
						minRect[i] = minAreaRect(theContours[i]);
						for (size_t k = 0; k < hull[i].size(); k++) {
							int ind = hull[i][k];
							hullPoint[i].push_back(theContours[i][ind]);
						}
						count = 0;

						for (size_t k = 0; k < defects[i].size(); k++) {
							srand((unsigned int)time(NULL));
							if (defects[i][k][3] > 14 * 256) {
								//int p_start = defects[i][k][0];
								int p_end = defects[i][k][1];
								int p_far = defects[i][k][2];
								defectPoint[i].push_back(theContours[i][p_far]);
								circle(img_roi, theContours[i][p_end], 5, Scalar(0, 255, 0), 5);
								count++;
								if (isNear((theContours[i][p_end].x / 2), x1)) {
									if (isNear((theContours[i][p_end].y / 9), y)) {
										//strcpy_s(a, "Hit!");
										//cout << "\nHit!\n" << endl; 
										x1 = rand() % 80 + 20;
										r1 = rand() % 4 + 4; 
										y = 0; 
										c1.set(x1, y, r1, cBLUE, X_FILL);
										++hits; 
										Beep(350, 500);
										system("CLS");
										Sleep(100);
									}
								}
								//cout << "Blade Cords: [" << theContours[i][p_end].x << ", " << theContours[i][p_end].y << "]\t";
								//cout << "Ball1 Cords: [" << x1 << ", " << y << "]\n";
								//cout << "Ball2 Cords: [" << x2 << ", " << y << "]\n";
							}

						}
						color arr1[] = { cBLUE, cGREEN, cRED, cWHITE };
						Scalar arr2[] = { Scalar(0, 0, 255), Scalar(0, 255, 255), Scalar(0, 255, 0) };
						int pud = 0;
						if (count >= 1) {
							if (hits - lost >= 4) {
								pud = 2;
							}
							if (hits - lost >= 1 && hits - lost <= 3) {
								pud = 1;
							}
							if (hits - lost < 1) {
								pud = 0; 
							}
							if (hits - lost == -2) {
								system("CLS");
								cout << "\tG\n\t\tA\n\t\t\tM\n\t\t\t\tE\n\t\t\t\t\tO\n\t\t\t\t\t\tV\n\t\t\t\t\t\t\tE\n\t\t\t\t\t\t\t\tR\n!" 
									<< endl; 
								Sleep(4500); 
								return -1; 
							}
							string points = "Points: " + to_string(hits - lost);
							strcpy_s(a, points.c_str());
							tripwire++;
							if (tripwire % 2 == 0 && tripwire != 0) {
								system("CLS");
								y += 3;
								if (y >= 30) {
									lost++;
									y = 0;
								}
								c1.set(x1, y, r1, arr1[hits % 4], X_FILL);
								//c2.set(x2, y, r2, cRED, X_FILL);
								c1.draw();
								//c2.draw();
								Sleep(50);
							}
						}
						else {
							strcpy_s(a, "Show Hand");
							tripwire = 0;
						}
						putText(img, a, Point(30, 70), CV_FONT_HERSHEY_SIMPLEX, 3, arr2[pud], 2, 8, false);
						drawContours(img_threshold, theContours, i, Scalar(255, 255, 0), 2, 8, vector<Vec4i>(), 0, Point());
						drawContours(img_threshold, hullPoint, i, Scalar(255, 255, 0), 1, 8, vector<Vec4i>(), 0, Point());
						drawContours(img_roi, hullPoint, i, Scalar(0, 0, 255), 2, 8, vector<Vec4i>(), 0, Point());
						approxPolyDP(theContours[i], contours_poly[i], 3, false);
						boundRect[i] = boundingRect(contours_poly[i]);
						rectangle(img_roi, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 2, 8, 0);
						minRect[i].points(rect_point);
						for (size_t k = 0; k < 4; k++) {
							line(img_roi, rect_point[k], rect_point[(k + 1) % 4], Scalar(0, 255, 0), 2, 8);
						}

					}
				}

			}

			imshow("Original_image", img);
			imshow("Gray_image", img_gray);
			imshow("Thresholded_image", img_threshold);
			imshow("ROI", img_roi);
			if (waitKey(30) == 27) {
				return -1;
			}

		}

	}
}
