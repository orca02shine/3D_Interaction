#include "Loader.h"

Loader::Loader(){
}
Loader:: ~Loader() {

}



bool Loader::FileOpen(std::string& sSelectedFile, std::string& sFilePath)
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

void Loader::LoadImage() {

	FileOpen(_FileName, _FilePath);

	cv::Mat Img= cv::imread(_FilePath, cv::IMREAD_UNCHANGED);

	//resize(Img, Img, cv::Size(512, 512), 0, 0, 1);

	cv::imshow("Input Image", Img);

	cvtColor(Img, Img, cv::COLOR_RGB2Lab);

	// Initilize Mean Shift with spatial bandwith and color bandwith
	MeanShift MSProc(8, 16);
	// Filtering Process
	MSProc.MSFiltering(Img);
	// Segmentation Process include Filtering Process (Region Growing)
//	MSProc.MSSegmentation(Img);

	// Print the bandwith
	cout << "the Spatial Bandwith is " << MSProc.hs << endl;
	cout << "the Color Bandwith is " << MSProc.hr << endl;

	// Convert color from Lab to RGB
	cvtColor(Img, Img, cv::COLOR_Lab2RGB);

	// Show the result image
	namedWindow("MS Picture");
	imshow("MS Picture", Img);


}