#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "MeshCreator.h"

class SimulationModel {
public:
	class Window* ParWin;

	class Mesh* m_mesh;

	std::vector<glm::vec3> m_vert;
	std::vector<float> m_uv;
	std::vector<int> m_idx;
	std::vector<int> m_wireIdx;

public:
	SimulationModel(std::vector<cv::Point> contour, Shader* shader, Shader* wireShader, Texture* t);
	~SimulationModel();
	void Init();
	void Update();





};