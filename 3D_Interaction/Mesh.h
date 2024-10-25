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

	class SimulationWindow* _Parent;

	std::vector<vec3> _Vertices; //íºê⁄Ç¢Ç∂ÇÈ
	std::vector<GLfloat> _UV;
	std::vector<GLuint> _Indices;
	std::vector<GLuint> _WireIdx;


public:
	Mesh(class SimulationWindow* window);
	~Mesh();

	void ProtoMesh();
	void UpdateMesh();
	void MakeVertices();
	void LinkTexture(Texture* tex);


};