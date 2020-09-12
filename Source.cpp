#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <iostream>
#include <Windows.h>

using namespace cv;
using namespace std;

int main()
{
    VideoCapture cam(0);
    if(!cam.isOpened()){
        cout<<"Camera not detected... "<< endl;
        return -1;
    }
    Mat img;
    Mat img_threshold;
    Mat img_gray;
    Mat img_roi;
    namedWindow("Original_image",CV_WINDOW_FULLSCREEN);
    namedWindow("Gray_image",CV_WINDOW_FULLSCREEN);
    namedWindow("Thresholded_image", CV_WINDOW_FULLSCREEN);
    namedWindow("ROI",CV_WINDOW_FULLSCREEN);
    char a[40];
    int count = 0;
    for(;;) {
        bool b=cam.read(img);
        if(!b){
            cout<<"Cannot read camera image..."<<endl;
            return -1;
        }
        Rect roi(340,100,270,270);
        img_roi =  img(roi);
        cvtColor(img_roi,img_gray,CV_RGB2GRAY);

        GaussianBlur(img_gray,img_gray,Size(25,25),0.0,0);
        threshold(img_gray,img_threshold,0,255,THRESH_BINARY_INV+THRESH_OTSU);

        vector<vector<Point>> theContours;
        vector<Vec4i> hierarchy;
        findContours(img_threshold,theContours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,Point());
        if(theContours.size()>0){
				int contSize = theContours.size(); 
                size_t indexOfBiggestContour = -1;
				size_t sizeOfBiggestContour = 0;

	            for (size_t i = 0; i < theContours.size(); i++){
		           if(theContours[i].size() > sizeOfBiggestContour){
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
                for(size_t i=0;i< contSize;i++){
                    if(contourArea(theContours[i])>5000){
                        convexHull(theContours[i],hull[i],true);
                        convexityDefects(theContours[i],hull[i],defects[i]);
						if (indexOfBiggestContour == i) {
							minRect[i] = minAreaRect(theContours[i]);
							for (size_t k = 0; k < hull[i].size(); k++) {
								int ind = hull[i][k];
								hullPoint[i].push_back(theContours[i][ind]);
							}
							count = 0;

							for (size_t k = 0; k < defects[i].size(); k++) {
								if (defects[i][k][3] > 13 * 256) {
									//int p_start = defects[i][k][0];
									int p_end = defects[i][k][1];
									int p_far = defects[i][k][2];
									defectPoint[i].push_back(theContours[i][p_far]);
									circle(img_roi, theContours[i][p_end], 5, Scalar(0, 255, 0), 5);
									count++;
									//if (theContours[i][p_end].x == xco && theContours[i][p_end].y == yco) 
								}

							}

							if (count == 1) {
								strcpy_s(a, "1 fingers ");
							}
							else if (count == 2) {
								
								strcpy_s(a, "2 fingers ");
							}
							else if (count == 3) {
								
								strcpy_s(a, "3 fingers ");
							}
							else if (count == 4) {
								
								strcpy_s(a, "4 fingers ");
							}
							else if (count == 5) {
								
								strcpy_s(a, "5 fingers ");
							}
							else {
								
								strcpy_s(a, "0 fingers ");
							}

                            putText(img,a,Point(70,70),CV_FONT_HERSHEY_SIMPLEX,3,Scalar(0,0,255),2,8,false);
                            drawContours(img_threshold, theContours, i,Scalar(255,255,0),2, 8, vector<Vec4i>(), 0, Point() );
                            drawContours(img_threshold, hullPoint, i, Scalar(255,255,0),1, 8, vector<Vec4i>(),0, Point());
                            drawContours(img_roi, hullPoint, i, Scalar(0,0,255),2, 8, vector<Vec4i>(),0, Point() );
                            approxPolyDP(theContours[i],contours_poly[i],3,false);
                            boundRect[i]=boundingRect(contours_poly[i]);
                            rectangle(img_roi,boundRect[i].tl(),boundRect[i].br(),Scalar(255,0,0),2,8,0);
                            minRect[i].points(rect_point);
                            for(size_t k=0;k<4;k++){
                                line(img_roi,rect_point[k],rect_point[(k+1)%4],Scalar(0,255,0),2,8);
                            }

                        }
                    }

                }

            imshow("Original_image",img);
            imshow("Gray_image",img_gray);
            imshow("Thresholded_image",img_threshold);
            imshow("ROI",img_roi);
            if(waitKey(30) == 27){
                  return -1;
            }

        }

    }

     return 0;
}
