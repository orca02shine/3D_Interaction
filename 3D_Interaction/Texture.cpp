#include "Texture.h"

Texture::Texture(GLuint shaderProgram)
	:_TextureID(0),
	_TexSamplerID(0),
	_Width(0),
	_Height(0),
	_ShaderProgram(shaderProgram)
{
	//test
	ProtoTex();
}

Texture::~Texture() {
}


bool Texture::Load() {


	return true;
}

void Texture::SetShader(int width,int height,int* data){
	_Width = width;
	_Height = height;

	glGenTextures(1, &_TextureID);
	glBindTexture(GL_TEXTURE_2D, _TextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _Width, _Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	LinkShader();


}

void Texture::Unload()
{
	glDeleteTextures(1, &_TextureID);
}

void Texture::LinkShader()
{
	_TexSamplerID = glGetUniformLocation(_ShaderProgram, "texSampler");
}

void Texture::SetActive()
{
	glBindTexture(GL_TEXTURE_2D, _TextureID);
	//glUniform1i(_TexSamplerID, 0);
}

void Texture::ProtoTex() {
	int w = 256;

	std::vector<int> protodata(w * w * 4 * sizeof(int), 120);

	SetShader(w, w, protodata.data());
}