#include"MeshCreator.h"

MeshCreator::MeshCreator() {

}
MeshCreator::~MeshCreator() {

}

void MeshCreator::CreateBackGround(std::vector<cv::Point> cor, std::vector<cv::Point> boundary,
	std::vector<glm::vec3>& vert, std::vector<float>& uv, std::vector<int>& idx, std::vector<int>& wireIdx) 
{
	float targetSize = 512.0;

	vert.clear();
	uv.clear();
	idx.clear();
	wireIdx.clear();

	//0,leftup
	vert.push_back({ cor[0].x, cor[0].y, 0 });
	{
		float uvx = cor[0].x / targetSize;
		float uvy = cor[0].y / targetSize;
		uv.push_back(uvx);
		uv.push_back(uvy);
	}
	vert.push_back({ cor[2].x,cor[2].y, 0 });
	{
		float uvx = cor[2].x / targetSize;
		float uvy = cor[2].y / targetSize;
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

	for (int i = 0; i < boundary.size(); ++i) {
		vert.push_back({ boundary[i].x,0,0 });//up
		{
			float uvx = boundary[i].x / targetSize;
			float uvy = 0.0;
			uv.push_back(uvx);
			uv.push_back(uvy);
		}
		vert.push_back({ boundary[i].x,boundary[i].y,0 });
		{
			float uvx = boundary[i].x / targetSize;
			float uvy = boundary[i].y / targetSize;
			uv.push_back(uvx);
			uv.push_back(uvy);
		}

	}
	//rightup
	vert.push_back({ cor[1].x, cor[1].y, 0 });
	{
		float uvx = cor[1].x / targetSize;
		float uvy = cor[1].y / targetSize;
		uv.push_back(uvx);
		uv.push_back(uvy);
	}
	vert.push_back({ cor[3].x,cor[3].y, 0 });
	{
		float uvx = cor[3].x / targetSize;
		float uvy = cor[3].y / targetSize;
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

	for (int i = 3; i < vert.size(); i+=2) {
		int leftup = i - 3;
		int leftdown = i - 2;
		int rightup = i - 1;
		int rightdown = i;

		idx.push_back(leftup); idx.push_back(leftdown); idx.push_back(rightup);
		idx.push_back(rightup); idx.push_back(rightdown); idx.push_back(rightdown);

		wireIdx.push_back(leftup); wireIdx.push_back(leftdown);
		wireIdx.push_back(leftdown); wireIdx.push_back(rightdown);
		wireIdx.push_back(rightdown); wireIdx.push_back(rightup);
		wireIdx.push_back(rightup); wireIdx.push_back(leftup);
		wireIdx.push_back(leftdown); wireIdx.push_back(rightup);

	}
}

void MeshCreator::CalcCord(int& x, int& y, int& z) {
	z = 0;
}