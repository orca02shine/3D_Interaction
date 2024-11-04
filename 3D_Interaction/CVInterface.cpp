#include"CVInterface.h"

std::string CVInterface::_FilePath;
std::string CVInterface::_FileName;

std::string CVInterface::WinName = "Input Image";
int CVInterface::IsClicked = false;
cv::Point CVInterface::PrePos = cv::Point(0, 0);
cv::Mat CVInterface::Img;

MeanShift CVInterface::MSProc(8, 16);

bool CVInterface::FileOpen(std::string& sSelectedFile, std::string& sFilePath)
{
	wchar_t filePath[MAX_PATH] = { 0 };

	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);		//�\���̂̃T�C�Y
	ofn.hwndOwner = NULL;						//�I�[�i�[�E�B���h�E�̃n���h��
	ofn.lpstrFilter = L"image(*.jpg,*.png)\0*.jpg;*.png\0"; //�g���q�t�B���^�[
	ofn.nFilterIndex = 0;			//�t�B���^�[�̏����l
	ofn.lpstrFile = filePath;		//�I�������t�@�C���p�X���󂯎��o�b�t�@
	ofn.nMaxFile = MAX_PATH;

	if (GetOpenFileName(&ofn)) {
		size_t i;
		char* pa = (char*)malloc(100);
		wcstombs_s(&i, pa, 100, filePath, 99);
		sFilePath = std::string(pa);

		std::cout << sFilePath << std::endl;
	}
	return true;

}

cv::Mat CVInterface::LoadImg() {

	FileOpen(_FileName, _FilePath);

	cv::Mat Img = cv::imread(_FilePath, cv::IMREAD_UNCHANGED);


	return Img;


}

void CVInterface::OnMouse(int event, int x, int y, int flags, void*) {
	//IsClicked,0:no,1:left,2:right,3:middle;

	vector<cv::Point> ar = { cv::Point(x,y),PrePos };

	switch (event) {
	case cv::EVENT_LBUTTONDOWN:
		IsClicked = 1;
		cv::circle(Img, { x,y }, 2, { 0,0,255 },-1);
		MSProc.UpdateLabelST(y, x, 1);
		break;

	case cv::EVENT_RBUTTONDOWN:
		IsClicked = 2;
		cv::circle(Img, { x,y }, 2, { 255,0,0 }, -1);
		MSProc.UpdateLabelST(y, x, 2);
		break;

	case cv::EVENT_LBUTTONUP:
		IsClicked = 0;
		break;

	case cv::EVENT_RBUTTONUP:
		IsClicked = 0;
		break;

	case cv::EVENT_MOUSEMOVE:
		if (IsClicked==1) {
			cv::polylines(Img,ar,false, { 0,0,255 },4);
			MSProc.UpdateLabelST(y, x, 1);
		}
		else if (IsClicked==2) {
			cv::polylines(Img, ar, false, { 255,0,0 }, 4);
			MSProc.UpdateLabelST(y, x, 2);
		}

		break;
	}
	if (IsClicked) {
		PrePos = cv::Point(x, y);
	}

	cv::imshow(WinName, Img);
}

bool CVInterface::Loop() {

	int key = cv::waitKey();

	return !(key == 27);
}

void CVInterface::UseInterface() {

	Img =LoadImg();
	cv::Mat src = Img.clone();
	MSProc.SetupLabelST(src);


	cv::namedWindow(WinName, cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback(WinName, OnMouse, 0);
	cv::imshow(WinName, Img);
	while(Loop()){}



	Clustering(src);

}

void CVInterface::Clustering(cv::Mat img) {


	cvtColor(img, img, cv::COLOR_RGB2Lab);

	// Initilize Mean Shift with spatial bandwith and color bandwith
	// Filtering Process
	MSProc.MSSegmentation(img);
	// Segmentation Process include Filtering Process (Region Growing)
//	MSProc.MSSegmentation(Img);

	// Print the bandwith
	cout << "the Spatial Bandwith is " << MSProc.hs << endl;
	cout << "the Color Bandwith is " << MSProc.hr << endl;

	// Convert color from Lab to RGB
	cvtColor(img, img, cv::COLOR_Lab2RGB);

	// Show the result image
	namedWindow("MS Picture");
	imshow("MS Picture", img);

}