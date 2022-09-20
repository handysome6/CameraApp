#include<iostream>
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;


class ClickImage
{
public:
	int mouseX = 0;
	int mouseY = 0;
	int init_x = 0;
	int init_y = 0;
	int init_drag_x = 0;
	int init_drag_y = 0;
	int coords[2][2] = { {0,0},{0,0} };
	int width;
	int height;
	double* Q[4][4];
	Mat img;
	Mat tempImg;
	bool flag = 0;
	bool show_big_image = 0;
	string windowName;
	ClickImage(Mat image, string windowName);
	ClickImage();
	void click_coord(ClickImage* self, vector<vector<int>> &);
};

extern void callBackForScale(int scale, void* userdata);

extern void callBackForPoint(int point, void* userdata);

extern void CallBackFunc(int event, int x, int y, int flags, void* userdata);

extern void click_event(int event, int x, int y, int flags, void* param);
