#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <opencv2/features2d/features2d.hpp>    
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "ruler.h"
#include "click_coord.h"
#include "matcher.h"
#include <vector>

using namespace std;
using namespace cv::xfeatures2d;
using namespace cv;


void PointsToKeyPoints(vector<Point2f>pts, vector<KeyPoint>& kpts)
{
	for (size_t i = 0; i < pts.size(); i++) {
		kpts.push_back(KeyPoint(pts[i], 1.f));
	}
}

Ruler::Ruler(float Qmatrix[4][4], Mat left, Mat right) 
{
	for (int i = 0; i < 4; ++i) {
		Q[i] = Qmatrix[i];
	}
	left_img = left;
	right_img = right;
	cvtColor(left_img, left_gray, COLOR_BGR2GRAY);
	cvtColor(right_img, right_gray, COLOR_BGR2GRAY);

}

//void Ruler::add_endpoints(Ruler* self, vector<KeyPoint> input)
//{
//	self->endpoints.push_back(input[0]);
//}

float Ruler::measure_segment(Ruler* self)
{
	float point1[2];
	float point2[2];
	point1[0] = self->endpoints[0].pt.x;
	point1[1] = self->endpoints[0].pt.y;
	point2[0] = self->endpoints[1].pt.x;
	point2[1] = self->endpoints[1].pt.y;
	//cout<<point1[0]<<endl<<point1[1]<<endl<<point2[0]<<endl<<point2[1]<<endl;
	float world_coord1[3];
	float world_coord2[3];
	get_world_coord_Q(self->Q, self->left_point1, point1, world_coord1);
	get_world_coord_Q(self->Q, self->left_point2, point2, world_coord2);
	cout << "Point1 coords(x,y,z): (" << world_coord1[0] << ", " << world_coord1[1] << ", " << world_coord1[2] << ')' << endl;
	cout << "Point2 coords(x,y,z): (" << world_coord2[0] << ", " << world_coord2[1] << ", " << world_coord2[2] << ')' << endl;
	self->segment_len = sqrt(pow((world_coord1[0] - world_coord2[0]), 2) + pow((world_coord1[1] - world_coord2[1]), 2) + pow((world_coord1[2] - world_coord2[2]), 2));
	//cout<<self->endpoints[1].pt.x<<endl<<self->endpoints[1].pt.y<<endl;
	return self->segment_len;
}

