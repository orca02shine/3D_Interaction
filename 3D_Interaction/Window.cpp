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
	//glfwSetMouseButtonCallback(window, MouseCallback);


	Resize(window, width, height);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	//glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);


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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwPollEvents();

	glfwSwapBuffers(window);
	return !glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE);

}

/*
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

	if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_PRESS) {
		instance->_IsMid = true;
	}
	if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_RELEASE) {
		instance->_IsMid = false;
	}



}
*/

void Window::GetCursorPos(float& X, float& Y) {
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	X = (x) * 2.0f / size[0] - 1.0f;
	Y = 1.0f - (y) * 2.0f / size[1];
}

void Window::Resize(GLFWwindow* const window, int width, int height) {

	int fbWidth, fbHeight;
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
		instance->SetViewPort();
	}

}

void Window::SetSize(int width, int height) {
	glfwSetWindowSize(window, width, height);

	size[0] = static_cast<GLfloat>(width);
	size[1] = static_cast<GLfloat>(height);

	aspect = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);
	SetViewPort();
	glViewport(0, 0, width, height);
}

void Window::SetViewPort() {
	_ViewPort = glm::vec4(0, 0, size[0], size[1]);
}

GLFWwindow* Window::getGLFW() {
	return this->window;
}

float Window::GetAspect() { return aspect; }




//-----------------------------


SimulationWindow::SimulationWindow(int width = 1280, int height = 720, const char* title = "No_Title")
	:Window(width, height, title)
{
	glfwSetMouseButtonCallback(window, MouseCallbackSim);
	glfwSetKeyCallback(window, KeyCallbackSim);

	_Shader = new Shader();
	_Shader->Load("shader.vert", "shader.frag");

	_WireShader = new Shader();
	_WireShader->Load("shader.vert", "shader_wire.frag");

	_MatrixID = glGetUniformLocation(_Shader->GetShaderID(), "MVP");


	_CameraPos = glm::vec3(0.0, 0.0, -4.0);
	_CameraCenter = glm::vec3(0, 0, 0);

	_Model = glm::mat4(1.0f);
	_View= glm::lookAt(
		_CameraPos, // カメラの位置
		_CameraCenter, // カメラの視点
		glm::vec3(0, 1, 0)  // カメラの頭の方向
	);
	_Projection = glm::perspectiveFov(glm::radians(45.0f), size[0], size[1], 0.1f, 10.0f);

	_MVP = _Projection * _View * _Model;


	test_pbd();
	test();

}

SimulationWindow::~SimulationWindow()
{
	//delete useShader;

}

void SimulationWindow::MouseCallbackSim(GLFWwindow* const window, int button, int action, int mods) {

	SimulationWindow* const
		instance(static_cast<SimulationWindow*>(glfwGetWindowUserPointer(window)));

	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
		instance->GetCursorPos(instance->_ClickedLocation[0], instance->_ClickedLocation[1]);
		//std::cout << "Mouse pos is  " <<instance-> _ClickedLocation[0] << "  " <<instance-> _ClickedLocation[1] << std::endl;
		instance->isClicked = true;
		instance->MeshSearcher();
		instance->SetScreenClipPos();
	}


	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
		instance->isClicked = false;
		instance->MeshTargetClear();
	}

	if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_PRESS) {
		instance->_IsMid = true;
	}
	if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_RELEASE) {
		instance->_IsMid = false;
	}



}
void SimulationWindow::KeyCallbackSim(GLFWwindow* const window, int key, int scancode, int action, int mods) {

	SimulationWindow* const
		instance(static_cast<SimulationWindow*>(glfwGetWindowUserPointer(window)));

	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		instance->SwitchPause();
	}

	if (key == GLFW_KEY_V && action == GLFW_PRESS) {
		instance->SwitchMeshVisibility();
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		instance->SwitchView();
	}

}

