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

	std::vector<vec3> _Vertices; //íºê⁄Ç¢Ç∂ÇÈ
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

	void UpdateMesh();
	void MakeVertices();
	void LinkTexture(Texture* tex);
	void LinkShader(Shader* mat, Shader* wire);

};