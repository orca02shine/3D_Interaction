#pragma once

#include <windows.h>
#include <shobjidl.h>
#include <string>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\core\core.hpp>


class Loader {

public:

	static std::string _FileName;
	static std::string _FilePath;


	//Loader();
	//~Loader();

	static bool FileOpen(std::string& sSelectedFile, std::string& sFilePath);

	static cv::Mat LoadImg();

};
