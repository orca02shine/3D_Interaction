#include "Window.h"


Window::Window(int width = 1920, int height = 1080, const char* title = "No_Title")
	:window(glfwCreateWindow(width, height, title, nullptr, nullptr)),
	name(title)
{
	if (window == NULL) {
		std::cerr << "Can't create GLFW window." << std::endl;
		exit(1);
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Can't initialize GLEW" << std::endl;
		exit(1);
	}
	glfwSwapInterval(1);

	glfwSetWindowUserPointer(window, this);

	glfwSetWindowSizeCallback(window, Resize);
	glfwSetMouseButtonCallback(window, MouseCallback);


	Resize(window, width, height);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


Window::~Window()
{
	glfwDestroyWindow(window);
}


bool Window::LoopEvents()
{
	glfwMakeContextCurrent(window);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwPollEvents();

	glfwSwapBuffers(window);
	return !glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE);

}

void Window::MouseCallback(GLFWwindow* const window, int button, int action, int mods) {

	Window* const
		instance(static_cast<Window*>(glfwGetWindowUserPointer(window)));

	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
		instance->GetCursorPos(instance->_ClickedLocation[0], instance->_ClickedLocation[1]);
		//std::cout << "Mouse pos is  " <<instance-> _ClickedLocation[0] << "  " <<instance-> _ClickedLocation[1] << std::endl;
		instance->isClicked = true;
	}


	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
		instance->isClicked = false;
	}

}

void Window::GetCursorPos(float& X, float& Y) {
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	X = (x) * 2.0f / size[0] - 1.0f;
	Y = 1.0f - (y) * 2.0f / size[1];
}

void Window::Resize(GLFWwindow* const window, int width, int height) {

	int fbWidth, fbHeight;;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	glViewport(0, 0, fbWidth, fbHeight);

	Window* const
		instance(static_cast<Window*>(glfwGetWindowUserPointer(window)));
	if (instance != NULL)
	{
		// 開いたウィンドウのサイズを保存する
		instance->size[0] = static_cast<GLfloat>(width);
		instance->size[1] = static_cast<GLfloat>(height);

		instance->aspect = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);
	}

}

GLFWwindow* Window::getGLFW() {
	return this->window;
}

float Window::GetAspect() { return aspect; }




//-----------------------------


SimulationWindow::SimulationWindow(int width = 1280, int height = 720, const char* title = "No_Title")
	:Window(width, height, title)
{
	_Shader = new Shader();
	_Shader->Load("shader.vert", "shader.frag");

	_WireShader = new Shader();
	_WireShader->Load("shader.vert", "shader_wire.frag");

	_MatrixID = glGetUniformLocation(_Shader->GetShaderID(), "MVP");

	_Model = glm::mat4(1.0f);
	_View= glm::lookAt(
		glm::vec3(6, 5, 4), // カメラの位置
		glm::vec3(0, 0, 0), // カメラの視点
		glm::vec3(0, 1, 0)  // カメラの頭の方向
	);
	_Projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

	_MVP = _Projection * _View * _Model;


	test();


}

SimulationWindow::~SimulationWindow()
{
	//delete useShader;

}

bool SimulationWindow::LoopEvents() {

	glfwMakeContextCurrent(window);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwPollEvents();

	UpdateMVP();
	glUniformMatrix4fv(_MatrixID, 1, GL_FALSE, glm::value_ptr(_MVP));



	//MeshContoroller();



	//長押し状態
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) != GLFW_RELEASE)
	{
		GetCursorPos(_CurrentLocation[0], _CurrentLocation[1]);
		//std::cout << "Mouse pos is  " << _CurrentLocation[0] << "  " << _CurrentLocation[1] << std::endl;
	}

	for (int i = 0; i < _Meshes.size(); ++i) {
		_Meshes[i]->UpdateMesh();
	}


	glfwSwapBuffers(window);
	return !glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE);


}

void SimulationWindow::UpdateMVP() {


	_Projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

	_MVP = _Projection * _View * _Model;
}

void SimulationWindow::test() {
	Texture* t = new Texture(_Shader->GetShaderID());
	Mesh* m = new Mesh(this);
	m->LinkShader(_Shader,_WireShader);
	m->LinkTexture(t);


	_Shader->SetActive();//これ絶対いる

	_Meshes.push_back(m);
	_Textures.push_back(t);

}
