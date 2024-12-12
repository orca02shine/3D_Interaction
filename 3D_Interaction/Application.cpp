#include "Application.h"

Application::Application()
	:_MainWindow(nullptr)
{



}

bool Application::Initialize()
{
	if (glfwInit() == GL_FALSE) {
		return false;
	}

	atexit(glfwTerminate);

	const char* glsl_version = "#version 330";

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	MakeWindow();


	Test();



	return true;
}

void Application::ShutDown()
{
	//delete _MainWindow;
	//delete _GuiWindow;
	glfwTerminate();
}

void Application::MakeWindow()
{
	_MainWindow = new SimulationWindow(1920, 1080, "Main_Window");

	//_GuiWindow = new GuiWindow(512,512,"Gui_Window");

}


void Application::RunLoop()
{
	using frame_duration = std::chrono::duration<int, std::ratio<1, 60>>; // 90 FPS
	auto next_frame = std::chrono::steady_clock::now() + frame_duration{};


	if (_MainWindow == nullptr /* || _GuiWindow == nullptr*/) {
		return;
	}

	while (/*_GuiWindow->LoopEvents() && */ _MainWindow->LoopEvents())
	{

		std::this_thread::sleep_until(next_frame); // éüÇÃÉtÉåÅ[ÉÄÇ‹Ç≈ë“ã@
		next_frame += frame_duration{};
	}



}

void Application::Test() 
{
}