bool Ruler::click_segment(Ruler* self, bool automatch, string matcher)
{
	//vector<KeyPoint> endpoints;
	string window_str;
	if (automatch) 
	{
		window_str = "Please click segment - AutoMatch - ON ";
	}
	else
	{
		window_str = "Please click segment  - AutoMatch - OFF ";
	}
	ClickImage left_clicker = ClickImage(self->left_img, window_str);
	
	// int** img_point_left = {0};
	//int img_point_left[2][2] = {0};
	vector<vector<int>> img_point_left = 
	{
        {1, 2},
        {4, 5}
    };
	left_clicker.click_coord(&left_clicker, img_point_left);
	cout<<"finish choosing"<<endl;
	self->left_point1[0] = float(img_point_left[0][0]);
	self->left_point1[1] = float(img_point_left[0][1]);
	self->left_point2[0] = float(img_point_left[1][0]);
	self->left_point2[1] = float(img_point_left[1][1]);
	// cout<<self->left_point1[0]<<endl;
	// cout<<self->left_point1[1]<<endl;
	// cout<<self->left_point2[0]<<endl;
	// cout<<self->left_point2[1]<<endl;
	cout<<"finish choosing"<<endl;
	//cout << left_clicker.coords[0][0] << endl << left_clicker.coords[0][1] << endl << left_clicker.coords[1][0] << endl << left_clicker.coords[1][1];
	//cout << img_point_left[0][0] << endl << img_point_left[0][1] << endl << img_point_left[1][0] << endl << img_point_left[1][1] << endl;
	if ((img_point_left[0][0] == 0) || (img_point_left[1][0] == 0))
	{
		return false;
	}
	vector<Point2f> points1;
	vector<Point2f> points2;
	points1.push_back(Point2f(img_point_left[0][0],img_point_left[0][1]));
	points2.push_back(Point2f(img_point_left[1][0],img_point_left[1][1]));
	TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 50, 0.00001);
	cornerSubPix(self->left_gray, points1, Size(3, 3), Size(-1, -1), termcrit);
	cornerSubPix(self->left_gray, points2, Size(3, 3), Size(-1, -1), termcrit);
	vector<KeyPoint> left_keypoint1;
	PointsToKeyPoints(points1,left_keypoint1);
	vector<KeyPoint> left_keypoint2;
	PointsToKeyPoints(points2, left_keypoint2);
	//int** img_point_right;
	vector<vector<int>> img_point_right = 
	{
        {0, 0},
        {0, 0}
    };
	//ClickImage user_clicker = ClickImage(self.right_img, "Please reselect the point if needed");
	if (automatch == true) {
		cout<<"start automatching"<<endl;
		ClickImage user_clicker = ClickImage(self->right_img, "Please reselect the point if needed");
		Automatcher matching = Automatcher(self->left_img, self->right_img, matcher);
		//vector<Point2f> img_point_right;	
		vector <int> point1 = {0,0};	
		matching.match(matching, left_keypoint1, true, point1);
		user_clicker.coords[0][0] = point1[0];
		user_clicker.coords[0][1] = point1[1];
		vector <int> point2 = {0,0};	
		matching.match(matching, left_keypoint2, true, point2);
		user_clicker.coords[1][0] = point2[0];
		user_clicker.coords[1][1] = point2[1];
		user_clicker.tempImg = user_clicker.img.clone();
		circle(user_clicker.tempImg, Point(user_clicker.coords[0][0], user_clicker.coords[0][1]), 5, Scalar(0, 0, 255), -1);
		putText(user_clicker.tempImg, (string)"1", Point(user_clicker.coords[0][0], user_clicker.coords[0][1]), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 255), 10, 0, 0);
		circle(user_clicker.tempImg, Point(user_clicker.coords[1][0], user_clicker.coords[1][1]), 5, Scalar(0, 0, 255), -1);
		putText(user_clicker.tempImg, (string)"2", Point(user_clicker.coords[1][0], user_clicker.coords[1][1]), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 255), 10, 0, 0);
		//cout << user_click.coords[0][0]<< endl<<user_click.coords[0][1]<< endl<<user_click.coords[1][0] << endl << user_click.coords[1][1];
		vector<vector<int>>  img_point_right = {
			{0, 0},
			{0, 0}
		};
		user_clicker.click_coord(&user_clicker, img_point_right);
		//cout<<img_point_right[0][0]<<endl<<img_point_right[1][1]<<endl;
		vector<Point2f> pointR;
		vector<KeyPoint> keypointR;
		//cout<<img_point_right[0][0]<<endl<<img_point_right[0][1]<<endl;
		pointR.push_back(Point2f(img_point_right[0][0], img_point_right[0][1]));
		pointR.push_back(Point2f(img_point_right[1][0], img_point_right[1][1]));
		//cout<<img_point_right[0][0]<<endl<<img_point_right[1][1]<<endl;
		//cout<<"pushed"<<endl;
		cornerSubPix(self->left_gray, pointR, Size(3, 3), Size(-1, -1), termcrit);
		//cout<<"subpixed"<<pointR[0].x<<pointR[1].y<<endl;
		PointsToKeyPoints(pointR, keypointR);
		self->endpoints.push_back(keypointR[0]);
		self->endpoints.push_back(keypointR[1]);
		//cout<<keypointR[1].pt.x<<endl<<keypointR[1].pt.y<<endl;
		//cout<<"pushed222"<<endl;
		return true;
	}
	else 
	{
		ClickImage user_clicker = ClickImage(self->right_img, "Please click segment");
		vector<vector<int>>  img_point_right = {
			{0, 0},
			{0, 0}
		};
		user_clicker.click_coord(&user_clicker, img_point_right);
		vector<Point2f> pointR;
		vector<KeyPoint> keypointR;
		//cout<<img_point_right[0][0]<<endl<<img_point_right[0][1]<<endl;
		pointR.push_back(Point2f(img_point_right[0][0], img_point_right[0][1]));
		pointR.push_back(Point2f(img_point_right[1][0], img_point_right[1][1]));
		//cout<<img_point_right[0][0]<<endl<<img_point_right[1][1]<<endl;
		//cout<<"pushed"<<endl;
		cornerSubPix(self->left_gray, pointR, Size(3, 3), Size(-1, -1), termcrit);
		//cout<<"subpixed"<<pointR[0].x<<pointR[1].y<<endl;
		PointsToKeyPoints(pointR, keypointR);
		self->endpoints.push_back(keypointR[0]);
		self->endpoints.push_back(keypointR[1]);
		//cout<<keypointR[1].pt.x<<endl<<keypointR[1].pt.y<<endl;
		//cout<<"pushed222"<<endl;
		return true;
	}

	//int** img_point_right = user_clicker.click_coord(&user_clicker);
	//cout << user_clicker.coords[0][0] << endl << user_clicker.coords[0][1] << endl << user_clicker.coords[1][0] << endl << user_clicker.coords[1][1];
	// 
	// 
	//vector<Point2f> pointsr1;
	//vector<Point2f> pointsr2;
	//pointsr1.push_back(Point2f(img_point_right[0][0], img_point_right[0][1]));
	//pointsr2.push_back(Point2f(img_point_right[1][0], img_point_right[1][1]));
	//cornerSubPix(self->left_gray, pointsr1, Size(3, 3), Size(-1, -1), termcrit);
	//cornerSubPix(self->left_gray, pointsr2, Size(3, 3), Size(-1, -1), termcrit);
	//vector<KeyPoint> right_keypoint1;
	//vector<KeyPoint> right_keypoint2;
	//PointsToKeyPoints(pointsr1, right_keypoint1);
	//PointsToKeyPoints(pointsr2, right_keypoint2);
	////endpoints.push_back(right_keypoint1[0]);
	////endpoints.push_back(right_keypoint2[0]);
	//self->endpoints.push_back(right_keypoint1[0]);
	//self->endpoints.push_back(right_keypoint2[0]);

	// vector<Point2f> pointR;
	// vector<KeyPoint> keypointR;
	// //cout<<img_point_right[0][0]<<endl<<img_point_right[0][1]<<endl;
	// pointR.push_back(Point2f(img_point_right[0][0], img_point_right[0][1]));
	// pointR.push_back(Point2f(img_point_right[1][0], img_point_right[1][1]));
	// cout<<img_point_right[0][0]<<endl<<img_point_right[1][1]<<endl;
	// //cout<<"pushed"<<endl;
	// cornerSubPix(self->left_gray, pointR, Size(3, 3), Size(-1, -1), termcrit);
	// //cout<<"subpixed"<<pointR[0].x<<pointR[1].y<<endl;
	// PointsToKeyPoints(pointR, keypointR);
	// self->endpoints.push_back(keypointR[0]);
	// self->endpoints.push_back(keypointR[1]);
	// cout<<keypointR[1].pt.x<<endl<<keypointR[1].pt.y<<endl;
	// //cout<<"pushed222"<<endl;
	// return true;
}

