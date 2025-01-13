#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "Delauney.h"

class MeshCreator {
public:
	MeshCreator();
	~MeshCreator();

	void CreateBackGround(std::vector<cv::Point> cor, std::vector<cv::Point> boundary, 
		std::vector<glm::vec3> &vert, std::vector<float> &uv, std::vector<uint> &idx, std::vector<uint> &wireIdx);

	void CreateBackGround_NoWall(std::vector<cv::Point> cor, std::vector<cv::Point> boundary,
		std::vector<glm::vec3>& vert, std::vector<float>& uv, std::vector<uint>& idx, std::vector<uint>& wireIdx);

	void CreateQuad(float asp,
			std::vector<glm::vec3>& vert, std::vector<float>& uv, std::vector<uint>& idx, std::vector<uint>& wireIdx);

	void CreateForeGround(std::vector<cv::Point> contour, std::vector<std::vector<cv::Point>> innerConts,
		std::vector<glm::vec3>& vert, std::vector<float>& uv, std::vector<uint>& idx, std::vector<uint>& wireIdx, std::vector<int>& tetIdx, int &negaOfset);

	void CalcCord(float& x, float& y, float& z);

	void CreateCursor(std::vector<glm::vec3>& vert, std::vector<float>& uv, std::vector<uint>& idx, std::vector<uint>& wireIdx);

};