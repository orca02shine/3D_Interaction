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
	

	//0,leftup to leftdown
	{
		float uvx = float(cor[0].x) / targetSize;
		float uvy = float(cor[0].y) / targetSize;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vy *= -1.0;
		vert.push_back({vx, vy, 0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}
	//2 leftdown to left up
	{
		float uvx = float(cor[2].x) / targetSize;
		float uvy = float(cor[2].y) / targetSize;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vy *= -1.0;
		vert.push_back({ vx, vy, 0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

	for (int i = 0; i < boundary.size(); ++i) {
		{
			float uvx = float(boundary[i].x )/ targetSize;
			float uvy = 0.0;
			float vx = uvx * 2 - 1.0;
			float vy = uvy * 2 - 1.0;
			vy *= -1.0;
			vert.push_back({ vx, vy, 0 });
			uv.push_back(uvx);
			uv.push_back(uvy);
		}
		{
			float uvx = float(boundary[i].x) / targetSize;
			float uvy = float(boundary[i].y) / targetSize;
			float vx = uvx * 2 - 1.0;
			float vy = uvy * 2 - 1.0;
			vy *= -1.0;
			vert.push_back({ vx, vy, 0 });
			uv.push_back(uvx);
			uv.push_back(uvy);
		}

	}
	//rightup
	{
		float uvx = float(cor[1].x) / targetSize;
		float uvy = float(cor[1].y )/ targetSize;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vy *= -1.0;
		vert.push_back({ vx, vy, 0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}
	//rightdown
	{
		float uvx = float(cor[3].x) / targetSize;
		float uvy = float(cor[3].y) / targetSize;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vy *= -1.0;
		vert.push_back({ vx, vy, 0 });
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

	//Delauney-----------------------------------------------------
	//DelId =id*2+1;

	std::vector<cv::Point> contour;
	contour.push_back(cor[2]);
	for (int i = 0; i < boundary.size(); ++i) {
		contour.push_back(boundary[i]);
	}
	contour.push_back(cor[3]);

	if (contour.size() > 2) {
		class Delauney delauney(contour, targetSize);

		std::vector<int> tempIdx = delauney.GetIndices();
		std::vector<int> tempWire = delauney.GetWireFrame();

		for (int i = 0; i < tempIdx.size(); ++i) {
			int id = tempIdx[i];
			idx.push_back(id * 2 + 1);
		}

		for (int i = 0; i < tempWire.size(); ++i) {
			int id = tempWire[i];
			wireIdx.push_back(id * 2 + 1);
		}
	}

	//coordinate xy-----------
	if (contour.size() > 2) {

		float f = 0.01f;
		float wm = 0.1;//wM=1.0;
		float y0 = vert[1].y;
		float yM = -1.0;

		
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 1.0f);
		glm::mat4 view=glm::lookAt(
			glm::vec3(0, 0.0, -0.4),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)  // ÉJÉÅÉâÇÃì™ÇÃï˚å¸
		);

		glm::mat4 test = glm::mat4(1.0);
			
		for (int i = 0; i < contour.size(); ++i) {
			yM = std::max(yM, vert[i * 2 + 1].y);
		}

		for (int i = 0; i < contour.size(); ++i) {
			float xi = vert[i * 2 + 1].x;
			float yi = vert[i * 2 + 1].y;
			
			float wi = ((yi - y0) / (yM - y0)) * wm + 1 - ((yi - y0) / (yM - y0));

			glm::vec4 scr = { xi,yi,f,wi };

			glm::mat4 inv = glm::inverse(view) * glm::inverse(proj);

			glm::vec4 world = inv*scr;

			
			std::cout << "world" << world.x << " " << world.y << " " << world.z <<" " <<world.w <<std::endl;
			vert[i * 2 + 1] = { world.x,world.y,world.z };
			vert[i * 2].x = world.x;
			vert[i * 2].z = world.z;
			vert[i * 2].y = 1.0f;
			

		}



	}



}


void MeshCreator::CreateForeGround(std::vector<cv::Point> cont,
	std::vector<glm::vec3>& vert, std::vector<float>& uv, std::vector<int>& idx, std::vector<int>& wireIdx) {

	float targetSize = 512.0;

	vert.clear();
	uv.clear();
	idx.clear();
	wireIdx.clear();

	std::vector<cv::Point> revcont = cont;

	std::reverse(revcont.begin(), revcont.end());

	for (int i = 0; i <revcont.size(); ++i) {

		float uvx = float(revcont[i].x) / targetSize;
		float uvy = float(revcont[i].y) / targetSize;

		uv.push_back(uvx);
		uv.push_back(uvy);


	}

	if (cont.size() > 2) {
		class Delauney delauney(revcont, targetSize);

		std::vector<glm::vec2> tempVert = delauney.GetVertices();
		std::vector<float> tempUv = delauney.GetUV();
		std::vector<int> tempIdx = delauney.GetIndices();
		std::vector<int> tempWire = delauney.GetWireFrame();

		std::vector<glm::vec3> tempVert3D = delauney.GetVertices3D();
		std::vector<int> tempIdx3D = delauney.GetIndices3D();
		std::vector<int> tempWire3D = delauney.GetWireFrame3D();

		for (int i = 0; i < tempVert.size(); ++i) {
			float x = tempVert[i].x;
			float y = tempVert[i].y;
			vert.push_back({ x,y,0 });
		}
		/*
		for (int i = 0; i < tempVert3D.size(); ++i) {
			vert.push_back(tempVert3D[i]);
		}
		*/
		for (int i = 0; i < tempIdx.size(); ++i) {
			int id = tempIdx[i];
			idx.push_back(id);
		}
		/*
		for (int i = 0; i < tempIdx3D.size(); ++i) {
			int id = tempIdx3D[i];
			idx.push_back(id);
		}
		*/
		for (int i = 0; i < tempWire.size(); ++i) {
			int id = tempWire[i];
			wireIdx.push_back(id);
		}
		/*
		for (int i = 0; i < tempWire3D.size(); ++i) {
			wireIdx.push_back(tempWire3D[i]);
		}
		*/
	}


}

void MeshCreator::CalcCord(float& x, float& y, float& z) {
	z = 0;
}