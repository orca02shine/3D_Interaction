#include"CVInterface.h"

std::string CVInterface::_FilePath;
std::string CVInterface::_FileName;

std::string CVInterface::WinName = "Input Image";
int CVInterface::IsClicked = 0;
int CVInterface::TargetSize = 512;
cv::Point CVInterface::PrePos = cv::Point(0, 0);
cv::Point CVInterface::ClickedPos = cv::Point(0, 0);
std::vector<cv::Point> CVInterface:: BoundaryPoint;
std::vector<cv::Point> CVInterface::Corners(4);
cv::Mat CVInterface::Img;
cv::Mat CVInterface::Img_Roi;
cv::Mat CVInterface::Mask_FP;
cv::Mat CVInterface::Mask_BP;
cv::Mat CVInterface::Mask_Constraint;
cv::Mat CVInterface::Result_Back;
cv::Mat CVInterface::Result_Fore;

std::vector< std::vector<cv::Point>> CVInterface::Contours;
std::vector<cv::Vec4i>CVInterface::Hierarchy;

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

	cv::Mat img = cv::imread(_FilePath, cv::IMREAD_UNCHANGED);


	return img;


}

void CVInterface::OnMouse(int event, int x, int y, int flags, void*) {
	//IsClicked,0:no,1:left,2:right,3:middle;

	vector<cv::Point> ar = { cv::Point(x,y),PrePos };
	vector<cv::Point> li = { cv::Point(x,y),ClickedPos };
	int brushSize_S = 2;
	int brushSize_L = 10;

	cv::Mat buffer = Img_Roi.clone();;

	switch (event) {
	case cv::EVENT_LBUTTONDOWN:
		if(IsClicked==0){
			IsClicked = 1;
			cv::circle(Img_Roi, { x,y }, brushSize_S, { 0,0,255 }, -1);
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
			cv::circle(Img_Roi, { x,y }, brushSize_L, { 255,0,0 }, -1);
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
			cv::circle(Img_Roi, { x,y }, brushSize_S, { 255,255,255 }, -1);
			cv::circle(Mask_Constraint, { x,y }, brushSize_S, 255, -1);
			ClickedPos = cv::Point(x, y);

			BoundaryPoint.push_back(cv::Point(x, y));

			buffer = Img_Roi.clone();
		}
		else if (IsClicked==3) {
			cv::circle(Img_Roi, { x,y }, brushSize_S, { 255,255,255 }, -1);
			cv::circle(Mask_Constraint, { x,y }, brushSize_S, 255, -1);
			cv::polylines(Img_Roi, li, false, { 255,255,255 }, brushSize_S*2);
			cv::polylines(Mask_Constraint, li, false, 255, brushSize_S*2);
			ClickedPos = cv::Point(x, y);

			BoundaryPoint.push_back(cv::Point(x, y));

			buffer = Img_Roi.clone();
		}
		else {
			IsClicked = 0;
		}
		cout <<"x,y " << x << " " << y << endl;

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
			cv::polylines(Img_Roi,ar,false, { 0,0,255 },brushSize_S*2);
			cv::polylines(Mask_FP, ar, false, 255, brushSize_S*2);
		}
		else if (IsClicked==2) {
			cv::polylines(Img_Roi, ar, false, { 255,0,0 }, brushSize_L*2);
			cv::polylines(Mask_BP, ar, false, 255, brushSize_L*2);
		}
		else if (IsClicked == 3) {
			buffer = Img_Roi.clone();
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

	cv::Mat loadImg =LoadImg();


	Roi(loadImg,Img_Roi);
	Mask_FP= cv::Mat::zeros(Img_Roi.rows, Img_Roi.cols, CV_8UC1);
	Mask_BP= cv::Mat::zeros(Img_Roi.rows, Img_Roi.cols, CV_8UC1);
	Mask_Constraint= cv::Mat::zeros(Img_Roi.rows, Img_Roi.cols, CV_8UC1);


	cv::Mat src = Img_Roi.clone();
	cv::Mat result = Img_Roi.clone();
	MSProc.SetupLabelST(src);

	cv::Mat con = Img_Roi.clone();
	MakeContour(con);

	cv::Mat Back = Img.clone();
	cv::cvtColor(Back, Back, cv::COLOR_BGRA2RGBA);


	cv::namedWindow(WinName, cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback(WinName, OnMouse, 0);
	cv::imshow(WinName, Img_Roi);
	while(Loop()){}

	/*debug
	for (int i = 0; i < 4; ++i) {
		cout << "corner " << Corners[i].x << " " << Corners[i].y << endl;
	}
	for (auto& p : BoundaryPoint) {
		cout << "Boudary " << p.x << " " << p.y << endl;
	}
	*/

	cv::cvtColor(result, result, cv::COLOR_BGRA2BGR);

	/*
	MSProc.SetMask(Mask_FP,Mask_BP);
	Clustering(src);
	MSProc.ShowLabelST(result);
	*/


	/*
	PatchMatch pm;
	pm.image_complete(result, Mask_BP, Mask_Constraint);
	*/


	//cv::flip(Back, Back, 0);
	Result_Back = Back;
	//Result_Fore = Roi(Img);

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

void CVInterface::Roi(cv::Mat img, cv::Mat &roi) {
	int targetSize = TargetSize;


	cv:Mat loadMat = img.clone();
	Img= cv::Mat::zeros(cv::Size(targetSize, targetSize), CV_8UC4);
	
	if (loadMat.channels() < 4) {
		cv::cvtColor(loadMat, loadMat, cv::COLOR_BGR2BGRA);
	}
	

	float w = loadMat.rows;
	float h = loadMat.cols;

	float asp = std::max(w, h);
	float ratio = (double)targetSize / asp;

	cv::resize(loadMat, loadMat, cv::Size(), ratio, ratio, cv::INTER_NEAREST);

	/*
	roi=cv::Mat(Img, cv::Rect((targetSize - loadMat.cols) / 2, (targetSize - loadMat.rows) / 2,
		loadMat.cols, loadMat.rows));
	*/

	roi = cv::Mat(Img, cv::Rect(0, 0,
		loadMat.cols, loadMat.rows));

	loadMat.copyTo(roi);

	int x = Img.rows;
	int y = Img.cols;
	int alpha = 3;

	for (int i = 0; i < Img.rows; ++i) {
		int val = Img.data[0 * Img.step + i * Img.elemSize() + alpha];
		if (val == 0) {
			x = i;
			break;
		}
	}
	for (int i = 0; i < Img.cols; ++i) {
		int val = Img.data[i * Img.step + 0 * Img.elemSize() + alpha];
		if (val == 0) {
			y = i;
			break;
		}
	}
	Corners[0] = cv::Point(0, 0);
	Corners[1] = cv::Point(x, 0);
	Corners[2] = cv::Point(0, y);
	Corners[3] = cv::Point(x, y);




}

void CVInterface::MakeContour(cv::Mat &img) {
	Contours.clear();

	if (img.channels() < 4) {
		cv::cvtColor(img, img, cv::COLOR_BGR2BGRA);
	}
	int alpha = 3;
	cv::Mat gray = img.clone();
	cv::cvtColor(gray, gray, cv::COLOR_BGRA2GRAY);

	for (int y = 0; y < img.cols; y++) {
		for (int x = 0; x < img.rows; x++) {
			int val = img.data[y * img.step + x * img.elemSize() + alpha];
			if (val != 0) {
				gray.data[y * gray.step + x * gray.elemSize()] = 255;
			}
			else {
				for (int c = 0; c < 3; c++) {
					//img.data[y * img.step + x * img.elemSize() + c]=0;
				}
			}
		}
	}

	cv::threshold(gray, gray, 0, 255, cv::THRESH_BINARY);
	cv::blur(gray, gray, cv::Size(3, 3));

	cv::findContours(gray, Contours, Hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_TC89_L1);

	double approxDist = 8.0;//近似精度
	for (auto& con : Contours) {
		cv::approxPolyDP(con, con, approxDist, true);
	}

	cv::Mat showImg = img.clone();
	cv::drawContours(showImg, Contours, -1, cv::Scalar(255, 100, 200), 2);

	//cv::flip(showImg, showImg, 0);
	//cv::cvtColor(showImg, showImg, cv::COLOR_RGBA2BGRA);
	//cv::imshow("aaa", showImg);

}

cv::Mat CVInterface::GetTexture(int i) {
	if (i == 0) {
		return Result_Back;
	}
	else if (i==1) {
		return Result_Fore;
	}
	else {
		int targetSize = 1024;
		return cv::Mat::zeros(cv::Size(targetSize, targetSize), CV_8UC4);
	}

}

std::vector<cv::Point> CVInterface::GetBoundary(){

	if (BoundaryPoint.size() == 0) {
		return BoundaryPoint;
	}

	if (BoundaryPoint[0].x > BoundaryPoint[BoundaryPoint.size() - 1].x) {
		std::reverse(BoundaryPoint.begin(), BoundaryPoint.end());
	}

	return BoundaryPoint;
}

std::vector<cv::Point> CVInterface::GetCorner() {
	return Corners;
}

std::vector<cv::Point> CVInterface::GetContour() {
	return Contours[0];
}