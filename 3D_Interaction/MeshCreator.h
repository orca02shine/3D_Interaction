#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <random>

#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

class MeshCreator {
public:
	MeshCreator();
	~MeshCreator();

	void CreateBackGround(std::vector<cv::Point> cor, std::vector<cv::Point> boundary, 
		std::vector<glm::vec3> &vert, std::vector<float> &uv, std::vector<int> &idx, std::vector<int> &wireIdx);

	void Delauney();

	void CalcCord(float& x, float& y, float& z);

};