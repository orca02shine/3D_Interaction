#include"CVInterface.h"

std::string CVInterface::_FilePath;
std::string CVInterface::_FileName;

std::string CVInterface::WinName = "Input Image";
int CVInterface::IsClicked = false;
cv::Point CVInterface::PrePos = cv::Point(0, 0);
cv::Mat CVInterface::Img;
cv::Mat CVInterface::Mask_FP;
cv::Mat CVInterface::Mask_BP;
cv::Mat CVInterface::Mask_Constraint;

MeanShift CVInterface::MSProc(8, 16);

bool CVInterface::FileOpen(std::string& sSelectedFile, std::string& sFilePath)
{
	wchar_t filePath[MAX_PATH] = { 0 };

	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAME);		//構造体のサイズ
	ofn.hwndOwner = NULL;						//オーナーウィンドウのハンドル
	ofn.lpstrFilter = L"image(*.jpg,*.png)\0*.jpg;*.png\0"; //拡張子フィルター
	ofn.nFilterIndex = 0;			//フィルターの初期値
	ofn.lpstrFile = filePath;		//選択したファイルパスを受け取るバッファ
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
		cv::circle(Mask_FP, { x,y }, 2,255, -1);
		break;

	case cv::EVENT_RBUTTONDOWN:
		IsClicked = 2;
		cv::circle(Img, { x,y }, 10, { 255,0,0 }, -1);
		cv::circle(Mask_BP, { x,y }, 10, 255, -1);
		break;

	case cv::EVENT_MBUTTONDOWN:
		IsClicked = 3;
		cv::circle(Img, { x,y }, 2, { 255,255,255 }, -1);
		cv::circle(Mask_Constraint, { x,y }, 2, 255, -1);
		break;

	case cv::EVENT_LBUTTONUP:
		IsClicked = 0;
		break;

	case cv::EVENT_RBUTTONUP:
		IsClicked = 0;
		break;

	case cv::EVENT_MBUTTONUP:
		IsClicked = 0;
		break;

	case cv::EVENT_MOUSEMOVE:
		if (IsClicked==1) {
			cv::polylines(Img,ar,false, { 0,0,255 },4);
			cv::polylines(Mask_FP, ar, false, 255, 4);
		}
		else if (IsClicked==2) {
			cv::polylines(Img, ar, false, { 255,0,0 }, 20);
			cv::polylines(Mask_BP, ar, false, 255, 20);
		}
		else if (IsClicked == 3) {
			cv::polylines(Img, ar, false, { 255,255,255}, 4);
			cv::polylines(Mask_Constraint, ar, false, 255, 4);
		}
		break;
	}
	if (IsClicked>0) {
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
	Mask_FP= cv::Mat::zeros(Img.rows, Img.cols, CV_8UC1);
	Mask_BP= cv::Mat::zeros(Img.rows, Img.cols, CV_8UC1);
	Mask_Constraint= cv::Mat::zeros(Img.rows, Img.cols, CV_8UC1);


	cv::Mat src = Img.clone();
	cv::Mat result = Img.clone();
	MSProc.SetupLabelST(src);


	cv::namedWindow(WinName, cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback(WinName, OnMouse, 0);
	cv::imshow(WinName, Img);
	while(Loop()){}

	//MSProc.SetMask(Mask_FP,Mask_BP);
	//Clustering(src);
	//MSProc.ShowLabelST(result);

	PatchMatch pm;
	//pm.image_complete(result, Mask_BP, Mask_Constraint);

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