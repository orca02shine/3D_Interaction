#pragma once

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

class Window {

public:
	GLFWwindow* const window;

	GLfloat size[2];

	float aspect;

	const char* name;

	float _CurrentLocation[2];
	float _ClickedLocation[2];
	bool isClicked = false;



public:

	Window(int width, int height, const char* title);
	~Window();

	virtual bool LoopEvents();

	static void MouseCallback(GLFWwindow* const window, int button, int action, int mods);
	void GetCursorPos(float& X, float& Y);

	static void Resize(GLFWwindow* const window, int width, int height);
	GLFWwindow* getGLFW();

	float GetAspect();
};