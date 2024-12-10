#include "SimulationModel.h"

SimulationModel::SimulationModel(std::vector<cv::Point> contour, Shader* shader, Shader* wireShader, Texture* t)
:ParWin(nullptr),m_mesh(new Mesh)
{

	std::vector<float> uv;
	std::vector<int> idx;
	std::vector<int> wireIdx;

	MeshCreator MC;

	MC.CreateForeGround(contour, m_vert, uv, m_idx, wireIdx);

	m_mesh->InsertMeshData(m_vert, uv, m_idx, wireIdx);
	m_mesh->LinkShader(shader, wireShader);
	m_mesh->LinkTexture(t);

	m_numParticles = m_vert.size();

	m_prevPos.resize(m_numParticles);
	for (int i = 0; i < m_numParticles; ++i) {
		m_prevPos[i] = m_vert[i];
	}
	m_vel.resize(m_numParticles, { 0.0,0.0,0.0 });

}
SimulationModel::~SimulationModel() {
	delete m_mesh;
}

void SimulationModel::Update() {

	Simulate();

	m_mesh->UpdateVertices(m_vert);
}

void SimulationModel::InitDistanceConstraint() {

}

void SimulationModel::Simulate() {
	if (pause) return;

	float sdt = fps / numSubstep;

	for (int step = 0; step < numSubstep; step++) {

		PreSolve(sdt);

		Solve(sdt);

		PostSolve(sdt);
	}
}

void SimulationModel::Solve(float dt) {

}
void SimulationModel::PreSolve(float dt) {

	for (int i = 0; i < m_numParticles; ++i) {
		m_vel[i] += (gravity * dt);

		m_prevPos[i] = m_vert[i];

		m_vert[i] += m_vel[i]*dt;

		if (m_vert[i].y < -2.0) {
			m_vert[i] = m_prevPos[i];
			m_vert[i].y = -2.0;
		}
	}

}
void SimulationModel::PostSolve(float dt) {

	for (int i = 0; i < m_numParticles; ++i) {

		m_vel[i] = (m_vert[i] - m_prevPos[i]);
		m_vel[i] *= (1.0 / dt);
	}

}