#pragma once

#include <windows.h>
#include <shobjidl.h>

#include "MeanShift.h"
#include "PatchMatch.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include<vector>
#include <iostream>



class CVInterface {

public:
	static std::string _FileName;
	static std::string _FilePath;

	static std::string WinName;

	static int IsClicked;
	static int TargetSize;
	static cv::Point PrePos;
	static cv::Point ClickedPos;
	static std::vector<cv::Point> BoundaryPoint;
	static std::vector<cv::Point> Corners;//0,leftup,1,rightup,2,leftbottom,3,rightbottom
	static cv::Mat Img_Clone;
	static cv::Mat Img;
	static cv::Mat Img_Roi;
	static cv::Mat Mask_FP;
	static cv::Mat Mask_BP;
	static cv::Mat Mask_Patch;
	static cv::Mat Mask_Constraint;

	static cv::Mat Mask_GC;
	static cv::Mat BgModel;
	static cv::Mat FgModel;

	static cv::Mat Result_Back;
	static cv::Mat Result_Fore;
	static cv::Mat Result_ForeFlip;
	static cv::Mat Result_TexAtlas;

	static std::vector< std::vector<cv::Point>> Contours;
	static std::vector<cv::Vec4i> Hierarchy;

	static MeanShift MSProc;

	static std::pair<int, int> TexSize;

	static bool Loop();

	static void OnMouse(int event, int x, int y, int flags, void*);

	static void UseInterface();

	static void GrabCut(cv::Mat src, cv::Mat &fore,cv::Mat &back);

	static void Clustering(cv::Mat img);

	static cv::Mat LoadImg();
	static bool FileOpen(std::string& sSelectedFile, std::string& sFilePath);

	static void Roi(cv::Mat img,cv::Mat &roi,cv::Mat &resiezedImg);
	static void MakeContour(cv::Mat &img);
	static void ImpaintFore(cv::Mat& img);
	static void MakeAtlas();

	static cv::Mat GetTexture(int i);

	static std::vector<cv::Point> GetBoundary();
	static std::vector<cv::Point> GetCorner();
	static std::vector<std::vector<cv::Point>> GetContour();
	static std::vector<cv::Vec4i> GetHierarchy();
	static std::pair<int, int> GetAspect();


};