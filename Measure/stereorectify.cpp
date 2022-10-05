#include "stereorectify.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace std;
using namespace cv;
using json = nlohmann::json;


//CameraModel::CameraModel(json camera)
StereoRectify::StereoRectify(json camera, Mat sbsImg, Mat &recL, Mat &recR)
{
    this->imageSize.width = camera["image_size"][0];
    this->imageSize.height = camera["image_size"][1];
    if(camera["is_fisheye"] == false)
        this->isFisheye = false;
        else
        this->isFisheye = true;
    double tcm1[3][3] = {0};
    double tcm2[3][3] = {0};
    double tR[3][3] = {0};
    for (int i = 0; i < 3; ++i)
        for(int j = 0; j< 3; ++j)
            {
                tcm1[i][j]=camera["cm1"][i][j];
                tcm2[i][j]=camera["cm2"][i][j];
                tR[i][j]=camera["R"][i][j];
            }
    double tcd1[1][5] = {0};
    double tcd2[1][5] = {0};
    for(int j = 0; j< 5; ++j)
            {
                tcd1[0][j]=camera["cd1"][0][j];
                tcd2[0][j]=camera["cd2"][0][j];
            }
    double tT[3][1] = {0};
    for(int i = 0; i< 3; ++i)
            tT[i][0]=camera["T"][i][0];
    cm1 = cv::Mat(3, 3, CV_64FC1, tcm1);
    cm2 = cv::Mat(3, 3, CV_64FC1, tcm2);
    cd1 = cv::Mat(1, 5, CV_64FC1, tcd1);
    cd2 = cv::Mat(1, 5, CV_64FC1, tcd2);
    R = cv::Mat(3, 3, CV_64FC1, tR);
    T = cv::Mat(3, 1, CV_64FC1, tT);
    //cout<< "the cm1 is "<<imageSize.width<<endl<<imageSize.height<<endl;
    //cout<< "cm1 "<< cm1<<endl<<"cm2"<<cm2<<endl<<"R "<<R<<endl<<"T"<<T<<endl<<"cd1"<<cd1<<endl<<"cd2"<<cd2<<endl;
    Size newImageSize;
    float newImageRatio =1;
    newImageSize.width = imageSize.width * newImageRatio;
    newImageSize.height = imageSize.height * newImageRatio;
    if (isFisheye)
    {

    }
    else
    {
        float alpha = 0;
        cout << "Vanilla rectifying...";
        Mat R1, R2, P1, P2, Q;
        Rect* validRoI;
        cout<< "cm1 "<< cm1<<endl<<"cm2"<<cm2<<endl<<"R "<<R<<endl<<"T"<<T<<endl<<"cd1"<<cd1<<endl<<"cd2"<<cd2<<endl<<"imageSize"<<newImageSize<<endl<<"cameraSize"<<cameraSize<<endl;
        cout<<R1.type()<<endl<<Q.type()<<endl<<R.type()<<endl<<T.type()<<endl<<cd1.type()<<endl<<cd2.type()<<endl<<cm1.type()<<endl<<cm2.type()<<endl;
        stereoRectify(cm1, cd1, cm2, cd2, imageSize, R, T, R1, R2, P1, P2, Q);
        this->Q = Q;
        //cout<<this->Q<<endl;
        Mat temp1, temp2;
        //cout<< "R1 "<< R<<endl<<"P1"<<P1<<endl;
        initUndistortRectifyMap(cm1, cd1, R1, P1, newImageSize, CV_16SC2, leftMapX, leftMapY);
        initUndistortRectifyMap(cm2, cd2, R2, P2, newImageSize, CV_16SC2, rightMapX, rightMapY);
        //cout<<rightMapX<<endl<<rightMapY<<endl;
        //cout<< "cm1 "<< cm1<<endl<<"cm2"<<cm2<<endl;
        //cout<<"cols "<<leftMapY.cols<<endl<<"rows "<<rightMapY.rows<<endl<<rightMapY.size()<<endl;
        cout << "Calculate map done."<<endl;

        int imgwidth = sbsImg.cols;
        int imgheight = sbsImg.rows;
        Rect areaL(0, 0, imgwidth/2, imgheight);
        Rect areaR(imgwidth/2, 0, imgwidth/2, imgheight);
        //cout<<"left" << areaL<< endl <<"right"<<areaR<<endl;
        // Rect areaL(0, 0, self.width, self.height);
        // Rect areaR(self.width, 0, self.width, self.height);
        Mat imgLeft = sbsImg(areaL);
        Mat imgRight = sbsImg(areaR);
        //cout<<self.leftMapY.at<int>(15,22)<<endl<<self.rightMapY.at<int>(143,123)<<endl;

        remap(imgLeft, recL, leftMapX, leftMapY, INTER_LINEAR, BORDER_CONSTANT);
        remap(imgRight, recR, rightMapX, rightMapY, INTER_LINEAR, BORDER_CONSTANT);
        // imshow("left",recL);
        // waitKey(1000);
        // imshow("right",recR);
        // waitKey(1000);
    }

}

