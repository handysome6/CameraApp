#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include "click_coord.h"

using namespace std;
using namespace cv;

void callBackForScale(int scale, void* userdata)
{

}

void callBackForPoint(int point, void* userdata)
{
	if (point < 2)
	{
		cout << "Choosing point " << point + 1 << " now" << endl;
	}
}

void click_event(int event, int x, int y, int flags, void* param)
{

	//cout<<"mouse nothing"<<endl;
	/*cout << " clkImg->flag " << clkImg->flag << endl;
	cout << " clkImg->show_big_image " << clkImg->flag << endl;
	cout << clkImg->mouseX << endl << clkImg->mouseY << endl;*/
	ClickImage* clkImg = (ClickImage*)param;
	//cout << "click events" << endl<<"x"<<clkImg->init_x << endl<<'y'<<clkImg->init_y << endl;
	if (clkImg->flag == 0)
	{
		// Mat showimg = clkImg->img.clone();
		// namedWindow(clkImg->windowName, WINDOW_NORMAL);
		// //int width = showimg->size()->width;
		// //int height = showimg->size()->height;
		// resizeWindow(clkImg->windowName, Size(clkImg->width * 500 / clkImg->height, 500));
		// //resize(showimg, showimg, Size(clkImg->width * 500 / clkImg->height, 500));
		// if (clkImg->coords[0][0] != 0)
		// {
		// 	circle(showimg, Point(clkImg->coords[0][0], clkImg->coords[0][1]), 5, Scalar(0, 0, 255), -1);
		// 	putText(showimg, (string)"1", Point(clkImg->coords[0][0], clkImg->coords[0][1]), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 255), 10, 0, 0);
		// }
		// if (clkImg->coords[1][0] != 0)
		// {
		// 	circle(showimg, Point(clkImg->coords[1][0], clkImg->coords[1][1]), 5, Scalar(0, 0, 255), -1);
		// 	putText(showimg, (string)"2", Point(clkImg->coords[1][0], clkImg->coords[1][1]), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 255), 10, 0, 0);
		// }
		// imshow(clkImg->windowName, showimg);
		namedWindow(clkImg->windowName, WINDOW_NORMAL);
		resizeWindow(clkImg->windowName, Size(clkImg->width * 500 / clkImg->height, 500));
		if(clkImg->tempImg.empty())
		{
			imshow(clkImg->windowName, clkImg->img);
		}
		else{
			imshow(clkImg->windowName, clkImg->tempImg);
		}
		//imshow(clkImg->windowName, clkImg->img);
	}

	if (event == EVENT_LBUTTONDOWN)
	{
		//cout<<"mouse down"<<endl;
		//cout << "mouse down" << clkImg->windowName << endl << "x" << clkImg->init_x << endl << 'y' << clkImg->init_y << endl << "big " << clkImg->show_big_image << endl << "flag " << clkImg->flag;
		if (clkImg->flag == 0)
		{
			//clkImg->flag = 1;
			clkImg->init_x = x;
			clkImg->init_y = y;
		}
		else if (clkImg->flag == 1)
		{
			clkImg->init_drag_x = x;
			clkImg->init_drag_y = y;
		}
	}

	if ((event == EVENT_MOUSEMOVE) && (flags & EVENT_LBUTTONDOWN))
	{
		//cout<<"mouse moving"<<endl;
		int show_width = clkImg->width * 500 / clkImg->height;
		
		//int width = showImg->size()->width;
		//int height = showImg->size()->height;
	
		clkImg->flag = 1;
		clkImg->show_big_image = 0;
		int scale = max(getTrackbarPos("scale", clkImg->windowName), 1);
		int x1 = clkImg->init_x - (x - clkImg->init_drag_x) / scale;
		int y1 = clkImg->init_y - (y - clkImg->init_drag_y) / scale;
		x1 = max(1, min(x1, clkImg->width));
		y1 = max(1, min(y1, clkImg->height));
		int x_range = clkImg->width / scale / 2;
		int y_range = clkImg->height / scale / 2;
		//cout << "range" << x_range << endl << y_range << endl;
		x1 = min(max(x_range, x1), clkImg->width - x_range);
		y1 = min(max(y_range, y1), clkImg->height - y_range);
		//cout << x1 << endl<< y1 << endl;
		//x1 = x1 * clkImg->height / 500;
		//y1 = y1 * clkImg->height / 500;
		Rect area(x1 - x_range, y1 - y_range, 2 * x_range, 2 * y_range);
		//Rect area(x1 + 5*x_range, y1 + 5*y_range, 2 * x_range, 2 * y_range);
		Mat cut_img = clkImg->img(area);
		//imshow("clkImg->windowName", clkImg->img);

		resize(cut_img, cut_img, Size(clkImg->width, clkImg->height));
		//line(cut_img, Point(show_width / 2, 0), Point(show_width / 2, 500), Scalar(0, 0, 255), 10, LINE_8);
		//line(cut_img, Point(0, 250), Point(show_width, 250), Scalar(0, 0, 255), 10, LINE_8);

		line(cut_img, Point(clkImg->width / 2, 0), Point(clkImg->width / 2, clkImg->height), Scalar(0, 0, 255), 10, LINE_8);
		line(cut_img, Point(0, clkImg->height / 2), Point(clkImg->width, clkImg->height / 2), Scalar(0, 0, 255), 10, LINE_8);

		string point_ID = to_string(getTrackbarPos("pick point", clkImg->windowName) + 1);
		putText(cut_img, point_ID, Point(clkImg->coords[1][0], clkImg->coords[1][1]), FONT_HERSHEY_SIMPLEX, 5, cv::Scalar(0, 255, 255), 10, 0, 0);

		namedWindow(clkImg->windowName, WINDOW_NORMAL);
		resizeWindow(clkImg->windowName, Size(show_width, 500));
		imshow(clkImg->windowName, cut_img);
		//cout << x<<endl<<y<<endl<<clkImg->init_x << endl << clkImg->init_y << endl << clkImg->init_drag_x << endl << clkImg->init_drag_y << endl;
	}

	if (event == EVENT_LBUTTONUP)
	{
		//cout<<"mouse up"<<endl;
		//cout << "mouse up" << clkImg->windowName << endl << "initx" << clkImg->init_x << endl << "'inity" << clkImg->init_y << endl << "dragx " << clkImg->init_drag_x <<endl<< "dragy" << clkImg->init_drag_y<<endl;
		if (clkImg->flag == 0)
		{
			clkImg->show_big_image = 1;
			clkImg->flag = 1;
		}
		else {
			if (clkImg->show_big_image == 1)
			{
				clkImg->show_big_image = 0;
			}
			clkImg->flag = 0;
			int scale = max(getTrackbarPos("scale", clkImg->windowName), 1);
			int x_range = clkImg->width / scale / 2;
			int y_range = clkImg->height / scale / 2;
			clkImg->mouseX = clkImg->init_x - (x - clkImg->init_drag_x) / scale;
			clkImg->mouseY = clkImg->init_y - (y - clkImg->init_drag_y) / scale;
			clkImg->mouseX = min(max(x_range, clkImg->mouseX), clkImg->width - x_range);
			clkImg->mouseY = min(max(y_range, clkImg->mouseY), clkImg->width - y_range);
			//cout << "mouseXY" << clkImg->mouseX << endl << clkImg->mouseY << endl;
			int point_ID = getTrackbarPos("pick point", clkImg->windowName);
			if ((clkImg->mouseX != clkImg->coords[0][0]) && (clkImg->mouseX != clkImg->coords[1][0]))
			{
				clkImg->coords[point_ID][0] = clkImg->mouseX;
				clkImg->coords[point_ID][1] = clkImg->mouseY;
			}
			// Mat showImg = clkImg->img.clone();
			// namedWindow(clkImg->windowName, WINDOW_NORMAL);
			// resizeWindow(clkImg->windowName, Size(clkImg->width * 500 / clkImg->height, 500));
			// //resize(showImg, showImg, Size(clkImg->width * 500 / clkImg->height, 500));
			// if (clkImg->coords[0][0] != 0)
			// {
			// 	circle(showImg, Point(clkImg->coords[0][0], clkImg->coords[0][1]), 5, Scalar(0, 0, 255), -1);
			// 	putText(showImg, (string)"1", Point(clkImg->coords[0][0], clkImg->coords[0][1]), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 255), 10, 0, 0);
			// }
			// if (clkImg->coords[1][0] != 0)
			// {
			// 	circle(showImg, Point(clkImg->coords[1][0], clkImg->coords[1][1]), 5, Scalar(0, 0, 255), -1);
			// 	putText(showImg, (string)"2", Point(clkImg->coords[1][0], clkImg->coords[1][1]), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 255), 10, 0, 0);
			// }
			// imshow(clkImg->windowName, showImg);
			clkImg->tempImg = clkImg->img.clone();
			namedWindow(clkImg->windowName, WINDOW_NORMAL);
			resizeWindow(clkImg->windowName, Size(clkImg->width * 500 / clkImg->height, 500));
			//resize(showImg, showImg, Size(clkImg->width * 500 / clkImg->height, 500));
			if (clkImg->coords[0][0] != 0)
			{
				circle(clkImg->tempImg, Point(clkImg->coords[0][0], clkImg->coords[0][1]), 5, Scalar(0, 0, 255), -1);
				putText(clkImg->tempImg, (string)"1", Point(clkImg->coords[0][0], clkImg->coords[0][1]), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 255), 10, 0, 0);
			}
			if (clkImg->coords[1][0] != 0)
			{
				circle(clkImg->tempImg, Point(clkImg->coords[1][0], clkImg->coords[1][1]), 5, Scalar(0, 0, 255), -1);
				putText(clkImg->tempImg, (string)"2", Point(clkImg->coords[1][0], clkImg->coords[1][1]), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 255), 10, 0, 0);
			}
			imshow(clkImg->windowName, clkImg->tempImg);
		}
	}
}

