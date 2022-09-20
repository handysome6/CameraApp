#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <fstream>
#include <iostream>
//#include <json/value.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include "click_coord.h"
#include "ruler.h"
#include "rectification.h"

using namespace std;
using namespace cv;
using json = nlohmann::json;

void measure(const String& filename, const char* cameraModel)
{
	
	// Mat imgl = imread("/home/andyls/Project1/Project1/resources/left.jpg");
	// Mat imgr = imread("/home/andyls/Project1/Project1/resources//right.jpg");
	// Mat test;
	// cout<<"111"<<test.empty()<<endl;
	// test = imread("/home/andyls/Project1/Project1/resources/sbs_01.jpg");
	// cout<<"222"<<test.empty()<<endl;	
	Mat imgLR = imread(filename);
	////ClickImage clkimg(img1, "test");
	//Mat fulImage = imread("C:/Users/xtx23/source/repos/Project1/Project1/18_27_43.jpg");
	// 
	// 
	ifstream f(cameraModel);
	json data = json::parse(f);
	// StereoRectify self(data);
	// cout<< self.cm1<<endl;
	// cout<<"after construct"<<endl<<"cm1 :"<<self.cm1<<endl<<"cd1 :"<<self.cd1<<endl<<"cm2 :"<<self.cm2<<endl<<"cd2 :"<<self.cd2<<endl<<"size :"<<self.cameraSize<<endl;
	// cout<<"after construct"<<self.imageSize<<endl;
	Mat recL, recR;
	 //rectifyImage(self, imgLR, recL, recR);
	Mat test2;
	StereoRectify test1 = StereoRectify(data, imgLR, recL, recR);
	// cout<<test2.at<double>(0,0)<<endl;
	// cout<<test1.cm1.at<double>(0,0)<<endl;
	cout<<"finished"<<endl;
	// imshow("left",imgLR);
	// waitKey(1000);
	// imshow("right",recR);
	// waitKey(1000);
	//json data = json::parseObject(f).getJSONObject("cd1");


	//cout<<data["R"];
	float Q[4][4] = {0};
	for(int i = 0; i< 4; i++)
		for(int j = 0; j < 4; j++)
		{
			Q[i][j] = test1.Q.at<double>(i,j);;
			Q[i][j] = (float) Q[i][j] ;
		}
			
	Ruler ruler1 = Ruler(Q, recL, recR);	
	bool flag = ruler1.click_segment(&ruler1, true, "SIFT");
	//cout<<"flag is "<<flag<<endl;
	if (flag == false)
	{
		cout << "Not enough points, program ends";
	}
	else
	{
		float dis = ruler1.measure_segment(&ruler1);
		cout << "Distance of the segment: " << dis << "mm." << endl;
		ruler1.show_result(ruler1);
	}
		// int coord[2][2]={{1,2},{3,4}};
		// int* result[2];
		// for(int i = 0; i< 2; ++i)
		// 	result[i]= coord[i];
		// cout<<result[1][1]<<endl;

	
}