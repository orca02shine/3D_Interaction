#pragma once
#include "Mesh.h"

Mesh::Mesh(class SimulationWindow* window)
	:_Texture(nullptr),_VertexObject(nullptr), _Parent(window)
{

}

Mesh::~Mesh()
{
	delete _VertexObject;
}

void Mesh::ProtoMesh() {
	_Vertices = { {-1.0,-1.0,0.0},{1.0,-1.0,0.0},{1.0,1.0,0.0},{-1.0,1.0,0.0} };

	_UV = { 0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0 };

	_Indices = { 0,1,2,0,2,3 };

}

void Mesh::UpdateMesh() {

	_VertexObject->Update();
	_VertexObject->SetActive();

	_Texture->SetActive();

	glDrawElements(GL_LINES, _VertexObject->_NumIndices, GL_UNSIGNED_INT, 0);



}

void Mesh::MakeVertices() {

	ProtoMesh();

	_VertexObject = new VertexObject
	(3, _Vertices.size(), _Vertices.data(),
		_UV.size(), _UV.data(),
		_Indices.size(), _Indices.data());

}

void Mesh::LinkTexture(Texture* tex) {
	_Texture = tex;
}