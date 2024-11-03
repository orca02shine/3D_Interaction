#pragma once

#include <opencv2/opencv.hpp>
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include  "Window.h"
#include "Loader.h"


class Application {

	class SimulationWindow* _MainWindow;


public:
	Application();

	bool Initialize();
	void RunLoop();
	void ShutDown();

	void MakeWindow();

	void Test();


};
