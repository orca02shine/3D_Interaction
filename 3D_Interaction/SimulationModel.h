#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "MeshCreator.h"
#include "PositionBasedDynamics.h"

struct distanceConstraint {
	int m_id[2];
	float m_restLength;

	distanceConstraint(int a, int b, float l) {
		m_id[0] = a;
		m_id[1] = b;
		m_restLength = l;
	}
};

struct volumeConstraint {
	int m_id[4];
	float m_restVolume;

	volumeConstraint(int a, int b, int c, int d, float v) {
		m_id[0] = a;
		m_id[1] = b;
		m_id[2] = c;
		m_id[3] = d;

		m_restVolume = v;
	}
};

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

	std::vector<float> m_invMass;

	std::vector<distanceConstraint> m_distanceConstraint;

public:
	int numSubstep = 10;
	const float G = -1.0f;

	glm::vec3 gradient = { 0.0,0.0,0.0 };
	glm::vec3 gravity = { 0.0,G,0.0 };

	float fps = 1.0f / 60.0f;
	bool pause = true;

public:
	SimulationModel(std::vector<cv::Point> contour, Shader* shader, Shader* wireShader, Texture* t);
	~SimulationModel();
	void Init();
	void InitDistanceConstraint(int k);
	void InitVolumeConstraint(int k);
	void Update();

	void Simulate();
	void Solve(float dt);
	void PreSolve(float dt);
	void PostSolve(float dt);

	void solveDistanceConstraint(float dt);
	void solveVolumeConstaraint();


};