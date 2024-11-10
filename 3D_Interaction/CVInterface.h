#pragma once

#include <windows.h>
#include <shobjidl.h>

#include "MeanShift.h"
#include "PatchMatch.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>



class CVInterface {

public:
	static std::string _FileName;
	static std::string _FilePath;

	static std::string WinName;

	static int IsClicked;
	static cv::Point PrePos;
	static cv::Mat Img;
	static cv::Mat Mask_FP;
	static cv::Mat Mask_BP;
	static cv::Mat Mask_Constraint;

	static MeanShift MSProc;

	static bool Loop();

	static void OnMouse(int event, int x, int y, int flags, void*);

	static void UseInterface();

	static void Clustering(cv::Mat img);

	static cv::Mat LoadImg();
	static bool FileOpen(std::string& sSelectedFile, std::string& sFilePath);



};