bool SimulationWindow::LoopEvents() {

	glfwMakeContextCurrent(window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwPollEvents();

	UpdateMousePos();
	UpdateMVP();
	glUniformMatrix4fv(_MatrixID, 1, GL_FALSE, glm::value_ptr(_MVP));

	GetScreenPos(_ScreenPos[0], _ScreenPos[1]);

	MeshContoroller();



	//長押し状態
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) != GLFW_RELEASE)
	{
		GetCursorPos(_CurrentLocation[0], _CurrentLocation[1]);
		//std::cout << "Mouse pos is  " << _CurrentLocation[0] << "  " << _CurrentLocation[1] << std::endl;
	}

	for (int i = 0; i < _Models.size(); ++i) {
		_Models[i]->Update();
	}
	if (_BackGround != nullptr) {
		_BackGround->UpdateMesh();
	}


	glfwSwapBuffers(window);
	return !glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE);


}

void SimulationWindow::SwitchMeshVisibility() {

	for (int i = 0; i < _Models.size(); ++i) {
		_Models[i]->SwitchVisibility();
	}
}

void SimulationWindow::SwitchPause() {
	for (int i = 0; i < _Models.size(); ++i) {
		_Models[i]->SwitchPause();
	}
}

void SimulationWindow::SwitchView() {
	IsPerspective = !IsPerspective;
}

void SimulationWindow::MeshSearcher() {
	if (isClicked && _SelectedModel == nullptr) {
		float minD = minArea;
		for (const auto& m : _Models) {
			int Num = m->GetNum();
			for (int i = 0; i < Num; ++i) {
				glm::vec3 pos = m->GetPos(i);

				glm::vec3 wim = glm::project(pos, _View, _Projection, _ViewPort);
				glm::vec2 mp = glm::vec2{ _ClickedLocation[0],_ClickedLocation[1] };
				glm::vec2 clip = glm::vec2{ wim.x * 2 / size[0] - 1.0f,wim.y * 2 / size[1] - 1.0 };

				float d = glm::distance(mp, clip);
				if (d < minD) {
					_SelectedModel = m;
					_VertPtr = i;
					minD = d;
				}
			}
		}
		//std::cout <<"id is " << _VertPtr << std::endl;
	}
}

void SimulationWindow::MeshContoroller() {
	
	if (isClicked && _SelectedModel != nullptr && _VertPtr != -1) {

		glm::vec3 pos = _SelectedModel->GetPos(_VertPtr);

		glm::vec3 targetScr = glm::vec3(_ScreenPos[0], _ScreenPos[1], _ScreenClipPos.z);
		glm::vec3 targetPos = glm::unProject(targetScr,_View,_Projection,_ViewPort);

		_SelectedModel->SetCoordinate(_VertPtr,targetPos);

	}
}

void SimulationWindow::MeshTargetClear() {
	if (isClicked == false && _SelectedModel != nullptr) {
		_SelectedModel = nullptr;
		_VertPtr = -1;
	}

}

void SimulationWindow::SetScreenClipPos() {

	if (isClicked && _SelectedModel != nullptr && _VertPtr != -1) {

		glm::vec3 pos = _SelectedModel->GetPos(_VertPtr);
		_ScreenClipPos = glm::project(pos, _View, _Projection, _ViewPort);


	}
}

void SimulationWindow::UpdateMVP() {

	_View = glm::lookAt(
		_CameraPos, // カメラの位置
		_CameraCenter, // カメラの視点
		glm::vec3(0, 1, 0)  // カメラの頭の方向
	);

	if (IsPerspective) {
		_Projection = glm::perspectiveFov(glm::radians(45.0f), size[0],size[1], 0.1f, 10.0f);
	}
	else {
		_Projection = glm::ortho(-1.0*aspect, 1.0*aspect, -1.0, 1.0,0.1,10.0);
	}

	_MVP = _Projection * _View * _Model;
}

void SimulationWindow::UpdateMousePos() {

	if (_IsMid) {
		float prex = _PreMid[0]; float prey = _PreMid[1];	
		GetCursorPos(_CurMid[0], _CurMid[1]);
		Revolution(prex,prey);
	}
	GetCursorPos(_PreMid[0], _PreMid[1]);

}

