#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

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

struct tetContactConstaraint {
	int m_id[4];
	float m_invMass[4];
	glm::vec3 m_bary;
	float m_lambda=1.0f;
	float m_fricCoef=0.0f;
	//glm::mat3 m_constraintInfo;
	//std::array<glm::vec3, 4> m_x;
	//std::array<glm::vec3, 4> m_v;

	tetContactConstaraint(int a, int b, int c, int d, glm::vec3 bary,
						float ai, float bi, float ci, float di) {
		m_id[0] = a;
		m_id[1] = b;
		m_id[2] = c;
		m_id[3] = d;

		m_bary = bary;

		m_invMass[0] = ai;
		m_invMass[1] = bi;
		m_invMass[2] = ci;
		m_invMass[3] = di;
	}

};

class SimulationModel {
private:
	class Window* ParWin;

	std::vector<glm::vec3> m_vert;
	std::vector<glm::vec3> m_prevPos;
	std::vector<glm::vec3> m_vel;

	int m_numParticles;

	std::vector<float> m_uv;
	std::vector<unsigned int> m_idx;
	std::vector<unsigned int> m_wireIdx;

	std::vector<int> m_tetIdx;

	std::vector<float> m_invMass;

	std::vector<distanceConstraint> m_distanceConstraint;
	std::vector<volumeConstraint> m_volumeConstraint;
	std::vector<tetContactConstaraint> m_tetContactConstraint;

	//---------------------------------------------
	class Texture* _Texture;
	class VertexObject* _VertexObject;
	class VertexObject* _WireObject;
	class Shader* _MatShader;
	class Shader* _WireShader;

	bool _EnableMat = true;
	bool _EnableWire = false;
	//--------------------------------------------------

public:
	int numSubstep = 6;
	const float G = -6.0f;

	float stif = 100.0;

	glm::vec3 gradient = { 0.0,0.0,0.0 };
	glm::vec3 gravity = { 0.0,G,0.0 };

	float fps = 1.0f / 60.0f;
	bool pause = true;

	glm::vec3 corr0;
	glm::vec3 corr1;
	glm::vec3 corr2;
	glm::vec3 corr3;

	glm::vec3 p0;
	glm::vec3 p1;
	glm::vec3 p2;
	glm::vec3 p3;

	float invM0;
	float invM1;
	float invM2;
	float invM3;

public:
	SimulationModel(std::vector<cv::Point> contour, Shader* shader, Shader* wireShader, Texture* t);
	~SimulationModel();
	void Init();
	void InitDistanceConstraint(int k);
	void InitVolumeConstraint(int k);
	void InitTetContactConstraint(int k);
	void Update();
	void UpdateMesh();
	void MakeMesh();

	void Simulate();
	void Solve(float dt);
	void PreSolve(float dt);
	void PostSolve(float dt);

	void solveDistanceConstraint(float dt);
	void solveVolumeConstaraint(float dt);
	void solveTetContactConstraint();
	void updateTetContactInfo();


	void SetCoordinate(int id, glm::vec3 targetPos);
	glm::vec3 GetPos(int i);
	int GetNum();

	void LinkTexture(Texture* tex);
	void LinkShader(Shader* mat, Shader* wire);

};