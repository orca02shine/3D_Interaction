#include"MeshCreator.h"

MeshCreator::MeshCreator() {

}
MeshCreator::~MeshCreator() {

}

void MeshCreator::CreateBackGround(std::vector<cv::Point> cor, std::vector<cv::Point> boundary,
	std::vector<glm::vec3>& vert, std::vector<float>& uv, std::vector<uint>& idx, std::vector<uint>& wireIdx)
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
		vert.push_back({ vx, vy, 0 });
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
			float uvx = float(boundary[i].x) / targetSize;
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
		float uvy = float(cor[1].y) / targetSize;
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

	for (int i = 3; i < vert.size(); i += 2) {
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
		delauney.MakePolygonData_2D();

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

	for (int i = 0; i < vert.size(); ++i) {
		vert[i].z =2.0f;
	}

	
	for (int i = 0; i < vert.size(); ++i) {
		vert[i].x = vert[i].x * -1.0;
	}
	
	/*
	//coordinate xy-----------
	if (contour.size() > 2) {

		
	
		float f = 0.01f;
		float wm = 0.1;//wM=1.0;
		float y0 = vert[1].y;
		float yM = -1.0;


		glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 1.0f);
		glm::mat4 view = glm::lookAt(
			glm::vec3(0, 0.0, -4.0),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)  // カメラの頭の方向
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

			glm::vec4 world = inv * scr;


			std::cout << "world" << world.x << " " << world.y << " " << world.z << " " << world.w << std::endl;
			vert[i * 2 + 1] = { world.x,world.y,world.z };
			vert[i * 2].x = world.x;
			vert[i * 2].z = world.z;
			vert[i * 2].y = 1.0f;


		}

		

	}
	*/


}

