#include "SimulationModel.h"

SimulationModel::SimulationModel(std::vector<cv::Point> contour, Shader* shader, Shader* wireShader, Texture* t)
:ParWin(nullptr),m_mesh(new Mesh)
{

	std::vector<float> uv;
	std::vector<int> idx;
	std::vector<int> wireIdx;

	MeshCreator MC;

	MC.CreateForeGround(contour, m_vert, uv, m_idx, wireIdx,m_tetIdx);

	m_mesh->InsertMeshData(m_vert, uv, m_idx, wireIdx);
	m_mesh->LinkShader(shader, wireShader);
	m_mesh->LinkTexture(t);

	m_numParticles = m_vert.size();

	m_prevPos.resize(m_numParticles);
	for (int i = 0; i < m_numParticles; ++i) {
		m_prevPos[i] = m_vert[i];
	}
	m_vel.resize(m_numParticles, { 0.0,0.0,0.0 });
	m_invMass.resize(m_numParticles, 1.0);

	Init();
}
SimulationModel::~SimulationModel() {
	delete m_mesh;
}

void SimulationModel::Update() {

	Simulate();

	m_mesh->UpdateVertices(m_vert);
}

void SimulationModel::Init() {
	int numTets = m_tetIdx.size() / 4;

	for (int i = 0; i < numTets; ++i) {
		int k = i * 4;
		InitDistanceConstraint(k);
		InitVolumeConstraint(k);
	}
}

void SimulationModel::InitDistanceConstraint(int k) {

	int a = m_tetIdx[k];
	int b = m_tetIdx[k + 1];
	int c = m_tetIdx[k + 2];
	int d = m_tetIdx[k + 3];

	distanceConstraint dc0(a, b, glm::distance(m_vert[a], m_vert[b]));
	distanceConstraint dc1(b, c, glm::distance(m_vert[b], m_vert[c]));
	distanceConstraint dc2(c, d, glm::distance(m_vert[c], m_vert[d]));
	distanceConstraint dc3(d, a, glm::distance(m_vert[d], m_vert[a]));

	m_distanceConstraint.push_back(dc0);
	m_distanceConstraint.push_back(dc1);
	m_distanceConstraint.push_back(dc2);
	m_distanceConstraint.push_back(dc3);

}

void SimulationModel::InitVolumeConstraint(int k) {
	int a = m_tetIdx[k];
	int b = m_tetIdx[k + 1];
	int c = m_tetIdx[k + 2];
	int d = m_tetIdx[k + 3];

	glm::vec3 p0 = m_vert[a];
	glm::vec3 p1 = m_vert[b];
	glm::vec3 p2 = m_vert[c];
	glm::vec3 p3 = m_vert[d];

	float volume = static_cast<float>(1.0 / 6.0) * glm::dot(glm::cross(p1 - p0, p2 - p0), p3 - p0);


	volumeConstraint vc(a, b, c, d, volume);
	m_volumeConstraint.push_back(vc);
}


void SimulationModel::Simulate() {
	//if (pause) return;

	float sdt = fps / numSubstep;

	for (int step = 0; step < numSubstep; step++) {

		PreSolve(sdt);

		Solve(sdt);

		PostSolve(sdt);
	}
}

void SimulationModel::Solve(float dt) {
	solveDistanceConstraint(dt);
	solveVolumeConstaraint(dt);
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

void SimulationModel::solveDistanceConstraint(float dt) {

	float stif = 100.0;

	for (auto& e : m_distanceConstraint) {
		glm::vec3 p0 = m_vert[e.m_id[0]];
		glm::vec3 p1 = m_vert[e.m_id[1]];
		float invM0 = m_invMass[e.m_id[0]];
		float invM1 = m_invMass[e.m_id[1]];

		float restL = e.m_restLength;

		glm::vec3 corr0 = { 0,0,0 };
		glm::vec3 corr1 = { 0,0,0 };

		bool res = PBD::PositionBasedDynamics::solve_DistanceConstraint(
			p0, invM0, p1, invM1,
			restL, stif, corr0, corr1);

		if (res) {
			if (invM0 != 0.0) {
				m_vert[e.m_id[0]] += (corr0 * dt);
			}
			if (invM1 != 0.0) {
				m_vert[e.m_id[1]] += (corr1 * dt);
			}
		}

	}

}

void SimulationModel::solveVolumeConstaraint(float dt) {
	float stif = 100.0;

	for (auto& e : m_volumeConstraint) {

		glm::vec3 p0 = m_vert[e.m_id[0]];
		glm::vec3 p1 = m_vert[e.m_id[1]];
		glm::vec3 p2 = m_vert[e.m_id[2]];
		glm::vec3 p3 = m_vert[e.m_id[3]];

		float invM0 = m_invMass[e.m_id[0]];
		float invM1 = m_invMass[e.m_id[1]];
		float invM2 = m_invMass[e.m_id[2]];
		float invM3 = m_invMass[e.m_id[3]];

		float restV = e.m_restVolume;

		glm::vec3 corr0 = { 0,0,0 };
		glm::vec3 corr1 = { 0,0,0 };
		glm::vec3 corr2 = { 0,0,0 };
		glm::vec3 corr3 = { 0,0,0 };

		bool res = PBD::PositionBasedDynamics::solve_VolumeConstraint(
			p0,invM0,p1,invM1,p2,invM2,p3,invM3,
			restV,stif,
			corr0,corr1,corr2,corr3);

		if (res) {
			if (invM0 != 0.0) {
				m_vert[e.m_id[0]] += (corr0 * dt);
			}
			if (invM1 != 0.0) {
				m_vert[e.m_id[1]] += (corr1 * dt);
			}
			if (invM2 != 0.0) {
				m_vert[e.m_id[2]] += (corr2 * dt);
			}
			if (invM3 != 0.0) {
				m_vert[e.m_id[3]] += (corr3 * dt);
			}
		}


	}
}

void SimulationModel::SetCoordinate(int id, int x, int y) {
	m_vert[id].x = x;
}

glm::vec3 SimulationModel::GetPos(int i) {
	return  m_vert[i];
}
int SimulationModel::GetNum() {
	return m_numParticles;
}