#pragma once
#include <GLEW/glew.h>
#include <iostream>
#include <glm/glm.hpp>


using namespace glm;


class VertexObject
{

	GLuint _VertArray;

	GLuint _VertBuffer;

	GLuint _IdxBuffer;

	GLuint _UvBuffer;

	int _DimSize;



public:

	const vec2* _Vert;
	GLsizei _NumVerts;
	GLsizei _NumIndices;

public:

	//size ’¸“_‚ÌŽŸŒ³
	VertexObject(GLint size, GLsizei numVerts, const vec2* vert,
		GLsizei numUV, const GLfloat* uv,
		GLsizei numindices, const GLuint* index);
	~VertexObject();


	void SetActive();
	void Update();



};