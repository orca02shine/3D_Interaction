#include "VertexObject.h"



VertexObject::VertexObject(GLint size, GLsizei numVerts, const vec3* vert,
	GLsizei numUV, const GLfloat* uv,
	GLsizei numindices, const GLuint* index)
	:_Vert(vert), _NumVerts(numVerts), _NumIndices(numindices), _DimSize(size)
{
	//create vertex array
	glGenVertexArrays(1, &_VertArray);
	glBindVertexArray(_VertArray);

	//create vertex buffer
	glGenBuffers(1, &_VertBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _VertBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		numVerts * size * sizeof(float), vert, GL_DYNAMIC_DRAW);

	//éQè∆Ç≈Ç´ÇÈ
	glVertexAttribPointer(0, size, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);


	//uv
	glGenBuffers(1, &_UvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _UvBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		numUV * sizeof(float), uv, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	//index
	glGenBuffers(1, &_IdxBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IdxBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		numindices * sizeof(GLuint), index, GL_STATIC_DRAW);

	//normal?
	// 
	// 
	//


	std::cout << "vertex object is made" << std::endl;
}

VertexObject::~VertexObject()
{
	glDeleteVertexArrays(1, &_VertArray);
	glDeleteBuffers(1, &_VertBuffer);


	glDeleteBuffers(1, &_UvBuffer);
	glDeleteBuffers(1, &_IdxBuffer);

}



void VertexObject::SetActive() {
	glBindVertexArray(_VertArray);
}

void VertexObject::Update() {
	glBindBuffer(GL_ARRAY_BUFFER, _VertBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, _NumVerts * _DimSize * sizeof(float), _Vert);
}