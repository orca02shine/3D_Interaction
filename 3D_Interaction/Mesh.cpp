#pragma once
#include "Mesh.h"

Mesh::Mesh()
	:_Texture(nullptr),_VertexObject(nullptr)
{

}

Mesh::~Mesh()
{
	delete _VertexObject;
}

void Mesh::ProtoMesh() {
	_Vertices.clear();
	_UV.clear();
	_Indices.clear();
	_WireIdx.clear();


	_Vertices = { {-1.0,-1.0,0.0},{1.0,-1.0,0.0},{1.0,1.0,0.0},{-1.0,1.0,0.0} };

	_UV = { 0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0 };

	_Indices = { 0,1,2,0,2,3 };

	_WireIdx = { 0,1,1,2,2,0,0,2,2,3,3,0 };

	MakeVertices();
}
void Mesh::ProtoMesh2() {
	_Vertices.clear();
	_UV.clear();
	_Indices.clear();
	_WireIdx.clear();


	_Vertices = { {-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},{1.0,1.0,-1.0},{-1.0,1.0,-1.0},
					{-1.0,-1.0,1.0},{1.0,-1.0,1.0},{1.0,1.0,1.0},{-1.0,1.0,1.0} };

	_UV = { 0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0,
			0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0 };

	_Indices = { 0,1,2, 2,3,0, 4,5,6, 6,7,4,
				 0,1,5, 5,4,0, 1,2,6, 6,5,1,
				 2,3,7, 7,6,2, 3,0,4, 4,7,3};

	_WireIdx = { 0,1,1,2,2,0, 0,2,2,3,3,0,  4,5,5,6,6,4, 6,7,7,4,4,6,
				 0,1,1,5,5,0, 5,4,4,0,0,5,  1,2,2,6,6,1, 6,5,5,1,1,6,
				 2,3,3,7,7,2, 7,6,6,2,2,7,  3,0,0,4,4,3, 4,7,7,3,3,4};

	MakeVertices();
}

void Mesh::InsertMeshData(std::vector<vec3> vert, std::vector<float> uv, std::vector<int> idx, std::vector<int> wireIdx) {

	_Vertices.clear();
	_UV.clear();
	_Indices.clear();
	_WireIdx.clear();

	for (int i = 0; i < vert.size(); ++i) {
		float x = vert[i].x;
		float y = vert[i].y;
		float z = vert[i].z;
		_Vertices.push_back({ x,y,z });
	}
	for (int i = 0; i < uv.size(); ++i) {
		_UV.push_back(uv[i]);
	}
	for (int i = 0; i < idx.size(); ++i) {
		_Indices.push_back(idx[i]);
	}
	for (int i = 0; i < wireIdx.size(); ++i) {
		_WireIdx.push_back(wireIdx[i]);
	}

	MakeVertices();
}

void Mesh::UpdateMesh() {

	if (_EnableMat) {
		_MatShader->SetActive();

		_VertexObject->Update();
		_VertexObject->SetActive();
		_Texture->SetActive();
		glDrawElements(GL_TRIANGLES, _VertexObject->_NumIndices, GL_UNSIGNED_INT, 0);
	}

	if (_EnableWire) {
		_WireShader->SetActive();

		_WireObject->Update();
		_WireObject->SetActive();
		glDrawElements(GL_LINES, _WireObject->_NumIndices, GL_UNSIGNED_INT, 0);
	}
	

}

void Mesh::MakeVertices() {


	_VertexObject = new VertexObject
	(3, _Vertices.size(), _Vertices.data(),
		_UV.size(), _UV.data(),
		_Indices.size(), _Indices.data());

	_WireObject = new VertexObject
	(3, _Vertices.size(), _Vertices.data(),
		_UV.size(), _UV.data(),
		_WireIdx.size(), _WireIdx.data());

}

void Mesh::LinkTexture(Texture* tex) {
	_Texture = tex;
}

void Mesh::LinkShader(Shader* mat, Shader* wire) {
	_MatShader = mat;
	_WireShader = wire;

}