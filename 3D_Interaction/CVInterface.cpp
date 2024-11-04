#include"CVInterface.h"

std::string CVInterface::WinName = "Input Image";

void CVInterface::OnMouse(int event, int x, int y, int flags, void*) {

	switch (event) {
	case cv::EVENT_LBUTTONDOWN:

		cout << "Now Botton is" << x << " " << y << endl;

		break;

	case cv::EVENT_RBUTTONDOWN:

		break;

	case cv::EVENT_LBUTTONUP:

		break;

	case cv::EVENT_RBUTTONUP:

		break;

	case cv::EVENT_MOUSEMOVE:

		break;
	}

}

bool CVInterface::Loop() {


	int key = cv::waitKey();

	return !(key == 27);
}

void CVInterface::UseInterface() {

	cv::namedWindow(WinName, cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback(WinName, OnMouse, 0);

	Loader loader;

	Mat img = loader.LoadImage();

	imshow(WinName, img);

	while(Loop()){}

	Clustering(img);


}

void CVInterface::Clustering(cv::Mat img) {


	cvtColor(img, img, cv::COLOR_RGB2Lab);

	// Initilize Mean Shift with spatial bandwith and color bandwith
	MeanShift MSProc(8, 16);
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