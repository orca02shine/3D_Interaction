#include"CVInterface.h"

std::string CVInterface::_FilePath;
std::string CVInterface::_FileName;

std::string CVInterface::WinName = "Input Image";
int CVInterface::IsClicked = false;
cv::Point CVInterface::PrePos = cv::Point(0, 0);
cv::Point CVInterface::ClickedPos = cv::Point(0, 0);
cv::Mat CVInterface::Img;
cv::Mat CVInterface::Mask_FP;
cv::Mat CVInterface::Mask_BP;
cv::Mat CVInterface::Mask_Constraint;
cv::Mat CVInterface::Result_Back;
cv::Mat CVInterface::Result_Fore;

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
	vector<cv::Point> li = { cv::Point(x,y),ClickedPos };
	int brushSize_S = 2;
	int brushSize_L = 10;

	cv::Mat buffer = Img.clone();;

	switch (event) {
	case cv::EVENT_LBUTTONDOWN:
		if(IsClicked==0){
			IsClicked = 1;
			cv::circle(Img, { x,y }, brushSize_S, { 0,0,255 }, -1);
			cv::circle(Mask_FP, { x,y }, brushSize_S, 255, -1);
			//buffer = Img.clone();
		}
		else {
			IsClicked = 0;
		}
		break;

	case cv::EVENT_RBUTTONDOWN:
		if (IsClicked == 0) {
			IsClicked = 2;
			cv::circle(Img, { x,y }, brushSize_L, { 255,0,0 }, -1);
			cv::circle(Mask_BP, { x,y }, brushSize_L, 255, -1);
			//buffer = Img.clone();
		}
		else {
			IsClicked = 0;
		}
		break;

	case cv::EVENT_MBUTTONDOWN:
		if (IsClicked == 0) {
			IsClicked = 3;
			cv::circle(Img, { x,y }, brushSize_S, { 255,255,255 }, -1);
			cv::circle(Mask_Constraint, { x,y }, brushSize_S, 255, -1);
			ClickedPos = cv::Point(x, y);

			buffer = Img.clone();
		}
		else if (IsClicked==3) {
			cv::circle(Img, { x,y }, brushSize_S, { 255,255,255 }, -1);
			cv::circle(Mask_Constraint, { x,y }, brushSize_S, 255, -1);
			cv::polylines(Img, li, false, { 255,255,255 }, brushSize_S*2);
			cv::polylines(Mask_Constraint, li, false, 255, brushSize_S*2);
			ClickedPos = cv::Point(x, y);

			buffer = Img.clone();
		}
		else {
			IsClicked = 0;
		}
		break;

	case cv::EVENT_LBUTTONUP:
		IsClicked = 0;
		break;

	case cv::EVENT_RBUTTONUP:
		IsClicked = 0;
		break;

	case cv::EVENT_MBUTTONUP:

		break;

	case cv::EVENT_MOUSEMOVE:
		if (IsClicked==1) {
			cv::polylines(Img,ar,false, { 0,0,255 },brushSize_S*2);
			cv::polylines(Mask_FP, ar, false, 255, brushSize_S*2);
		}
		else if (IsClicked==2) {
			cv::polylines(Img, ar, false, { 255,0,0 }, brushSize_L*2);
			cv::polylines(Mask_BP, ar, false, 255, brushSize_L*2);
		}
		else if (IsClicked == 3) {
			buffer = Img.clone();
			cv::polylines(buffer, li, false, { 255,255,255 }, brushSize_S*2);
		}
		break;
	}
	if (IsClicked>0) {
		PrePos = cv::Point(x, y);
	}

	cv::imshow(WinName, buffer);
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

	//PatchMatch pm;
	//pm.image_complete(result, Mask_BP, Mask_Constraint);

	Result_Back = Roi(Img);
	Result_Fore = Roi(Img);

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

cv::Mat CVInterface::Roi(cv::Mat img) {
	int targetSize = 512;


	cv:Mat loadMat = img.clone();
	cv::Mat workMat= cv::Mat::zeros(cv::Size(targetSize, targetSize), CV_8UC4);

	if (loadMat.channels() < 4) {
		cv::cvtColor(loadMat, loadMat, cv::COLOR_BGR2RGBA);
	}


	float w = loadMat.rows;
	float h = loadMat.cols;

	float asp = std::max(w, h);
	float ratio = (double)targetSize / asp;

	cv::resize(loadMat, loadMat, cv::Size(), ratio, ratio, cv::INTER_NEAREST);

	cv::Mat Roi1(workMat, cv::Rect((targetSize - loadMat.cols) / 2, (targetSize - loadMat.rows) / 2,
		loadMat.cols, loadMat.rows));

	loadMat.copyTo(Roi1);

	cv::Mat ret = workMat.clone();

	cv::flip(ret, ret, 0);

	return ret;

}

cv::Mat CVInterface::GetTexture(int i) {
	if (i == 0) {
		return Result_Back;
	}
	else if (i==1) {
		return Result_Fore;
	}
	else {
		int targetSize = 512;
		return cv::Mat::zeros(cv::Size(targetSize, targetSize), CV_8UC4);
	}

}