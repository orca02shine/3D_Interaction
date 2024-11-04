#include "Loader.h"

/*
Loader::Loader(){
}
Loader:: ~Loader() {

}
*/

std::string Loader::_FilePath;
std::string Loader::_FileName;

bool Loader::FileOpen(std::string& sSelectedFile, std::string& sFilePath)
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

cv::Mat Loader::LoadImg() {

	FileOpen(_FileName, _FilePath);

	cv::Mat Img= cv::imread(_FilePath, cv::IMREAD_UNCHANGED);


	return Img;


}