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

	_WireIdx = { 0,1,1,2,2,0,0,2,2,3,3,0 };

}
void Mesh::ProtoMesh2() {
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