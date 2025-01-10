#include "SimulationModel.h"

SimulationModel::SimulationModel(std::vector<cv::Point> contour,std::vector<std::vector<cv::Point>> innerConts,Shader* shader, Shader* wireShader, Texture* t)
	:ParWin(nullptr),
	_VertexObject(nullptr), _WireObject(nullptr), _MatShader(shader), _WireShader(wireShader), _Texture(t)
{


	MeshCreator MC;

	MC.CreateForeGround(contour,innerConts,m_vert, m_uv, m_idx, m_wireIdx,m_tetIdx,m_negaOfset);

	MakeMesh();

	m_numParticles = m_vert.size();

	m_prevPos.resize(m_numParticles);
	for (int i = 0; i < m_numParticles; ++i) {
		m_prevPos[i] = m_vert[i];
	}
	m_vel.resize(m_numParticles, { 0.0,0.0,0.0 });
	m_invMass.resize(m_numParticles, 1.0);

	FixPosition();

	Init();
}
SimulationModel::~SimulationModel() {
}

void SimulationModel::FixPosition() {
	int idx = -1;
	float maxY = 2.0f;

	for (int i = 0; i < m_vert.size(); ++i) {
		float y = m_vert[i].y;

		if (y < maxY) {
			maxY = y;
			idx = i;
		}
	}
	float offset = maxY - (-1.0);
	for (int i = 0; i < m_vert.size(); ++i) {
		m_vert[i].y = m_vert[i].y - offset;
	}

	float fixRange = -0.95;

	for (int i = 0; i < m_vert.size(); ++i) {
		if (m_vert[i].y < fixRange) {
			m_invMass[i] = 0;
		}
	}

}

void SimulationModel::Update() {

	Simulate();

	UpdateMesh();
}

void SimulationModel::SwitchVisibility() {
	_EnableMat = !_EnableMat;
	_EnableWire = !_EnableWire;
}

void SimulationModel::SwitchPause() {
	_Pause = !_Pause;
}

void SimulationModel::UpdateMesh() {
	if (_EnableMat) {
		_VertexObject->Update();
		_MatShader->SetActive();
		_VertexObject->SetActive();
		_Texture->SetActive();
		glDrawElements(GL_TRIANGLES, _VertexObject->_NumIndices, GL_UNSIGNED_INT, 0);
	}

	if (_EnableWire) {
		_WireObject->Update();
		_WireShader->SetActive();
		_WireObject->SetActive();
		glDrawElements(GL_LINES, _WireObject->_NumIndices, GL_UNSIGNED_INT, 0);
	}

}

void SimulationModel::MakeMesh() {

	_VertexObject = new VertexObject
	(3, m_vert.size(), m_vert.data(),
		m_uv.size(), m_uv.data(),
		m_idx.size(), m_idx.data());

	_WireObject = new VertexObject
	(3, m_vert.size(), m_vert.data(),
		m_uv.size(), m_uv.data(),
		m_wireIdx.size(), m_wireIdx.data());

}

void SimulationModel::Init() {
	int numTets = m_tetIdx.size() / 4;

	for (int i = 0; i < numTets; ++i) {
		int k = i * 4;
		InitDistanceConstraint(k);
		InitVolumeConstraint(k);
		//InitTetContactConstraint(k);
	}
	glm::mat3 rot = { -1, 0, 0, 0, 1, 0, 0, 0, -1 };
	for (int i = 0; i < m_vert.size(); ++i) {
		m_vert[i] = rot * m_vert[i];
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

void SimulationModel::InitTetContactConstraint(int k) {
	int a = m_tetIdx[k];
	int b = m_tetIdx[k + 1];
	int c = m_tetIdx[k + 2];
	int d = m_tetIdx[k + 3];

	float ai = m_invMass[a];
	float bi = m_invMass[b];
	float ci = m_invMass[c];
	float di = m_invMass[d];

	glm::vec3 bary = m_vert[a]*ai+m_vert[b]*bi+m_vert[c]*ci+m_vert[d]*di;
	bary /= 4;

	tetContactConstaraint tc(a, b, c, d, bary, ai, bi, ci, di);
	m_tetContactConstraint.push_back(tc);

}


void SimulationModel::Simulate() {
	if (_Pause) return;

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
	DuplicateConstraint();
}
void SimulationModel::PreSolve(float dt) {

	for (int i = 0; i < m_numParticles; ++i) {
		if (m_invMass[i] == 0)continue;

		m_vel[i] += (gravity * dt);

		m_prevPos[i] = m_vert[i];

		m_vert[i] += m_vel[i]*dt;

		if (m_vert[i].y < -1.0) {
			m_vert[i] = m_prevPos[i];
			m_vert[i].y = -1.0;
		}
		if (m_vert[i].y > 2.0) {
			m_vert[i] = m_prevPos[i];
			m_vert[i].y = 2.0;
		}

		if (m_vert[i].x < -2.0-((m_vert[i].z + 1.5) * 1.0)){
			m_vert[i] = m_prevPos[i];
			m_vert[i].x = -1.5 - ((m_vert[i].z + 1.5) * 1.0);
		}
		if (m_vert[i].x > 2.0 + ((m_vert[i].z + 1.5) * 1.0)){
			m_vert[i] = m_prevPos[i];
			m_vert[i].x = 1.5 + ((m_vert[i].z + 1.5) * 1.0);
		}
		if (m_vert[i].z < -1.5) {
			m_vert[i] = m_prevPos[i];
			m_vert[i].z = -1.5;
		}
		if (m_vert[i].z > 2.0) {
			m_vert[i] = m_prevPos[i];
			m_vert[i].z = 2.0;
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

	for (auto& e : m_distanceConstraint) {
		 p0 = m_vert[e.m_id[0]];
		 p1 = m_vert[e.m_id[1]];
		invM0 = m_invMass[e.m_id[0]];
		invM1 = m_invMass[e.m_id[1]];


		bool res = PBD::PositionBasedDynamics::solve_DistanceConstraint(
			p0, invM0, p1, invM1,
			e.m_restLength, stif, corr0, corr1);

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

	for (auto& e : m_volumeConstraint) {

		p0 = m_vert[e.m_id[0]];
		p1 = m_vert[e.m_id[1]];
		p2 = m_vert[e.m_id[2]];
		p3 = m_vert[e.m_id[3]];

		invM0 = m_invMass[e.m_id[0]];
		invM1 = m_invMass[e.m_id[1]];
		invM2 = m_invMass[e.m_id[2]];
		invM3 = m_invMass[e.m_id[3]];

		bool res = PBD::PositionBasedDynamics::solve_VolumeConstraint(
			p0,invM0,p1,invM1,p2,invM2,p3,invM3,
			e.m_restVolume,stif,
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

void SimulationModel::DuplicateConstraint() {
	for (int i = 0; i < m_negaOfset; ++i) {
		if (m_invMass[i + m_negaOfset] != 0) {
			m_vert[i + m_negaOfset] = m_vert[i];
		}
		if (m_invMass[i] != 0) {
			m_vert[i] = m_vert[i + m_negaOfset];
		}
	}
}

void::SimulationModel::solveTetContactConstraint() {
	//衝突すると判断されたとき更新？
}

void SimulationModel::updateTetContactInfo() {

	//衝突すると判断されたとき更新？

}

void SimulationModel::SetCoordinate(int id,glm::vec3 targetPos) {
	glm::vec3 v = targetPos - m_vert[id];
	v *= 100;
	m_vel[id] =v;
}

glm::vec3 SimulationModel::GetPos(int i) {
	return  m_vert[i];
}
int SimulationModel::GetNum() {
	return m_numParticles;
}