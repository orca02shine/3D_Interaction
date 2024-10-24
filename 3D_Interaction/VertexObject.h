#pragma once
#include <GLEW/glew.h>
#include <iostream>

struct Vertex {

	GLfloat x;
	GLfloat y;
	GLfloat z;
};

class VertexObject
{

	GLuint _VertArray;

	GLuint _VertBuffer;

	GLuint _IdxBuffer;

	GLuint _UvBuffer;


public:

	//const Vertex* _Vert;
	GLsizei _NumVerts;
	GLsizei _NumIndices;

public:

	//size ’¸“_‚ÌŽŸŒ³
	VertexObject(GLint size, GLsizei numVerts, const Vertex* vert,
		GLsizei numUV, const GLfloat* uv,
		GLsizei numindices, const GLuint* index);
	~VertexObject();


	void SetActive();
	void Update();



};