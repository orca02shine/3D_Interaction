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
cv::Mat CVInterface::Img_Clone;
cv::Mat CVInterface::Img;
cv::Mat CVInterface::Img_Roi;
cv::Mat CVInterface::Mask_FP;
cv::Mat CVInterface::Mask_BP;
cv::Mat CVInterface::Mask_Constraint;
cv::Mat CVInterface::Mask_Patch;
cv::Mat CVInterface::Mask_GC;
cv::Mat CVInterface::BgModel;
cv::Mat CVInterface::FgModel;
cv::Mat CVInterface::Result_Back;
cv::Mat CVInterface::Result_Fore;
cv::Mat CVInterface::Result_ForeFlip;
cv::Mat CVInterface::Result_TexAtlas;

std::vector< std::vector<cv::Point>> CVInterface::Contours;
std::vector<cv::Vec4i>CVInterface::Hierarchy;

std::pair<int, int> CVInterface::TexSize;
float CVInterface::MeshRatio=1.0f;
std::pair<int, int> CVInterface::Ofs = { 0,0 };

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
	int brushSize_L = 5;

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

			BoundaryPoint.push_back(cv::Point(x+Ofs.first, y+Ofs.second));

			buffer = Img_Roi.clone();
		}
		else if (IsClicked==3) {
			cv::circle(Img_Roi, { x,y }, brushSize_S, { 255,255,255 }, -1);
			cv::circle(Mask_Constraint, { x,y }, brushSize_S, 255, -1);
			cv::polylines(Img_Roi, li, false, { 255,255,255 }, brushSize_S*2);
			cv::polylines(Mask_Constraint, li, false, 255, brushSize_S*2);
			ClickedPos = cv::Point(x, y);

			BoundaryPoint.push_back(cv::Point(x+Ofs.first, y+Ofs.second));

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

	if (key == 'n') {
		GrabCut(Img_Clone, Result_Fore, Result_Back);
		return true;
	}

	return !(key == 27);
}