void Ruler::show_result(Ruler self)
{
	int showRange= 200;
	int width = self.left_img.size().width;
	int height = self.left_img.size().height;
	Rect areaL1(max(int(self.left_point1[0]) - showRange,0), max(int(self.left_point1[1]) - showRange, 0), 
		min(2 * showRange, width - max(int(self.left_point1[0]) - showRange,0)), 
		min(2 * showRange, height - max(int(self.left_point1[1]) - showRange,0)));
	Rect areaL2(max(int(self.left_point2[0]) - showRange,0), max(int(self.left_point2[1]) - showRange, 0), 
		min(2 * showRange, width - max(int(self.left_point2[0]) - showRange,0)), 
		min(2 * showRange, height - max(int(self.left_point2[1]) - showRange,0)));
	Rect areaR1(max(int(self.endpoints[0].pt.x) - showRange,0), max(int(self.endpoints[0].pt.y) - showRange, 0), 
		min(2 * showRange, width - max(int(self.endpoints[0].pt.x) - showRange,0)), 
		min(2 * showRange, height - max(int(self.endpoints[0].pt.y) - showRange,0)));
	Rect areaR2(max(int(self.endpoints[1].pt.x) - showRange,0), max(int(self.endpoints[1].pt.y) - showRange, 0), 
		min(2 * showRange, width - max(int(self.endpoints[1].pt.x) - showRange,0)), 
		min(2 * showRange, height - max(int(self.endpoints[1].pt.y) - showRange,0)));
	// Rect areaL2(self.left_point2[0] - showRange, self.left_point2[1] - showRange, 2 * showRange, 2 * showRange);
	// Rect areaR1(endpoints[0].pt.x - showRange, endpoints[0].pt.y - showRange, 2 * showRange, 2 * showRange);
	// Rect areaR2(endpoints[1].pt.x - showRange, endpoints[1].pt.y - showRange, 2 * showRange, 2 * showRange);
	//cout<<self.endpoints[1].pt.x<<endl<<self.endpoints[1].pt.y<<endl;
	Mat imgL1 = self.left_img.clone()(areaL1);
	Mat imgL2 = self.left_img.clone()(areaL2);
	Mat imgR1 = self.right_img.clone()(areaR1);
	Mat imgR2 = self.right_img.clone()(areaR2);
	Mat tleft = self.left_img.clone();
	line(tleft, Point(int(self.left_point1[0]), int(self.left_point1[1])), Point(int(self.left_point2[0]), int(self.left_point2[1])), 
		Scalar(0, 0, 255), 10, LINE_8);
	circle(tleft, Point(int(self.left_point1[0]), int(self.left_point1[1])), 15, Scalar(0, 0, 255), -1);
	circle(tleft, Point(int(self.left_point2[0]), int(self.left_point2[1])), 15, Scalar(0, 0, 255), -1);
	line(imgL1, Point(0, imgL1.size().height / 2), Point(imgL1.size().width / 2, imgL1.size().height / 2), Scalar(0, 0, 255), 5, LINE_8);
	line(imgL1, Point(imgL1.size().width / 2, imgL1.size().height / 2), Point(imgL1.size().width / 2, 0), Scalar(0, 0, 255), 5, LINE_8);
	line(imgL2, Point(0, imgL2.size().height / 2), Point(imgL2.size().width / 2, imgL2.size().height / 2), Scalar(0, 0, 255), 5, LINE_8);
	line(imgL2, Point(imgL2.size().width / 2, imgL2.size().height / 2), Point(imgL2.size().width / 2, 0), Scalar(0, 0, 255), 5, LINE_8);
	line(imgR1, Point(0, imgR1.size().height / 2), Point(imgR1.size().width / 2, imgR1.size().height / 2), Scalar(0, 0, 255), 5, LINE_8);
	line(imgR1, Point(imgR1.size().width / 2, imgR1.size().height / 2), Point(imgR1.size().width / 2, 0), Scalar(0, 0, 255), 5, LINE_8);
	line(imgR2, Point(0, imgR1.size().height / 2), Point(imgR1.size().width / 2, imgR1.size().height / 2), Scalar(0, 0, 255), 5, LINE_8);
	line(imgR2, Point(imgR1.size().width / 2, imgR1.size().height / 2), Point(imgR1.size().width / 2, 0), Scalar(0, 0, 255), 5, LINE_8);
	
	int imageWidth = self.left_img.size().width;
	int imageHeight = self.left_img.size().height;
	resize(imgL1, imgL1, Size(imageHeight/2,imageHeight/2));
	resize(imgL2, imgL2, Size(imageHeight/2,imageHeight/2));
	resize(imgR1, imgR1, Size(imageHeight/2,imageHeight/2));
	resize(imgR2, imgR2, Size(imageHeight/2,imageHeight/2));
	Mat leftPoints, point1, point2, Result;
	//line(self.left_img, Point(clkImg->width / 2, 0), Point(clkImg->width / 2, clkImg->height), Scalar(0, 0, 255), 10, LINE_8);
	vconcat(imgL1, imgR1, point1);
	vconcat(imgL2, imgR2, point2);
	hconcat(point1,tleft,Result);
	hconcat(Result,point2,Result);
	string showLength ="Length of segment: ";
	//cout<<length;
	//showLength.append(String(format:"%.2f", self.segment_len));
	//self.segment_len.precision(2);
	string length = to_string(self.segment_len);
	length = length.substr(0, length.length()-4);
	showLength.append(length);
	showLength.append("mm.");
	
	putText(Result, showLength, Point(imageHeight/2 + 100,200), FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 255, 255), 10, 0, 0);
	//namedWindow("result", WINDOW_AUTOSIZE);
	namedWindow("result", WINDOW_NORMAL);
	//setWindowProperty("result", WND_PROP_AUTOSIZE, WINDOW_NORMAL);
	resizeWindow("result", Size(imageWidth * 1000 / imageHeight, 1000));
	imshow("result",Result);

	while (1)
	{
		int k = waitKey(100);
		//cout<<111<<endl;
		double a = getWindowProperty("result", WND_PROP_AUTOSIZE);
		if (k == 27)
		{
			destroyWindow("result");
			break;
		}
		if (a < 0)
		{
			// int b = getWindowProperty("result", WND_PROP_VISIBLE);
			// cout<<b<<endl;
			break;
		}
			
		//int a = getWindowProperty("result", 0);
		// if (getWindowProperty("result", 0))
		// {
		// 	// int a = getWindowProperty("result", 0);
		// 	// cout<<a<<endl;
		// 	waitKey(1000);
		// 	break;
		// }
	}

	//destroyWindow("result");
	//waitKey(0);
	// imshow("right",point2);
	 //waitKey(10000);
	// imshow("right",self.left_img);
	// waitKey(1000);
// 	imshow("right",imgR2);
// 	waitKey(1000);
}

void Ruler::get_world_coord_Q(float* Qmatrix[4], float* left, float* right, float* world_coord)
{
	float x = left[0];
	float y = left[1];
	float d = left[0] - right[0];
	float vec[4] = { x,y,d,1.0 };
	int r1 = 4;
	int c1 = 4;
	//int r2 = 4;
	//int c2 = 1;
	float result[4] = {0,0,0,0};

	for (int i = 0; i < r1; ++i)
	{
		for (int k = 0; k < c1; ++k)
		{
			result[i] += Qmatrix[i][k] * vec[k];
		}
	}
	for (int i = 0; i < 3; ++i)
	{
		//cout << result[i]<<endl;
		world_coord[i] = result[i]/ result[3];
		//cout << "world" <<i<<endl<< world_coord[i] << endl;
	}
	//cout << result[3] << endl;
}


