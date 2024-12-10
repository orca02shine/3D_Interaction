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
	std::vector<glm::vec3> m_prevPos;
	std::vector<glm::vec3> m_vel;

	int m_numParticles;

	std::vector<int> m_idx;

	std::vector<int> m_tetIdx;

public:
	int numSubstep = 10;
	const float G = -0.1f;

	glm::vec3 gradient = { 0.0,0.0,0.0 };
	glm::vec3 gravity = { 0.0,G,0.0 };

	float fps = 1.0f / 60.0f;
	bool pause = true;


	float Width = 2.0f;
	float Height = 2.0f;

public:
	SimulationModel(std::vector<cv::Point> contour, Shader* shader, Shader* wireShader, Texture* t);
	~SimulationModel();
	void InitDistanceConstraint();
	void InitVolumeConstraint();
	void Update();

	void Simulate();
	void Solve(float dt);
	void PreSolve(float dt);
	void PostSolve(float dt);



};