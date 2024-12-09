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
	m_mesh->UpdateMesh();
}