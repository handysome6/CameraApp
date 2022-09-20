#include<iostream>
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv; 

class Ruler
{
public:
	float* Q[4];
	float left_point1[2];
	float left_point2[2];
	vector<KeyPoint> endpoints;
	double point1_left;
	double segment_len;
	Mat left_img;
	Mat right_img;
	Mat left_gray;
	Mat right_gray;
	Ruler(float Qmatrix[4][4], Mat left, Mat right);
	float measure_segment(Ruler* self);
	bool click_segment(Ruler* self, bool automatch, string matcher);
	//void add_endpoints(Ruler* self, vector<KeyPoint> input);
	void show_result(Ruler self);
	void get_world_coord_Q(float* Qmatrix[4], float* left, float* right, float* result);
}; 

