#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "MeshCreator.h"

class SimulationModel {
private:
	class Window* ParWin;

	class Mesh* m_mesh;
	std::vector<glm::vec3> m_vert;
	std::vector<float> m_uv;
	std::vector<int> m_idx;
	std::vector<int> m_wireIdx;

	std::vector<int> m_tetIdx;

public:
	int numSubstep = 10;
	const float G = -6.0f;

	glm::vec3 gradient = { 0.0,0.0,0.0 };
	glm::vec3 gravity = { 0.0,G,0.0 };

	float fps = 1.0f / 60.0f;
	bool pause = false;

	float edgeComp = 0.02f;
	float areaComp = 0.01;

	float Width = 1.8f;
	float Height = 1.0f;

public:
	SimulationModel(std::vector<cv::Point> contour, Shader* shader, Shader* wireShader, Texture* t);
	~SimulationModel();
	void Init();
	void Update();

	void Simulate();
	void Solve(float dt);
	void PreSolve(float dt);
	void PostSolve(float dt);



};