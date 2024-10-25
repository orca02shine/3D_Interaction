#pragma once
#include "Mesh.h"

Mesh::Mesh(class SimulationWindow* window)
	:_Texture(nullptr),_VertexObject(nullptr), _Parent(window)
{
	MakeVertices();

}

Mesh::~Mesh()
{
	delete _VertexObject;
}

void Mesh::ProtoMesh() {
	_Vertices = { {-0.5,-0.5},{0.5,-0.5},{0.5,0.5},{-0.5,0.5} };

	_UV = { 0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0 };

	_Indices = { 0,1,2,0,2,3 };

	_WireIdx = { 0,1,1,2,2,0,0,2,2,3,3,0 };

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

	ProtoMesh();

	_VertexObject = new VertexObject
	(2, _Vertices.size(), _Vertices.data(),
		_UV.size(), _UV.data(),
		_Indices.size(), _Indices.data());

	_WireObject = new VertexObject
	(2, _Vertices.size(), _Vertices.data(),
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