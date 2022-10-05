#ifndef AUTOMATCHER_H
#define AUTOMATCHER_H


#include<iostream>
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;


class Automatcher
{
public:
    Mat left_img;
    Mat right_img;
    int block_y = 3;
    int max_disp = 600;
    int min_disp = 50;
    string matcher;
    Automatcher(Mat left, Mat right, string descriptor);
    //int click_segment(bool automatch, string descriptor);
    void match(Automatcher obj, vector<KeyPoint>& point, bool show_result, vector<int> &); //��python code��ͬ��ֻ����key point
    double kp_size(Automatcher obj, double dist);
    //int** get_candidates(Automatcher obj, int* point);
};

extern void KeyPointsToPoints(vector<KeyPoint> kpts, vector<Point2f>& pts);

extern void PointsToKeyPoints(vector<Point2f> pts, vector<KeyPoint>& kpts);


#endif // AUTOMATCHER_H
