#pragma once

#include <windows.h>
#include <shobjidl.h>
#include <string>
#include <iostream>

#include "Meanshift.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\core\core.hpp>


class Loader {

public:

	std::string _FileName;
	std::string _FilePath;


	Loader();
	~Loader();

	bool FileOpen(std::string& sSelectedFile, std::string& sFilePath);

	cv::Mat LoadImage();

};
