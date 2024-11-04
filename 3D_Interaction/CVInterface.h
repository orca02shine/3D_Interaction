#pragma once

#include "Loader.h"
#include "MeanShift.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>



class CVInterface {

public:
	static std::string WinName;

	static int IsClicked;
	static cv::Point PrePos;
	static cv::Mat Img;

	static bool Loop();

	static void OnMouse(int event, int x, int y, int flags, void*);

	static void UseInterface();

	static void Clustering(cv::Mat img);



};