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
	Texture*  _Texture;
	VertexObject* _VertexObject;

	std::vector<vec3> _Vertices; //íºê⁄Ç¢Ç∂ÇÈ
	std::vector<GLfloat> _UV;
	std::vector<GLuint> _Indices;
	std::vector<GLuint> _WireIdx;


public:
	Mesh(GLuint shadrProgram, class SimulationWindow* window);
	~Mesh();




};