#include "Shader.h"

Shader::Shader()
	: _ShaderProgram(0)
	, _VertexShader(0)
	, _FragShader(0)
{

}
Shader::~Shader()
{

}

bool Shader::Load(const std::string& vertName, const std::string& fragName) {

	_ShaderProgram = glCreateProgram();

	if (!CompileShader(vertName, GL_VERTEX_SHADER, _VertexShader) || !CompileShader(fragName, GL_FRAGMENT_SHADER, _FragShader)) {

		return false;
	}

	glLinkProgram(_ShaderProgram);

	//set uniform
	//sizeLoc = glGetUniformLocation(_ShaderProgram, "size");


	if (!IsValidProgram()) {
		return false;
	}
	return true;

}

void Shader::Unload() {
	glDeleteProgram(_ShaderProgram);
	glDeleteProgram(_VertexShader);
	glDeleteProgram(_FragShader);
}

void Shader::SetActive() {
	glUseProgram(_ShaderProgram);
}

bool Shader::CompileShader(const std::string fileName, GLenum shaderType, GLuint& outShader) {

	std::ifstream shaderFile(fileName, std::ios::binary);
	if (shaderFile.fail()) {
		std::cerr << "Error: Can't open source file: " << fileName << std::endl;
		return false;
	}
	auto shaderSource = std::string(std::istreambuf_iterator<char>(shaderFile),
		std::istreambuf_iterator<char>());

	const GLchar* shaderSourcePointer = shaderSource.c_str();

	GLuint shader = glCreateShader(shaderType);

	glShaderSource(shader, 1, &shaderSourcePointer, nullptr);
	glCompileShader(shader);
	glAttachShader(_ShaderProgram, shader);
	if (!IsCompiled(shader)) {
		return false;
	}


	return true;
}

bool Shader::IsCompiled(GLuint shader) {
	GLint status = GL_FALSE;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {
		std::cerr << "Compile Error." << std::endl;

		GLsizei infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 1) {

			std::vector<GLchar> vertexShaderErrorMessage(infoLogLength);
			glGetShaderInfoLog(shader, infoLogLength, nullptr,
				vertexShaderErrorMessage.data());
			std::cerr << vertexShaderErrorMessage.data() << std::endl;

		}

		return false;
	}

	return true;
}

bool Shader::IsValidProgram() {
	GLint status = GL_FALSE;

	glGetShaderiv(_ShaderProgram, GL_VALIDATE_STATUS, &status);

	if (status == GL_FALSE) {
		std::cerr << "Link Erorr" << std::endl;

		GLsizei infoLogLength;

		glGetProgramiv(_ShaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 1) {
			std::vector<GLchar> programLinkErrorMessage(infoLogLength);
			glGetProgramInfoLog(_ShaderProgram, infoLogLength, nullptr,
				programLinkErrorMessage.data());
			std::cerr << programLinkErrorMessage.data() << std::endl;
		}
		else {
			std::cerr << "log length is 0" << std::endl;
		}
		return false;
	}

	return true;
}