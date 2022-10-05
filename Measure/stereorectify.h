#ifndef STEREORECTIFY_H
#define STEREORECTIFY_H


#include<iostream>
#include "opencv2/highgui/highgui.hpp"
#include <nlohmann/json.hpp>

using namespace std;
using namespace cv;
using json = nlohmann::json;




class StereoRectify
{
public:
    StereoRectify(json camera);
    StereoRectify(json , Mat sbsImg, Mat &recL, Mat &recR);
    Size imageSize;
    bool isFisheye;
    Mat cm1 = cv::Mat(3, 3, CV_64FC1);
    Mat cm2 = cv::Mat(3, 3, CV_64FC1);
    Mat cd1 = cv::Mat(1, 5, CV_64FC1);
    Mat cd2 = cv::Mat(1, 5, CV_64FC1);
    Mat R = cv::Mat(3, 3, CV_64FC1);
    Mat T = cv::Mat(3, 1, CV_64FC1);

    //CameraModel *camera;
    int width, height;
    Size cameraSize;
    Mat leftMapX, leftMapY, rightMapX, rightMapY, Q;

};

extern void rectifyImage(StereoRectify &self, Mat sbsImg, Mat &recL, Mat &recR);

#endif // STEREORECTIFY_H
