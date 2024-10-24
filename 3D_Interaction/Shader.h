#pragma once

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

class Shader
{
	// Store the shader object IDs
	GLuint _VertexShader;
	GLuint _FragShader;
	GLuint _ShaderProgram;

	//GLuint sizeLoc;
	//GLuint scaleLoc;

public:
	Shader();
	~Shader();
	// Load the vertex/fragment shaders with the given names
	bool Load(const std::string& vertName, const std::string& fragName);
	void Unload();
	// Set this as the active shader program
	void SetActive();

	GLuint GetShaderID() { return _ShaderProgram; }

private:
	// Tries to compile the specified shader
	bool CompileShader(const std::string fileName,
		const GLenum shaderType,
		GLuint& outShader);

	// Tests whether shader compiled successfully
	bool IsCompiled(GLuint shader);
	// Tests whether vertex/fragment programs link
	bool IsValidProgram();

};