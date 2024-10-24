#pragma once

#include <GLEW/glew.h>
#include <vector>
#include <iostream>

#include "Shader.h"


class Texture {
	Texture(GLuint shaderProgram);
	~Texture();


	bool Load();
	void Unload();

	void LinkShader();

	int GetWidth() const { return _Width; }
	int GetHeight() const { return _Height; }

public:
	GLuint _TextureID;
	GLuint _TexSamplerID;
	int _Width;
	int _Height;

	GLuint _ShaderProgram;




};