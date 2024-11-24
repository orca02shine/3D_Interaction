#pragma once

#include "Window.h"
#include "Texture.h"
#include "VertexObject.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <random>


class Mesh {

private:
	class Texture*  _Texture;
	class VertexObject* _VertexObject;
	class VertexObject* _WireObject;

	class Shader* _MatShader;
	class Shader* _WireShader;

	class SimulationWindow* _Parent;

	std::vector<vec3> _Vertices; //���ڂ�����
	std::vector<GLfloat> _UV;
	std::vector<GLuint> _Indices;
	std::vector<GLuint> _WireIdx;

	bool _EnableMat = true;
	bool _EnableWire = false;


public:
	Mesh(class SimulationWindow* window);
	~Mesh();

	void ProtoMesh();
	void ProtoMesh2();

	void InsertMeshData(std::vector<vec3> vert, std::vector<float> uv, std::vector<int> idx, std::vector<int> wireIdx);
	void UpdateMesh();
	void MakeVertices();
	void LinkTexture(Texture* tex);
	void LinkShader(Shader* mat, Shader* wire);

};