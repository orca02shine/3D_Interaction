#include "SimulationModel.h"

SimulationModel::SimulationModel(std::vector<cv::Point> contour, Shader* shader, Shader* wireShader, Texture* t)
:ParWin(nullptr),m_mesh(new Mesh)
{

	MeshCreator MC;

	MC.CreateForeGround(contour, m_vert, m_uv, m_idx, m_wireIdx);

	m_mesh->InsertMeshData(m_vert, m_uv, m_idx, m_wireIdx);
	m_mesh->LinkShader(shader, wireShader);
	m_mesh->LinkTexture(t);


}
SimulationModel::~SimulationModel() {
	delete m_mesh;
}

void SimulationModel::Update() {
	/*
	for (int i = 0; i < m_vert.size(); ++i) {
		m_vert[i].z += 0.01f;
	}
	*/
	m_mesh->UpdateVertices(m_vert);
}