void MeshCreator::CreateBackGround_NoWall(std::vector<cv::Point> cor, std::vector<cv::Point> boundary,
	std::vector<glm::vec3>& vert, std::vector<float>& uv, std::vector<uint>& idx, std::vector<uint>& wireIdx) {

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
		vert.push_back({ vx, vy, 0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}
	//2 horizon
	{
		float uvx = float(cor[0].x) / targetSize;
		float uvy = float(boundary[0].y) / targetSize;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vy *= -1.0;
		vert.push_back({ vx, vy, 0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

	//3 left down to leftup
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
			float uvx = float(boundary[i].x) / targetSize;
			float uvy = (cor[0].y) / targetSize;
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

		{
			float uvx = float(boundary[i].x) / targetSize;
			float uvy = float(cor[2].y) / targetSize;
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
		float uvy = float(cor[1].y) / targetSize;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vy *= -1.0;
		vert.push_back({ vx, vy, 0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}
	//rightdown
	{
		float uvx = float(cor[1].x) / targetSize;
		float uvy = float(boundary[boundary.size()-1].y) / targetSize;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vy *= -1.0;
		vert.push_back({ vx, vy, 0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

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

	for (int i = 5; i < vert.size(); i += 3) {
		int leftup = i - 5;
		int lefthori = i - 4;
		int leftdown = i - 3;
		int rightup = i - 2;
		int righthori = i - 1;
		int rightdown = i;

		idx.push_back(leftup); idx.push_back(lefthori); idx.push_back(rightup);
		idx.push_back(rightup); idx.push_back(lefthori); idx.push_back(righthori);
		idx.push_back(lefthori); idx.push_back(leftdown); idx.push_back(righthori);
		idx.push_back(righthori); idx.push_back(leftdown); idx.push_back(rightdown);

		wireIdx.push_back(leftup); wireIdx.push_back(lefthori);
		wireIdx.push_back(lefthori); wireIdx.push_back(leftdown);
		wireIdx.push_back(rightup); wireIdx.push_back(righthori);
		wireIdx.push_back(righthori); wireIdx.push_back(rightdown);

		wireIdx.push_back(leftup); wireIdx.push_back(rightup);
		wireIdx.push_back(leftdown); wireIdx.push_back(rightdown);
		wireIdx.push_back(lefthori); wireIdx.push_back(righthori);

		wireIdx.push_back(lefthori); wireIdx.push_back(rightup);
		wireIdx.push_back(leftdown); wireIdx.push_back(righthori);

	}




	for (int i = 0; i < vert.size(); ++i) {
		vert[i].z = 2.0f;
	}


	float rati = abs(vert[vert.size() - 1].x- vert[2].x);

	int cols = vert.size() / 3;
	float wid = 6.0 / rati;
	//down
	for (int i = 0; i < cols; ++i) {
		int p = i * 3;
		p += 2;

		vert[p] = {vert[p].x*wid,-1.0,-2.0};

	}

	//hori
	wid = 12.0 / rati;
	for (int i = 0; i < cols; ++i) {
		int p = i * 3;
		p += 1;

		vert[p] = { vert[p].x * wid,-1.0,2.0};

	}

	//up
	for (int i = 0; i < cols; ++i) {
		int p = i * 3;
		vert[p] = { vert[p].x * wid,4.0,2.0 };

	}


	for (int i = 0; i < vert.size(); ++i) {
		vert[i].x = vert[i].x * -1.0;
	}
}

void MeshCreator::CreateQuad(float asp,
	std::vector<glm::vec3>& vert, std::vector<float>& uv, std::vector<uint>& idx, std::vector<uint>& wireIdx) {

	{
		float uvx = 0;
		float uvy = 0;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vx *= -1.0;
		vy *= -1.0;
		vert.push_back({ vx, vy, 1.0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

	{
		float uvx = 1.0;
		float uvy = 0;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vx *= -1.0;
		vy *= -1.0;
		vert.push_back({ vx, vy, 1.0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

	{
		float uvx = 1.0;
		float uvy = 1.0;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vx *= -1.0;
		vy *= -1.0;
		vert.push_back({ vx, vy, 1.0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

	{
		float uvx = 0;
		float uvy = 1.0;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vx *= -1.0;
		vy *= -1.0;
		vert.push_back({ vx, vy, 1.0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

	idx.push_back(0); idx.push_back(2); idx.push_back(1);
	idx.push_back(0); idx.push_back(3); idx.push_back(2);

	wireIdx.push_back(0), wireIdx.push_back(1);
	wireIdx.push_back(0), wireIdx.push_back(2);
	wireIdx.push_back(0), wireIdx.push_back(3);
	wireIdx.push_back(1), wireIdx.push_back(2);
	wireIdx.push_back(3), wireIdx.push_back(2);
}

void MeshCreator::CreateCursor(std::vector<glm::vec3>& vert, std::vector<float>& uv, std::vector<uint>& idx, std::vector<uint>& wireIdx) {
	float asp = 0.04;
	{
		float uvx = 0;
		float uvy = 0;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vx *= -1.0;
		vy *= -1.0;
		vx *= asp;
		vy *= asp;
		vert.push_back({ vx, vy, -1.0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

	{
		float uvx = 1.0;
		float uvy = 0;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vx *= -1.0;
		vy *= -1.0;
		vx *= asp;
		vy *= asp;
		vert.push_back({ vx, vy, -1.0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

	{
		float uvx = 1.0;
		float uvy = 1.0;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vx *= -1.0;
		vy *= -1.0;
		vx *= asp;
		vy *= asp;
		vert.push_back({ vx, vy, -1.0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

	{
		float uvx = 0;
		float uvy = 1.0;
		float vx = uvx * 2 - 1.0;
		float vy = uvy * 2 - 1.0;
		vx *= -1.0;
		vy *= -1.0;
		vx *= asp;
		vy *= asp;
		vert.push_back({ vx, vy, -1.0 });
		uv.push_back(uvx);
		uv.push_back(uvy);
	}

	idx.push_back(0); idx.push_back(2); idx.push_back(1);
	idx.push_back(0); idx.push_back(3); idx.push_back(2);

	wireIdx.push_back(0), wireIdx.push_back(1);
	wireIdx.push_back(0), wireIdx.push_back(2);
	wireIdx.push_back(0), wireIdx.push_back(3);
	wireIdx.push_back(1), wireIdx.push_back(2);
	wireIdx.push_back(3), wireIdx.push_back(2);
	wireIdx.push_back(3), wireIdx.push_back(1);
}

void MeshCreator::CreateForeGround(std::vector<cv::Point> cont, std::vector<std::vector<cv::Point>> innerConts,
	std::vector<glm::vec3>& vert, std::vector<float>& uv, std::vector<uint>& idx, std::vector<uint>& wireIdx, std::vector<int>& tetIdx, int& negaOfset) {

	float targetSize = 512.0;

	vert.clear();
	uv.clear();
	idx.clear();
	wireIdx.clear();

	std::vector<cv::Point> revcont = cont;
	std::vector<std::vector<cv::Point>> revInnerConts;

	for (int i = 0; i < innerConts.size(); ++i) {
		std::vector<cv::Point> rev=innerConts[i];
		std::reverse(rev.begin(), rev.end());
		revInnerConts.push_back(rev);
	}

	std::reverse(revcont.begin(), revcont.end());


	if (cont.size() > 2) {
		class Delauney delauney(revcont, targetSize);
		delauney.SetInnerConts(revInnerConts);
		delauney.MakePolygonData();

		//std::vector<glm::vec2> tempVert = delauney.GetVertices();
		//std::vector<float> tempUV = delauney.GetUV();
		//std::vector<int> tempIdx = delauney.GetIndices();
		//std::vector<int> tempWire = delauney.GetWireFrame();

		std::vector<glm::vec3> tempVert3D = delauney.GetVertices3D();
		std::vector<int> tempIdx3D = delauney.GetIndices3D();
		std::vector<int> tempWire3D = delauney.GetWireFrame3D();
		std::vector<float> tempUV3D = delauney.GetUV3D();

		std::vector<int> tempTet = delauney.GetTetMesh();
		negaOfset = delauney.GetNegaOfset();
		/*
		for (int i = 0; i < tempVert.size(); ++i) {
			vert.push_back({ tempVert[i].x,tempVert[i].y,0 });
		}
		for (int i = 0; i < tempIdx.size(); ++i) {
			int id = tempIdx[i];
			idx.push_back(id);
		}
		for (int i = 0; i < tempUV.size(); ++i) {
			uv.push_back(tempUV[i]);
		}
		for (int i = 0; i < tempWire.size(); ++i) {
			wireIdx.push_back(tempWire[i]);
		}
		*/
		
		for (int i = 0; i < tempVert3D.size(); ++i) {
			vert.push_back(tempVert3D[i]);
		}

		for (int i = 0; i < tempIdx3D.size(); ++i) {
			int id = tempIdx3D[i];
			idx.push_back(id);
		}
		for (int i = 0; i < tempWire3D.size(); ++i) {
			wireIdx.push_back(tempWire3D[i]);
		}
		for (int i = 0; i < tempUV3D.size(); ++i) {
			uv.push_back(tempUV3D[i]);
		}	
		for (int i = 0; i < tempTet.size(); ++i) {
			tetIdx.push_back(tempTet[i]);
		}
	}


}

void MeshCreator::CalcCord(float& x, float& y, float& z) {
	z = 0;
}