void CVInterface::UseInterface() {

	cv::Mat loadImg =LoadImg();

	float wi = loadImg.cols;
	float he = loadImg.rows;
	float as = 1.0f;
	if (wi < 1024 || he < 1024) {
		float mi = min(wi, he);
		as = 1024.0 / mi;
		wi = wi * as;
		he = he * as;
	}
	MeshRatio = std::max(wi, he) / std::min(wi, he);
	TexSize = { (int)wi,(int)he};

	Roi(loadImg,Img_Roi,Img);
	Mask_FP= cv::Mat::zeros(Img_Roi.rows, Img_Roi.cols, CV_8UC1);
	Mask_BP= cv::Mat::zeros(Img_Roi.rows, Img_Roi.cols, CV_8UC1);
	Mask_Constraint= cv::Mat::zeros(Img_Roi.rows, Img_Roi.cols, CV_8UC1);

	Img_Clone = Img_Roi.clone();
	//MSProc.SetupLabelST(src);


	cv::namedWindow(WinName, cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback(WinName, OnMouse, 0);
	cv::imshow(WinName, Img_Roi);

	//Result_Back= cv::Mat::zeros(cv::Size(Img.rows, Img.cols), CV_8UC4);;

	if (loadImg.channels() == 3) {
		while (Loop()) {}
		cv::cvtColor(Result_Fore, Result_Fore, cv::COLOR_BGRA2RGBA);
		cv::cvtColor(Result_Back, Result_Back, cv::COLOR_BGRA2RGBA);

		//patchmatch
		cv::Mat tes = Img_Clone.clone();
		cv::cvtColor(tes, tes, cv::COLOR_BGRA2BGR);
		PatchMatch pm;
		pm.image_complete(tes, Mask_Patch, Mask_Constraint);
		cv::Mat tmpBack = pm.getResult();
		cv::Mat dammy;
		Roi(tmpBack, dammy, Result_Back);
		cv::cvtColor(Result_Back, Result_Back, cv::COLOR_BGRA2RGBA);
	}
	else {
		Result_Fore = Img.clone();
		cv::cvtColor(Result_Fore, Result_Fore, cv::COLOR_BGRA2RGBA);
	}

	MakeContour(Result_Fore);


	/*debug
	for (int i = 0; i < 4; ++i) {
		cout << "corner " << Corners[i].x << " " << Corners[i].y << endl;
	}
	for (auto& p : BoundaryPoint) {
		cout << "Boudary " << p.x << " " << p.y << endl;
	}
	*/



	/*
	MSProc.SetMask(Mask_FP,Mask_BP);
	Clustering(src);
	MSProc.ShowLabelST(result);
	*/
	

	ImpaintFore(Result_Fore);

	MakeAtlas();

	//cv::flip(Back, Back, 0);

}

void CVInterface::GrabCut(cv::Mat src,cv::Mat &fore, cv::Mat &back) {

	cv::Mat im = src.clone();

	if (im.channels() == 4) {
		cv::cvtColor(im, im, cv::COLOR_BGRA2BGR);
	}


	Mask_GC.create(im.size(), CV_8UC1);
	Mask_GC.setTo(cv::Scalar::all(cv::GC_PR_FGD));

	cv::Rect rec= cv::Rect(1, 1, im.cols - 4, im.rows - 4);
	Mask_GC(rec).setTo(cv::Scalar(cv::GC_PR_FGD));

	for (int i = 0; i < Mask_GC.rows; ++i) {
		for (int j = 0; j < Mask_GC.cols; ++j){
			if (Mask_BP.data[i * Mask_BP.step + j * Mask_BP.elemSize()] == 255) {
				Mask_GC.data[i * Mask_GC.step + j * Mask_GC.elemSize()] = GC_BGD;
			}
			if (Mask_FP.data[i * Mask_FP.step + j * Mask_FP.elemSize()] == 255) {
				Mask_GC.data[i * Mask_GC.step + j * Mask_GC.elemSize()] = GC_FGD;
			}
		}
	}

	cv::grabCut(im, Mask_GC, rec, BgModel, FgModel, 1);

	cv::Mat binMask;
	binMask = Mask_GC & 1;
	cv::Mat kernel= cv::Mat::ones(cv::Size(4,4), CV_8UC1);
	cv::morphologyEx(binMask, binMask, cv::MORPH_OPEN, kernel);
	cv::erode(binMask, binMask, kernel);

	
	cv::Mat res_fore= cv::Mat(im.size(), CV_8UC4, cv::Scalar(0, 0, 0, 0));
	cv::Mat res_back = cv::Mat(im.size(), CV_8UC4, cv::Scalar(0, 0, 0, 0));
	cv::cvtColor(im, im, cv::COLOR_BGR2BGRA);
	im.copyTo(res_fore, binMask);

	binMask = ~Mask_GC & 1;
	cv::erode(binMask, binMask, kernel);
	cv::erode(binMask, binMask, kernel);
	im.copyTo(res_back, binMask);

	Mask_Patch = cv::Mat(im.size(), CV_8UC1, cv::Scalar(255));
	cv::Mat tb = cv::Mat(im.size(), CV_8UC1, cv::Scalar(0));
	tb.copyTo(Mask_Patch, binMask);

	cv::Mat dammy,dammy2;

	Roi(res_fore, dammy, fore);
	Roi(res_back, dammy2, back);


	cv::imshow("fore", res_fore);
	cv::imshow("back", res_back);

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

void CVInterface::Roi(cv::Mat img, cv::Mat &roi, cv::Mat &resizedImg) {
	int targetSize = TargetSize;


	cv:Mat loadMat = img.clone();
	resizedImg= cv::Mat::zeros(cv::Size(targetSize, targetSize), CV_8UC4);
	
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

	int ofsx = (targetSize - loadMat.cols) / 2;
	int ofsy = (targetSize - loadMat.rows) / 2;

	roi = cv::Mat(resizedImg, cv::Rect(ofsx, ofsy,
		loadMat.cols, loadMat.rows));

	loadMat.copyTo(roi);

	Ofs = { ofsx,ofsy };
	/*
	int x = resizedImg.rows;
	int y = resizedImg.cols;
	int alpha = 3;
	for (int i = 0; i < resizedImg.cols; ++i) {
		int val = resizedImg.data[ofsy * resizedImg.step + i * resizedImg.elemSize() + alpha];
		if (val == 0) {
			x = i;
			break;
		}
	}
	for (int i = 0; i < resizedImg.rows; ++i) {
		int val = resizedImg.data[i * resizedImg.step + ofsx * resizedImg.elemSize() + alpha];
		if (val == 0) {
			y = i;
			break;
		}
	}
	*/
	Corners[0] = cv::Point(ofsx, ofsy);
	Corners[1] = cv::Point(ofsx+loadMat.cols, ofsy);
	Corners[2] = cv::Point(ofsx, ofsy+loadMat.rows);
	Corners[3] = cv::Point(ofsx+loadMat.cols, ofsy+loadMat.rows);


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
			if (val > 127) {
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

	for (auto& con : Contours) {
		double approxDist = 0.004*cv::arcLength(con, true);//近似精度
		cv::approxPolyDP(con, con, approxDist, true);
	}

	cv::Mat showImg = img.clone();
	cv::drawContours(showImg, Contours, -1, cv::Scalar(255, 100, 200), 6);

	//cv::flip(showImg, showImg, 0);
	cv::cvtColor(showImg, showImg, cv::COLOR_RGBA2BGRA);
	cv::imshow("aaa", showImg);

}

void CVInterface::ImpaintFore(cv::Mat& img) {
	if (img.channels() < 4) {
		return;
	}

	cv::Mat work = img;

	for (int y = 0; y < img.cols; y++) {
		for (int x = 0; x < img.rows; x++) {
			if (img.data[y * img.step + x * img.elemSize() + 3] != 255) {
				img.data[y * img.step + x * img.elemSize() + 3] = 0;
			}
		}
	}


	for (int y = 0; y < img.cols; y++) {
		bool flag = false;
		for (int x = 0; x < img.rows; x++) {
			int val = img.data[y * img.step + x * img.elemSize() + 3];
			if (val == 255 && flag == false) {
				flag = true;
				int col0 = img.data[y * img.step + x * img.elemSize() + 0];
				int col1 = img.data[y * img.step + x * img.elemSize() + 1];
				int col2 = img.data[y * img.step + x * img.elemSize() + 2];
				int col3 = img.data[y * img.step + x * img.elemSize() + 3];

				if (x + 5 < img.rows) {
					col0 = img.data[y * img.step + (x + 5) * img.elemSize() + 0];
					col1 = img.data[y * img.step + (x + 5) * img.elemSize() + 1];
					col2 = img.data[y * img.step + (x + 5) * img.elemSize() + 2];
					col3 = img.data[y * img.step + (x + 5) * img.elemSize() + 3];
				}

				for (int i = x-1; i > 0; i--) {
					if (work.data[y * work.step + i * work.elemSize() + 3] != 255) {
						work.data[y * work.step + i * work.elemSize() + 0] = col0;
						work.data[y * work.step + i * work.elemSize() + 1] = col1;
						work.data[y * work.step + i * work.elemSize() + 2] = col2;
						work.data[y * work.step + i * work.elemSize() + 3] = 255;
					}
					else {
						break;
					}
				}
			}

			else if (val == 0 && flag==true && x>0) {
				flag = false;
				int col0 = img.data[y * img.step + (x-1) * img.elemSize() + 0];
				int col1 = img.data[y * img.step + (x-1) * img.elemSize() + 1];
				int col2 = img.data[y * img.step + (x-1) * img.elemSize() + 2];
				int col3 = img.data[y * img.step + (x-1) * img.elemSize() + 3];

				if (x -5 >=0) {
					col0 = img.data[y * img.step + (x - 5) * img.elemSize() + 0];
					col1 = img.data[y * img.step + (x - 5) * img.elemSize() + 1];
					col2 = img.data[y * img.step + (x - 5) * img.elemSize() + 2];
					col3 = img.data[y * img.step + (x - 5) * img.elemSize() + 3];
				}

				for (int i = x; i < img.rows; i++) {
					if (work.data[y * work.step + i * work.elemSize() + 3] != 255) {
						work.data[y * work.step + i * work.elemSize() + 0] = col0;
						work.data[y * work.step + i * work.elemSize() + 1] = col1;
						work.data[y * work.step + i * work.elemSize() + 2] = col2;
						work.data[y * work.step + i * work.elemSize() + 3] = 255;
					}
					else {
						break;
					}
				}

			}
		}
	}


	for (int x = 0; x < img.rows; x++) {
		bool flag = false;
		for (int y = 0; y < img.cols; y++) {
			int val = img.data[y * img.step + x * img.elemSize() + 3];
			if (val == 255 && flag == false) {
				flag = true;
				int col0 = img.data[y * img.step + x * img.elemSize() + 0];
				int col1 = img.data[y * img.step + x * img.elemSize() + 1];
				int col2 = img.data[y * img.step + x * img.elemSize() + 2];
				int col3 = img.data[y * img.step + x * img.elemSize() + 3];

				for (int i = y-1; i > 0; i--) {
					if (work.data[i * work.step + x * work.elemSize() + 3] != 255) {
						work.data[i * work.step + x * work.elemSize() + 0] = col0;
						work.data[i * work.step + x * work.elemSize() + 1] = col1;
						work.data[i * work.step + x * work.elemSize() + 2] = col2;
						work.data[i * work.step + x * work.elemSize() + 3] = 255;
					}
					else {
						break;
					}
				}
			}

			else if (val == 0 && flag == true && y>0) {
				flag = false;
				int col0 = img.data[(y-1) * img.step + x * img.elemSize() + 0];
				int col1 = img.data[(y-1) * img.step + x * img.elemSize() + 1];
				int col2 = img.data[(y-1) * img.step + x * img.elemSize() + 2];
				int col3 = img.data[(y-1) * img.step + x * img.elemSize() + 3];

				for (int i = y; i < img.cols; i++) {
					if (work.data[i * work.step + x * work.elemSize() + 3] != 255) {
						work.data[i * work.step + x * work.elemSize() + 0] = col0;
						work.data[i * work.step + x * work.elemSize() + 1] = col1;
						work.data[i * work.step + x * work.elemSize() + 2] = col2;
						work.data[i * work.step + x * work.elemSize() + 3] = 255;
					}
					else {
						break;
					}
				}
			}

		}
	}

	//cv::imshow("test1", work);

	cv::Mat fliptex = work.clone();
	cv::medianBlur(fliptex, fliptex, 63);
	Result_ForeFlip = fliptex;

	//imshow("test2", Result_ForeFlip);
}

void CVInterface::MakeAtlas() {
	int targetSize = TargetSize*2;
	cv::Mat atlas= cv::Mat::zeros(cv::Size(targetSize, targetSize), CV_8UC4);
	cv::Rect recRoi1(0, 0, TargetSize, TargetSize);
	cv::Rect recRoi2(TargetSize, 0, TargetSize, TargetSize);
	
	cv::Mat roi1 = atlas(recRoi1);
	cv::Mat roi2 = atlas(recRoi2);
	Result_Fore.copyTo(roi1);
	Result_ForeFlip.copyTo(roi2);

	Result_TexAtlas = atlas;

	//imshow("test4", atlas);
}

cv::Mat CVInterface::GetTexture(int i) {
	if (i == 0) {
		return Result_Back;
	}
	else if (i==1) {
		return Result_Fore;
	}
	else if (i == 2) {
		return Result_TexAtlas;
	}
	else {
		int targetSize = 512;
		cv::Mat k= cv::Mat::zeros(cv::Size(targetSize, targetSize), CV_8UC3);
		cv::cvtColor(k, k, cv::COLOR_BGR2BGRA);

		return k;
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

std::vector<std::vector<cv::Point>> CVInterface::GetContour() {
	return Contours;
}
std::vector<cv::Vec4i> CVInterface::GetHierarchy() {
	return Hierarchy;
}
std::pair<int, int> CVInterface::GetAspect() {
	return TexSize;
}
float CVInterface::GetMeshRatio() {
	return MeshRatio;
}