void stereoRectifyVanilla(StereoRectify& self, float alpha, Size newImageSize)
{
    cout << "Vanilla rectifying...";
    // Mat R1 = cv::Mat(3, 3, CV_32FC1);
    // Mat R2 = cv::Mat(3, 3, CV_32FC1);
    // Mat P1 = cv::Mat(3, 4, CV_32FC1);
    // Mat P2 = cv::Mat(3, 4, CV_32FC1);
    // Mat Q = cv::Mat(4, 4, CV_32FC1);
    Mat R1, R2, P1, P2, Q;
    Rect* validRoI;
    //cout<<R1.type()<<endl<<Q.type()<<endl<<self.R.type()<<endl<<self.T.type()<<endl<<self.cd1.type()<<endl<<self.cd2.type()<<endl<<self.cm1.type()<<endl<<self.cm2.type()<<endl;

    stereoRectify(self.cm1, self.cd1, self.cm2, self.cd2, self.cameraSize, self.R, self.T, R1, R2, P1, P2, Q);

    self.Q = Q;
    Mat temp1, temp2;
    initUndistortRectifyMap(self.cm1, self.cd1, R1, P1, newImageSize, CV_16SC2, self.leftMapX, self.leftMapY);
    initUndistortRectifyMap(self.cm2, self.cd2, R2, P2, newImageSize, CV_16SC2, self.rightMapX, self.rightMapY);
    cout<<"cols "<<self.leftMapY.cols<<endl<<"rows "<<self.rightMapY.rows<<endl<<self.rightMapY.size()<<endl;
    cout << "Calculate map done."<<endl;
}

void rectifyCamera(StereoRectify &self, float roiRatio = 0, float newImageRatio = 1)
{
    //float roiRatio = roiRatio;
    Size newImageSize;
    newImageSize.width = self.imageSize.width * newImageRatio;
    newImageSize.height = self.imageSize.height * newImageRatio;

    if (self.isFisheye)
    {

    }
    else
    {
        stereoRectifyVanilla(self, roiRatio, newImageSize);

    }
}



void rectifyImage(StereoRectify &self, Mat sbsImg, Mat &recL, Mat &recR)
{
    cout<<"recitify"<<endl<<"cm1 :"<<self.cm1<<endl<<"cd1 :"<<self.cd1<<endl<<"cm2 :"<<self.cm2<<endl<<"cd2 :"<<self.cd2<<endl<<"size :"<<self.cameraSize<<endl;
    if (self.leftMapX.rows * self.leftMapY.rows * self.rightMapX.cols * self.rightMapY.cols * self.cameraSize.width == 0)
    {
        rectifyCamera(self);
    }
    int imgwidth = sbsImg.cols;
    int imgheight = sbsImg.rows;
    Rect areaL(0, 0, imgwidth/2, imgheight);
    Rect areaR(imgwidth/2, 0, imgwidth/2, imgheight);
    cout<<"left" << areaL<< endl <<"right"<<areaR<<endl;
    // Rect areaL(0, 0, self.width, self.height);
    // Rect areaR(self.width, 0, self.width, self.height);
    Mat imgLeft = sbsImg(areaL);
    Mat imgRight = sbsImg(areaR);
    //cout<<self.leftMapY.at<int>(15,22)<<endl<<self.rightMapY.at<int>(143,123)<<endl;

    remap(imgLeft, recL, self.leftMapX, self.leftMapY, INTER_LINEAR, BORDER_CONSTANT);
    remap(imgRight, recR, self.rightMapX, self.rightMapY, INTER_LINEAR, BORDER_CONSTANT);
// 	imshow("left",recL);
// 	waitKey(3000);
// 	imshow("right",recR);
// 	waitKey(3000);
 }