void SimulationWindow::Revolution(float prex, float prey) {
	auto u = glm::normalize(_Up);
	auto w = glm::normalize(glm::cross(u, _CameraPos));

	float theta_u = -(_CurMid[0] - prex) * _Speed;
	auto q = glm::angleAxis(glm::radians(theta_u), u);
	auto tmp_pos = q * _CameraPos * glm::conjugate(q);

	const float theta_w = (_CurMid[1] - prey) * _Speed;
	q = glm::angleAxis(glm::radians(theta_w), w);
	auto p = glm::conjugate(q);
	tmp_pos = q * tmp_pos * p;

	_CameraPos = tmp_pos;
	//_Up = q * _Up* p;
}

void SimulationWindow::test() {

	std::pair<int, int> asp = CVInterface::GetAspect();
	SetSize(asp.first, asp.second);

	//CVInterface::UseInterface();
	cv::Mat back = CVInterface::GetTexture(0);
	if (back.empty()) {
		//std::cout << "back data is empty." << std::endl;
		return;
	}

	Texture* t_back = new Texture(_Shader->GetShaderID());
	t_back->SetShader(back.rows,back.cols,back.data);



	std::vector<cv::Point> corner = CVInterface::GetCorner();
	std::vector<cv::Point> boundary = CVInterface::GetBoundary();
	std::vector<glm::vec3> vert;
	std::vector<float> uv;
	std::vector<uint> idx;
	std::vector<uint> wireIdx;

	
	for (int i = 0; i < 4; ++i) {
		cout << "corner " << corner[i].x << " " << corner[i].y << endl;
	}
	for (auto& p : boundary) {
		cout << "Boudary " << p.x << " " << p.y << endl;
	}
	

	MeshCreator MC;

	MC.CreateBackGround_NoWall(corner, boundary, vert, uv, idx, wireIdx);

	Mesh* m = new Mesh();
	m->InsertMeshData(vert, uv, idx, wireIdx);
	m->LinkShader(_Shader, _WireShader);
	m->LinkTexture(t_back);


	_BackGround = m;
	_Textures.push_back(t_back);

	std::cout << "Mesh created" << std::endl;

}

void SimulationWindow::test_pbd() {
	for (int i = 0; i < 1; ++i) {
		CVInterface::UseInterface();

		
		cv::Mat fore = CVInterface::GetTexture(2);
		float meshSize = CVInterface::GetMeshRatio();
		std::cout << "MeshRatioSize  " << meshSize << std::endl;

		Texture* t = new Texture(_Shader->GetShaderID());
		t->SetShader(fore.rows, fore.cols, fore.data);

		std::vector<std::vector<cv::Point>> contour = CVInterface::GetContour();
		std::vector<cv::Vec4i> hierarchy = CVInterface::GetHierarchy();

		_Textures.push_back(t);

		for (int con = 0; con < contour.size(); ++con) {
			if (hierarchy[con][3] != -1)continue;
			std::vector<cv::Point> outer = contour[con];
			if (outer.size() < 3)continue;

			std::vector<std::vector<cv::Point>> innerConts;
			for (int con2=0; con2 < contour.size(); ++con2) {
				if (hierarchy[con2][3] == con && contour[con2].size()>3) {
					innerConts.push_back(contour[con2]);
				}
			}
			SimulationModel* sm = new SimulationModel(outer, innerConts, _Shader, _WireShader, t,meshSize);
			_Models.push_back(sm);

		}

		/*
		for (const auto& cont : contour) {

			std::vector<std::vector<cv::Point>> innerConts;
			SimulationModel* sm = new SimulationModel(cont,innerConts,_Shader, _WireShader, t);

			_Models.push_back(sm);

		}
		*/
		
	}
}

void SimulationWindow::GetScreenPos(float& x,float& y) {
	double X, Y;
	glfwGetCursorPos(window, &X, &Y);

	x = X;
	y = size[1] - Y;


}
