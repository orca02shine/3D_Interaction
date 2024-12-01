#pragma once

#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <utility>
#include <random>
#include <cmath>
#include <limits>
#include <chrono>
#include <cassert>
#include <stack>
#include <deque>
#include <set>

#include <glm/glm.hpp>
#include <glm/gtx/exterior_product.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>


using Contour = std::vector<cv::Point>;
using DeEdge = std::pair<size_t, size_t>;//頂点番号

struct Line {

	glm::vec2 start;
	glm::vec2  end;

	float a() {
		return end.y - start.y;
	}
	float b() {
		return start.x - end.x;
	}
	float c() {
		return (end.x - start.x) * start.x - (end.y - start.y) * start.x;
	}

	glm::vec2 vec() {
		return end - start;
	}

	float length() {
		glm::vec2 tmp = end - start;
		return tmp.length();
	}

};

struct Triangle {
	size_t id[3];

	size_t opposite(DeEdge e) {
		if (e.first != id[0] && e.second != id[0])return id[0];
		if (e.first != id[1] && e.second != id[1]) return id[1];
		return id[2];
	}

	bool hasCommonEdge(DeEdge e) {
		if (id[0] != e.first && id[1] != e.first && id[2] != e.first)return false;
		if (id[0] != e.second && id[1] != e.second && id[2] != e.second)return false;
		return true;
	}
};

struct AdjTriangles {
	int adjtri;

	int e1;
	int e2;

};

class Delauney
{
	Contour _Contour;
	int _TexSize;


	std::vector<glm::vec2> _Vertices;

	std::vector<float> _UV;
	std::vector<int> _Indices;
	std::vector<int> _WireIdx;

	std::vector<Triangle> _Triangles;
	std::stack<DeEdge> _EdgeStack;


	std::vector<glm::vec2> _TeddyTempVerts;
	std::vector<std::vector<int>> _TeddyTempAdj;
	std::vector<Triangle> _TeddyTriangles_Outer;//外側に辺がある三角形の番号
	std::vector<Triangle> _TeddyTriangles_Inner;//
	std::vector<bool> _IsChodralAxis; //内側の軸頂点かどうか


	bool _EdgeC[200][200];
	std::vector<int> _VertexBound;
	std::stack<std::pair<int, int>> _ContourIdx;

public:

	bool Is_ccw(size_t a, size_t b, size_t c);

	DeEdge MakeEdge(size_t a, size_t b);
	Triangle MakeTriangle(size_t a, size_t b, size_t c);

	Delauney(Contour contour,int texSize);
	~Delauney();

	void Init();

	void MakeVirtualTriangle();


	void MakeContours();


	//void MakeAdditionalTriangle();

	void MakePolygonData();

	int FIndTriange(size_t tar);
	//int LawsonTriangleDetection(size_t tar);


	bool IntersectEdge(const DeEdge& e1, const DeEdge& e2);

	bool IsInTriangle(size_t tar, const Triangle& t);

	bool IsConstrainedEdge(const DeEdge& e);

	bool IsInCircle(size_t tar, const Triangle& tri);

	void SplitTriangle(size_t triID, size_t newVertID);

	void FlipTriangle(int idC, int idD, size_t C, size_t D, const DeEdge& e);

	void MakeEdgeConstraint(Contour contour);

	void MakeDelauney(Contour contour);

	void DigestStack();

	void FixDelauney(Contour contour);

	void DeleteVirtualTri();

	void DeleteTris();
	void DeleteOuterTris();
	void DeleteInnerTris();

	void MakeTeddyTempVerts();
	bool TeddyInCircle(DeEdge e,std::vector<int> vertices);
	void TestSetData();

	void SetData();

public:
	std::vector<float> GetUV() { return _UV; }
	std::vector<int> GetIndices() { return _Indices; }
	std::vector<glm::vec2> GetVertices() { return  _Vertices; }
	std::vector<int> GetWireFrame() { return _WireIdx; }
};