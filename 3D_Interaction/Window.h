#pragma once

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include"Shader.h"
#include"Mesh.h"
#include"Texture.h"
#include"MeshCreator.h"
#include"SimulationModel.h"
#include "CVInterface.h"

class Window {

public:
	GLFWwindow* const window;

	GLfloat size[2];

	float aspect;
	glm::vec4 _ViewPort;

	const char* name;

	float _CurrentLocation[2];
	float _ClickedLocation[2];
	bool isClicked = false;

	float _CurMid[2];
	float _PreMid[2];
	float _IsMid = false;



public:

	Window(int width, int height, const char* title);
	~Window();

	virtual bool LoopEvents();

	//static void MouseCallback(GLFWwindow* const window, int button, int action, int mods);
	void GetCursorPos(float& X, float& Y);

	static void Resize(GLFWwindow* const window, int width, int height);
	void SetViewPort();
	GLFWwindow* getGLFW();

	float GetAspect();

};

//-------------------------------------------------


class SimulationWindow :Window {

public:

	std::vector<class Texture*> _Textures;
	std::vector<class SimulationModel*> _Models;
	class Mesh* _BackGround = nullptr;
	class SimulationModel* _SelectedModel = nullptr;
	int _VertPtr = -1;

	Shader* _Shader;
	Shader* _WireShader;
	GLuint _MatrixID;


	glm::mat4 _Model;
	glm::mat4 _View;
	glm::mat4 _Projection;
	glm::mat4 _MVP;

	glm::vec3 _CameraPos;
	glm::vec3 _CameraCenter;
	glm::vec3 _Up = glm::vec3(0, 1, 0);

	float _ScreenPos[2];
	glm::vec3 _ScreenClipPos;

	const float minArea = 0.1f;
	//float limX = 2.0f;
	//float limY = 1.0f;
	float _Speed = 50.0f;

public:


	SimulationWindow(int width, int height, const char* title);
	~SimulationWindow();

	static void MouseCallbackSim(GLFWwindow* const window, int button, int action, int mods);
	static void KeyCallbackSim(GLFWwindow* const window, int key, int scancode, int action, int mods);

	void UpdateMVP();
	void UpdateMousePos();
	void Revolution(float prex, float prey);

	void GetScreenPos(float& x, float& y);

	bool LoopEvents() override;
	void SwitchMeshVisibility();
	void SwitchPause();
	void MeshSearcher();
	void MeshContoroller();
	void MeshTargetClear();
	void SetScreenClipPos();

	void test();
	void test_pbd();

};