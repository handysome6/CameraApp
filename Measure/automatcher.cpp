#include "automatcher.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

using namespace std;
using namespace cv::xfeatures2d;
using namespace cv;


void KeyPointsToPoints(vector<KeyPoint> kpts, vector<Point2f>& pts)
{
    for (int i = 0; i < kpts.size(); i++) {
        pts.push_back(kpts[i].pt);
    }
}

vector<int> convertMat2Vector(const Mat& mat)
{
    return (vector<int>)(mat);
}

Automatcher::Automatcher(Mat left, Mat right, string descriptor)
{
    left_img = left;
    right_img = right;
    matcher = descriptor;
}

void Automatcher::match(Automatcher self, vector<KeyPoint>& point, bool show_result, vector<int> &matching_point) {
    cout << "Finding matching point..." << endl;
    Ptr<SIFT> descriptor = SIFT::create();
    /*if (self.matcher == "FREAK")
    {
        Ptr<FREAK> descriptor = FREAK::create();
    }
    else if (self.matcher == "VGG")
    {
        Ptr<VGG> descriptor = VGG::create();
    }*/
    Mat descriptorsL, descriptorsR;
    descriptor->compute(self.left_img, point, descriptorsL);
    vector<Point2f> left_points;
    KeyPointsToPoints(point, left_points);
    //cout << point[0].pt << endl <<(int) left_points[0].x;
    vector<Point2f> candidates;
    for (float i = left_points[0].y - self.block_y/2 ; i < left_points[0].y + self.block_y / 2; ++i)
    {
        for (float j = max(left_points[0].x - self.max_disp, (float) 0);j<left_points[0].x - self.min_disp + 1; ++j)
        {
            candidates.push_back({j, i});
        }
    }
    vector<KeyPoint> candidateK;
    PointsToKeyPoints(candidates, candidateK);
    descriptor->compute(self.right_img, candidateK, descriptorsR);
    int row = descriptorsR.rows * descriptorsR.cols / 128;
    descriptorsR.reshape(0, row);
    //cout<<descriptorsR;
    //cout << "computed the feature 111" << endl;
    //vector<int> disR = (vector<int>)descriptorsR;
    //vector<float[]>disR(descriptorsR.begin<float>(), descriptorsR.end<float>());
    //cout << disR;
    int index = 0;
    double mindist = 99999999;
    double dist_l2;
    for (int i = 0; i < descriptorsR.size().height; ++i)
    {
        dist_l2 = norm(descriptorsL, descriptorsR.rowRange(i,i+1), NORM_L2);
        if (dist_l2 < mindist)
        {
            mindist = dist_l2;
            index = i;
        }
    }
    //cout << "computed the feature 222" << endl;
    //cout << index << endl << mindist;
    //cout << norm(descriptorsL, descriptorsR.rowRange(index, index + 1), NORM_L2);
    matching_point[0] = candidateK[index].pt.x;
    matching_point[1] = candidateK[index].pt.y;
    //int matching_point[2] = { candidateK[index].pt.x, candidateK[index].pt.y };
    //cout<<matching_point[0]<<endl<<matching_point[1]<<endl;
    //************************************
    //return matching_point;
}

double Automatcher::kp_size(Automatcher obj, double dist)
{
    if (obj.matcher == "SIFT")
    {
        return 140000 / (dist + 0.01);
    }
    else if (obj.matcher == "FREAK")
    {
        return 280000 / (dist + 0.01);
    }
    else if (obj.matcher == "VGG")
    {
        return 1400 / (dist + 0.01);
    }
}