ClickImage::ClickImage(Mat image, string windowName)
{
	img = image;
	this->windowName = windowName;
	width = img.size().width;
	height = img.size().height;
}

ClickImage::ClickImage()
{
	width = 0;
	height = 0;
}


void ClickImage::click_coord(ClickImage* input, vector<vector<int>> &result)
{
	ClickImage clkImg = *input;
	//while (1) {
	Mat showImg = clkImg.img.clone();
	namedWindow(clkImg.windowName, WINDOW_NORMAL);
	/*int width = showImg.size().width;
	int height = showImg.size().height;*/
	resizeWindow(clkImg.windowName, Size(clkImg.width * 500 / clkImg.height, 500));
	//resize(showImg, showImg, Size(clkImg.width * 500 / clkImg.height, 500));
	int scale = 16;
	createTrackbar("scale", clkImg.windowName, &scale, 50, callBackForScale);
	int point = 0;
	createTrackbar("pick point", clkImg.windowName, &point, 2, callBackForPoint);
	setMouseCallback(clkImg.windowName, click_event, &clkImg);
	while (1)
	{
		//imshow(clkImg.windowName, showImg);
		int k = waitKey(20) & 0xFF;
		
		if (clkImg.show_big_image == 1)
		{
			int x = clkImg.init_x;
			int y = clkImg.init_y;
			int scale = max(getTrackbarPos("scale", clkImg.windowName), 1);
			int x_range = clkImg.width / scale / 2;
			int y_range = clkImg.height / scale / 2;
			x = min(max(x_range, x), clkImg.width - x_range);
			y = min(max(y_range, y), clkImg.height - y_range);
			//x = x * clkImg.height / 500;
			//y = y * clkImg.height / 500;
			//cout << "showx" << x << "showy" << y << endl;
			Rect area(x - x_range, y - y_range, 2 * x_range, 2 * y_range);
			Mat cut_img = clkImg.img(area);

			int show_width = clkImg.width * 500 / clkImg.height;
			
			resize(cut_img, cut_img, Size(clkImg.width, clkImg.height));
			line(cut_img, Point(clkImg.width / 2, 0), Point(clkImg.width / 2, clkImg.height), Scalar(0, 0, 255), 10, LINE_8);
			line(cut_img, Point(0, clkImg.height / 2), Point(clkImg.width, clkImg.height / 2), Scalar(0, 0, 255), 10, LINE_8);
			string point_ID = to_string(getTrackbarPos("pick point", clkImg.windowName) + 1);
			putText(cut_img, point_ID, Point(clkImg.coords[1][0], clkImg.coords[1][1]), FONT_HERSHEY_SIMPLEX, 5, cv::Scalar(0, 255, 255), 10, 0, 0);
			//clkImg.img = cut_img;
			namedWindow(clkImg.windowName, WINDOW_NORMAL);
			resizeWindow(clkImg.windowName, Size(clkImg.width * 500 / clkImg.height, 500));
			showImg = cut_img;
			imshow(clkImg.windowName, showImg);
			//imshow("big", cut_img);

		}
		else if (clkImg.flag == 0)
		{
			// Mat showImg = clkImg.img.clone();
			// namedWindow(clkImg.windowName, WINDOW_NORMAL);
			// //int width = showImg.size().width;
			// //int height = showImg.size().height;
			// resizeWindow(clkImg.windowName, Size(clkImg.width * 500 / clkImg.height, 500));
			// //resize(showImg, showImg, Size(clkImg.width * 500 / clkImg.height, 500));
			// if (clkImg.coords[0][0] != 0)
			// {
			// 	circle(showImg, Point(clkImg.coords[0][0], clkImg.coords[0][1]), 5, Scalar(0, 0, 255), -1);
			// 	putText(showImg, (string)"1", Point(clkImg.coords[0][0], clkImg.coords[0][1]), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 255), 10, 0, 0);
			// }
			// if (clkImg.coords[1][0] != 0)
			// {
			// 	circle(showImg, Point(clkImg.coords[1][0], clkImg.coords[1][1]), 5, Scalar(0, 0, 255), -1);
			// 	putText(showImg, (string)"2", Point(clkImg.coords[1][0], clkImg.coords[1][1]), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 255), 10, 0, 0);
			// }
			// imshow(clkImg.windowName, showImg);
			//******************************************************************************************
			namedWindow(clkImg.windowName, WINDOW_NORMAL);
			resizeWindow(clkImg.windowName, Size(clkImg.width * 500 / clkImg.height, 500));
			if(clkImg.tempImg.empty())
			{
				imshow(clkImg.windowName, clkImg.img);
			}
			else{
				imshow(clkImg.windowName, clkImg.tempImg);
			}
			// Mat showImg = clkImg.img.clone();
			// namedWindow(clkImg.windowName, WINDOW_NORMAL);
			// //int width = showImg.size().width;
			// //int height = showImg.size().height;
			// resizeWindow(clkImg.windowName, Size(clkImg.width * 500 / clkImg.height, 500));
			// //resize(showImg, showImg, Size(clkImg.width * 500 / clkImg.height, 500));
			// if (clkImg.coords[0][0] != 0)
			// {
			// 	circle(showImg, Point(clkImg.coords[0][0], clkImg.coords[0][1]), 5, Scalar(0, 0, 255), -1);
			// 	putText(showImg, (string)"1", Point(clkImg.coords[0][0], clkImg.coords[0][1]), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 255), 10, 0, 0);
			// }
			// if (clkImg.coords[1][0] != 0)
			// {
			// 	circle(showImg, Point(clkImg.coords[1][0], clkImg.coords[1][1]), 5, Scalar(0, 0, 255), -1);
			// 	putText(showImg, (string)"2", Point(clkImg.coords[1][0], clkImg.coords[1][1]), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 255), 10, 0, 0);
			// }
			// imshow(clkImg.windowName, showImg);

			//imshow(clkImg.windowName, clkImg.img);
		}
		int point = getTrackbarPos("pick point", clkImg.windowName);
		if (point == 2)
		{
			break;
		}
	}

	*input = clkImg;
	destroyWindow(clkImg.windowName);
	for (int i = 0; i < 2; ++i)
		for(int j = 0; j < 2; ++j)
			// cout << clkImg.coords[i][j]<<endl;
	 		result[i][j] = clkImg.coords[i][j];
	//cout<<"11"<<endl<<clkImg.coords[1][1]<<endl;
	//cout<<"22"<<endl;
	//cout<<result[1][1]<<endl;
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_RBUTTONDOWN)
	{
		cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MBUTTONDOWN)
	{
		cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MOUSEMOVE)
	{
		cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;

	}
}

//int Clickimage(int argc, char** argv)
//{
//	// Read image from file 
//	Mat img1 = imread("bug.jpg");
//	ClickImage clkImg(img1, "test");
//	//clkImg = input;
//
//	//cout << clkImg.width << endl << clkImg.height;
//	int** point = clkImg.click_coord(&clkImg);
//	cout << "result" << point[0][0] << point[0][1] << endl << point[1][0] << point[1][1];
//
//	return 0;
//}