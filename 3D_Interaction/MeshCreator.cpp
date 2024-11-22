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
	{
		float uvx = float(cor[0].x) / targetSize;
		float uvy = float(cor[0].y) / targetSize;
		vert.push_back({uvx, uvy, 0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}
	{
		float uvx = float(cor[2].x) / targetSize;
		float uvy = float(cor[2].y) / targetSize;
		vert.push_back({ uvx, uvy, 0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

	for (int i = 0; i < boundary.size(); ++i) {
		{
			float uvx = float(boundary[i].x )/ targetSize;
			float uvy = 0.0;
			vert.push_back({ uvx, uvy, 0 });
			uv.push_back(uvx);
			uv.push_back(uvy);
		}
		{
			float uvx = float(boundary[i].x) / targetSize;
			float uvy = float(boundary[i].y) / targetSize;
			vert.push_back({ uvx, uvy, 0 });
			uv.push_back(uvx);
			uv.push_back(uvy);
		}

	}
	//rightup
	{
		float uvx = float(cor[1].x) / targetSize;
		float uvy = float(cor[1].y )/ targetSize;
		vert.push_back({ uvx, uvy, 0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}
	{
		float uvx = float(cor[3].x) / targetSize;
		float uvy = float(cor[3].y) / targetSize;
		vert.push_back({ uvx, uvy, 0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

	for (int i = 3; i < vert.size(); i+=2) {
		int leftup = i - 3;
		int leftdown = i - 2;
		int rightup = i - 1;
		int rightdown = i;

		idx.push_back(leftup); idx.push_back(leftdown); idx.push_back(rightup);
		idx.push_back(rightup); idx.push_back(leftdown); idx.push_back(rightdown);

		wireIdx.push_back(leftup); wireIdx.push_back(leftdown);
		wireIdx.push_back(leftdown); wireIdx.push_back(rightdown);
		wireIdx.push_back(rightdown); wireIdx.push_back(rightup);
		wireIdx.push_back(rightup); wireIdx.push_back(leftup);
		wireIdx.push_back(leftdown); wireIdx.push_back(rightup);

	}
}

void MeshCreator::CalcCord(float& x, float& y, float& z) {
	z = 0;
}