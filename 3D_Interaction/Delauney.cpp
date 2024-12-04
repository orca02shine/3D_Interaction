#include "Delauney.h"

Delauney::Delauney(Contour contour, int texSize) :_Contour(contour), _TexSize(texSize)
{
	Init();
}
Delauney::~Delauney()
{
}

void Delauney::Init() {

	MakePolygonData();
}

DeEdge Delauney::MakeEdge(size_t a, size_t b) { return DeEdge((std::min)(a, b), (std::max)(a, b)); }

Triangle Delauney::MakeTriangle(size_t a, size_t b, size_t c) {
	/*** Make triangle where the direction 'a->b->c' is counter-clockwise ***/
			//std::cout << "tri check  " <<a<<"  " << b << "  " << c << std::endl;
	if (!Is_ccw(a, b, c)) {
		std::swap(b, c);
	}
	return Triangle{ a, b, c };
}


bool Delauney::Is_ccw(size_t a, size_t b, size_t c) {
	float val = glm::cross(_Vertices[b] - _Vertices[a], _Vertices[c] - _Vertices[a]);
	return(val > 0.0);
}

bool Delauney::IsConstrainedEdge(const DeEdge& e) {

	return _EdgeC[e.first][e.second];
}

bool Delauney::IntersectEdge(const DeEdge& e1, const DeEdge& e2) {
	glm::vec2 v1 = _Vertices[e1.second] - _Vertices[e1.first];
	glm::vec2 v2 = _Vertices[e2.second] - _Vertices[e2.first];


	if (glm::cross(v1, v2) == 0)return false;

	glm::vec2 v0 = _Vertices[e2.first] - _Vertices[e1.first];
	double t1 = -(glm::cross(v0, v1) / glm::cross(v2, v1));
	double t2 = (glm::cross(v0, v2) / glm::cross(v1, v2));

	if (0.0 < t1 && t1 < 1.0 && 0.0 < t2 && t2 < 1.0) {
		return true;
	}
	return false;
}

bool Delauney::IsInTriangle(size_t tar, const Triangle& t) {
	if (!Is_ccw(t.id[0], t.id[1], tar)) return false;
	if (!Is_ccw(t.id[1], t.id[2], tar)) return false;
	if (!Is_ccw(t.id[2], t.id[0], tar)) return false;
	return true;
}

int Delauney::FIndTriange(size_t tar) {
	for (int i = 0; i < _Triangles.size(); i++) {
		if (IsInTriangle(tar, _Triangles[i])) {
			return i;
		}
	}
	return -1;
}

void Delauney::SplitTriangle(size_t triID, size_t newVertID)
{
	int v1 = _Triangles[triID].id[0];
	int v2 = _Triangles[triID].id[1];
	int v3 = _Triangles[triID].id[2];

	_EdgeStack.push(MakeEdge(v1, v2));
	_EdgeStack.push(MakeEdge(v2, v3));
	_EdgeStack.push(MakeEdge(v3, v1));

	Triangle rmTri = MakeTriangle(v1, v2, v3);
	_Triangles.erase(_Triangles.begin() + triID);

	Triangle tri1 = MakeTriangle(newVertID, v1, v2);
	Triangle tri2 = MakeTriangle(newVertID, v2, v3);
	Triangle tri3 = MakeTriangle(newVertID, v3, v1);

	//std::cout <<"Split triangle id is  " << newVertID << "  " << v1 << "  " << v2 << "  " << v3 << std::endl;

	_Triangles.push_back(tri1);
	_Triangles.push_back(tri2);
	_Triangles.push_back(tri3);
}

void Delauney::FlipTriangle(int idC, int idD, size_t C, size_t D, const DeEdge& e) {
	if (idD < idC)std::swap(idC, idD);

	_Triangles.erase(_Triangles.begin() + idD);//D
	_Triangles.erase(_Triangles.begin() + idC);//C

	_Triangles.push_back(MakeTriangle(D, e.first, C));
	//Triangle A = _Triangles.back();
	//std::cout << "tri check flip A  " << A.id[0] << "  " << A.id[1] << "  " << A.id[2] << std::endl;

	_Triangles.push_back(MakeTriangle(D, C, e.second));
	//Triangle B = _Triangles.back();
	//std::cout << "tri check flip B  " << B.id[0] << "  " << B.id[1] << "  " << B.id[2] << std::endl;

	/*
	_EdgeStack.push(MakeEdge(e.first,C));
	_EdgeStack.push(MakeEdge(e.second, C));
	_EdgeStack.push(MakeEdge(e.first, D));
	_EdgeStack.push(MakeEdge(e.second, D));
	*/

	return;
}

bool Delauney::IsInCircle(size_t tar, const Triangle& tri) {
	double x1 = _Vertices[tri.id[0]].x; double y1 = _Vertices[tri.id[0]].y;
	double x2 = _Vertices[tri.id[1]].x; double y2 = _Vertices[tri.id[1]].y;
	double x3 = _Vertices[tri.id[2]].x; double y3 = _Vertices[tri.id[2]].y;

	//Š„‚é
	double m = 2 * ((x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1));

	double x = ((y3 - y1) * (x2 * x2 - x1 * x1 + y2 * y2 - y1 * y1)
		+ (y1 - y2) * (x3 * x3 - x1 * x1 + y3 * y3 - y1 * y1)) / m;

	double y = ((x1 - x3) * (x2 * x2 - x1 * x1 + y2 * y2 - y1 * y1)
		+ (x2 - x1) * (x3 * x3 - x1 * x1 + y3 * y3 - y1 * y1)) / m;


	double dx = x1 - x; double dy = y1 - y;
	double radius = sqrt(dx * dx + dy * dy);

	double dx2 = _Vertices[tar].x - x;
	double dy2 = _Vertices[tar].y - y;
	double dist = sqrt(dx2 * dx2 + dy2 * dy2);

	return (dist < radius);

}

void Delauney::MakeVirtualTriangle() {

	const glm::vec2 v0 = { -3.0,-2.0 };
	const glm::vec2 v1 = { 3.0,-2.0 };
	const glm::vec2 v2 = { 0.0,3.0 };

	_Vertices.emplace_back(v0);
	_Vertices.emplace_back(v1);
	_Vertices.emplace_back(v2);


	Triangle fstri = { 0,1,2 };
	_Triangles.push_back(fstri);

}

void Delauney::MakeContours() {
	MakeEdgeConstraint(_Contour);

	MakeDelauney(_Contour);
	FixDelauney(_Contour);
}


void Delauney::MakePolygonData() {
	MakeVirtualTriangle();//ok

	//“_‚Ì’€ŽŸ’Ç‰Á
	MakeContours();//?


	//delete supartriangle
	DeleteVirtualTri();

	//‹«ŠEŠO‚ðíœ
	DeleteOuterTris();

	//‹«ŠE‚Ì“àŠO”»’è
	DeleteTris();

	//ŒŠ‹«ŠE
	//MakeInternalContours();

	//DeleteInnerTris();

	//•iŽ¿Œüã‚Ìˆ—

	//-----

	MakeTeddyTempVerts();

	//SetData();

}

void Delauney::MakeEdgeConstraint(Contour contour)
{
	int size = contour.size();
	int startIdx = _Vertices.size();
	_VertexBound.push_back(startIdx);
	int endIdx = startIdx + size - 1;


	for (int i = startIdx + 1; i <= endIdx; i++) {
		_EdgeC[i - 1][i] = true;
		_EdgeC[i][i - 1] = true;
	}
	_EdgeC[startIdx][endIdx] = true;
	_EdgeC[endIdx][startIdx] = true;

	//std::cout<<"EdgeC cpomplete"<<std::endl;

}

void Delauney::MakeDelauney(Contour contour)
{
	int size = contour.size();;
	if (size < 3)return;

	int ofset = _Vertices.size();
	//int stIdx = _Vertices.size();
	for (int i = 0; i < size; i++) {
		float w = _TexSize;
		float h = _TexSize;

		float X = (contour[i].x / (w / 2.0)) - 1.0f;
		float Y = (contour[i].y / (h / 2.0)) - 1.0f;
		Y *= -1.0;

		glm::vec2 addVert = { X, Y };


		_Vertices.emplace_back(addVert);

		int currentIdx = _Vertices.size() - 1;

		int tri = FIndTriange(currentIdx);

		//std::cout << "find triangle is" << tri<< std::endl;
		if (tri > -1) {
			SplitTriangle(tri, i + ofset);
		}
		DigestStack();

	}
	//int edIdx = _Vertices.size() - 1;
	//_ContourIdx.push({ stIdx,edIdx });

	//std::cout << "Make delauney complete" << std::endl;

}

void Delauney::DigestStack() {

	while (!_EdgeStack.empty()) {
		DeEdge nextEdge = _EdgeStack.top();
		_EdgeStack.pop();

		if (IsConstrainedEdge(nextEdge)) { continue; }

		std::vector<Triangle> shareEdgeTris;
		std::vector<int> idxOfTris;

		for (int ct = 0; ct < _Triangles.size(); ++ct) {
			if (_Triangles[ct].hasCommonEdge(nextEdge)) {
				shareEdgeTris.push_back(_Triangles[ct]);
				idxOfTris.push_back(ct);
			}
		}

		if (shareEdgeTris.size() != 2) { continue; }

		Triangle tri_abc = shareEdgeTris[0]; //Šù‚É‚ ‚Á‚½ŽOŠpŒ`
		Triangle tri_abd = shareEdgeTris[1];//addVert‚ðŠÜ‚ÞV‚µ‚­ì‚ç‚ê‚½ŽOŠpŒ`

		int id_abc = idxOfTris[0];
		int id_abd = idxOfTris[1];

		size_t vertC = tri_abc.opposite(nextEdge);
		size_t vertD = tri_abd.opposite(nextEdge);

		DeEdge tempEdge = MakeEdge(vertC, vertD);
		bool chk = false;

		if (IsConstrainedEdge(tempEdge) && IntersectEdge(nextEdge, tempEdge)) { chk = true; }//flipEdge‚ª§–ñ‚ ‚è‚Ìê‡

		if (IsInCircle(vertC, _Triangles[id_abd])) { chk = true; }

		if (chk) {
			FlipTriangle(id_abc, id_abd, vertC, vertD, nextEdge);

			_EdgeStack.push(MakeEdge(nextEdge.first, vertC));
			_EdgeStack.push(MakeEdge(nextEdge.second, vertC));
			_EdgeStack.push(MakeEdge(nextEdge.first, vertD));
			_EdgeStack.push(MakeEdge(nextEdge.second, vertD));
		}
	}

	//std::cout << "     DigestStack complete" << std::endl;
}

void Delauney::FixDelauney(Contour contour) {
	std::set<DeEdge> edges;
	std::vector<DeEdge> edge_ab;

	for (int i = 0; i < _Triangles.size(); i++) {
		int id0 = _Triangles[i].id[0];
		int id1 = _Triangles[i].id[1];
		int id2 = _Triangles[i].id[2];

		if (id0 < 3 || id1 < 3 || id2 < 3)continue;

		DeEdge e1 = MakeEdge(id0, id1);
		DeEdge e2 = MakeEdge(id1, id2);
		DeEdge e3 = MakeEdge(id2, id0);

		edges.insert(e1);
		edges.insert(e2);
		edges.insert(e3);
	}
	int lastVert = _Vertices.size() - 1;
	int startVert = _Vertices.size() - contour.size();

	std::deque<DeEdge> Q;

	for (int i = startVert + 1; i <= lastVert; i++) {
		edge_ab.push_back(MakeEdge(i - 1, i));
	}
	edge_ab.push_back(MakeEdge(startVert, lastVert));

	for (auto& e_ab : edge_ab) {
		for (auto& e : edges) {
			if (IntersectEdge(e_ab, e)) {

				//std::cout << "edge  " << e_ab.first << "  " << e_ab.second << " is intersected  " << e.first << " "<<e.second << std::endl;
				/*
				std::cout << "edge0.first  " << _Vertices[e_ab.first].position[0] << "  " << _Vertices[e_ab.first].position[1] <<std::endl;
				std::cout << "edge0.second  " << _Vertices[e_ab.second].position[0] << "  " << _Vertices[e_ab.second].position[1] << std::endl;
				std::cout << "edge1.first  " << _Vertices[e.first].position[0] << "  " << _Vertices[e.first].position[1] << std::endl;
				std::cout << "edge1.second  " << _Vertices[e.second].position[0] << "  " << _Vertices[e.second].position[1] << std::endl;
				*/

				Q.push_front(e);
			}
		}


		while (!Q.empty()) {
			DeEdge cd = Q.front();
			Q.pop_front();

			std::vector<Triangle> shareEdgeTris;
			std::vector<int> idxOfTris;

			for (int ct = 0; ct < _Triangles.size(); ++ct) {
				if (_Triangles[ct].hasCommonEdge(cd)) {
					shareEdgeTris.push_back(_Triangles[ct]);
					idxOfTris.push_back(ct);
				}
			}

			if (shareEdgeTris.size() != 2) { continue; }

			Triangle tri_cde = shareEdgeTris[0];
			Triangle tri_cdf = shareEdgeTris[1];

			int id_cde = idxOfTris[0];
			int id_cdf = idxOfTris[1];

			size_t vertE = tri_cde.opposite(cd);
			size_t vertF = tri_cdf.opposite(cd);

			DeEdge ef = MakeEdge(vertE, vertF);

			if (IntersectEdge(cd, ef)) {
				FlipTriangle(id_cde, id_cdf, vertE, vertF, cd);
				_EdgeStack.push(ef);
			}
			else {
				Q.push_back(cd);
			}
		}

		DigestStack();


	}


	//std::cout << "Fix delauney complete" << std::endl;
}

void Delauney::DeleteVirtualTri() {
	std::stack<int> delTriId;
	for (int i = 0; i < _Triangles.size(); ++i) {

		if (_Triangles[i].id[0] < 3 || _Triangles[i].id[1] < 3 || _Triangles[i].id[2] < 3) {
			delTriId.push(i);
			//std::cout <<"delete tri id is  " <<i<<"   Vertidx is  " << _Triangles[i].id[0] << "  " << _Triangles[i].id[1] << "  " << _Triangles[i].id[2] << std::endl;
		}
	}
	while (!delTriId.empty()) {
		int del = delTriId.top();
		delTriId.pop();

		_Triangles.erase(_Triangles.begin() + del);
	}
}

void Delauney::DeleteTris() {

	std::stack<int> delTriId;
	for (int i = 0; i < _Triangles.size(); ++i) {
		bool del = false;
		Triangle e = _Triangles[i];

		if ((e.id[0] > e.id[1] && e.id[1] > e.id[2]) ||
			(e.id[1] > e.id[2] && e.id[2] > e.id[0]) ||
			(e.id[2] > e.id[0] && e.id[0] > e.id[1]))
		{
		}
		else {
			del = true;
		}



		if (del) {
			delTriId.push(i);
		}
	}
	while (!delTriId.empty()) {
		int del = delTriId.top();
		delTriId.pop();

		_Triangles.erase(_Triangles.begin() + del);
	}

}

void Delauney::DeleteOuterTris() {

	//_VertexBound.push_back(_Vertices.size());
	std::stack<int> delTriId;

	for (int i = 0; i < _Triangles.size(); ++i) {
		bool del = false;
		Triangle e = _Triangles[i];

		int e_max = std::max(e.id[0], std::max(e.id[1], e.id[2]));
		int e_min = std::min(e.id[0], std::min(e.id[1], e.id[2]));

		for (int j = 0; j < _VertexBound.size(); ++j) {
			int ch = _VertexBound[j];
			if (e_min < ch && ch <= e_max) {
				del = true;
			}

		}
		if (del) {
			delTriId.push(i);
		}
	}
	while (!delTriId.empty()) {
		int del = delTriId.top();
		delTriId.pop();

		_Triangles.erase(_Triangles.begin() + del);
	}


}

void Delauney::DeleteInnerTris() {

	_VertexBound.push_back(_Vertices.size());
	std::stack<int> delTriId;

	for (int i = 0; i < _Triangles.size(); ++i) {
		bool del = false;
		Triangle e = _Triangles[i];

		int e_max = std::max(e.id[0], std::max(e.id[1], e.id[2]));
		int e_min = std::min(e.id[0], std::min(e.id[1], e.id[2]));

		for (int j = 0; j < _VertexBound.size() - 1; ++j) {
			int ch_min = _VertexBound[j];
			int ch_max = _VertexBound[j + 1];

			if (ch_min <= e_min && e_max < ch_max) {

				if (!(e.id[0] > e.id[1] && e.id[1] > e.id[2]) ||
					(e.id[1] > e.id[2] && e.id[2] > e.id[0]) ||
					(e.id[2] > e.id[0] && e.id[0] > e.id[1]))
				{
					del = true;
				}

			}

		}
		if (del) {
			delTriId.push(i);
		}
	}
	while (!delTriId.empty()) {
		int del = delTriId.top();
		delTriId.pop();

		_Triangles.erase(_Triangles.begin() + del);
	}


}

void Delauney::SetData() {

	std::set<DeEdge> wireFrame;
	for (int i = 0; i < _Triangles.size(); i++) {
		int k0 = _Triangles[i].id[0];
		int k1 = _Triangles[i].id[1];
		int k2 = _Triangles[i].id[2];

		if (k0 < 3 || k1 < 3 || k2 < 3)continue;

		k0 -= 3;
		k1 -= 3;
		k2 -= 3;

		_Indices.emplace_back(k0);
		_Indices.emplace_back(k1);
		_Indices.emplace_back(k2);


		DeEdge e1 = MakeEdge(k0, k1);
		DeEdge e2 = MakeEdge(k1, k2);
		DeEdge e3 = MakeEdge(k2, k0);

		wireFrame.insert(e1);
		wireFrame.insert(e2);
		wireFrame.insert(e3);
	}

	for (auto& e : wireFrame) {
		_WireIdx.emplace_back(e.first);
		_WireIdx.emplace_back(e.second);
	}


	for (int i = 3; i < _Vertices.size(); i++) {
		float x = _Vertices[i].x;
		float y = 1.0 - _Vertices[i].y;

		_UV.emplace_back(x);
		_UV.emplace_back(y);
	}

	_Vertices.erase(_Vertices.begin() + 2);
	_Vertices.erase(_Vertices.begin() + 1);
	_Vertices.erase(_Vertices.begin());


}

bool Delauney::TeddyInCircle(DeEdge e, std::vector<int> vertices) {
	glm::vec2 v = _Vertices[e.first] + _Vertices[e.second];
	v /= 2;
	glm::vec2 l = _Vertices[e.first] - _Vertices[e.second];
	double rad = glm::length(l);
	rad /= 2;

	for (int i = 0; i < vertices.size(); ++i) {
		glm::vec2 lo = _Vertices[vertices[i]] - v;
		double d = glm::length(lo);
		//std::cout << d << " "<<rad << std::endl;
		if (d > rad) {
			return false;
		}
	}

	return true;
}
Triangle Delauney::MakeTeddyTriangle(size_t a, size_t b, size_t c, bool negaMode) {
	glm::vec2 v0 = { _TeddyVertices[a].x,_TeddyVertices[a].y };
	glm::vec2 v1 = { _TeddyVertices[b].x,_TeddyVertices[b].y };
	glm::vec2 v2 = { _TeddyVertices[c].x,_TeddyVertices[c].y };

	float val = glm::cross(v1 - v0, v2 - v0);

	if (negaMode) {
		val *= -1;
	}

	if (!(val > 0.0)) {
		std::swap(b, c);
	}

	return Triangle{ a,b,c };

}

void Delauney::MakeTeddyTempVerts() {

	std::vector<bool> _IsChodralAxis;
	_IsChodralAxis.resize(500, false);
	std::vector<std::vector<float>> _SumLengthFromAxis(500);

	_Vertices.erase(_Vertices.begin() + 2);
	_Vertices.erase(_Vertices.begin() + 1);
	_Vertices.erase(_Vertices.begin());

	for (int i = 0; i < _Vertices.size(); ++i) {
		glm::vec2 v = _Vertices[i];
		_TeddyVertices.push_back({ v.x,v.y,0 });
	}



	for (int i = 0; i < _Triangles.size(); ++i) {
		_Triangles[i].id[0] -= 3;
		_Triangles[i].id[1] -= 3;
		_Triangles[i].id[2] -= 3;
	}

	int numVerts = _Vertices.size();



	int Tsize = _Triangles.size();

	std::vector<std::vector<AdjTriangles>> Graph(Tsize);
	std::vector<std::vector<int>> edgeChk(numVerts, std::vector<int>(numVerts, -1));

	std::vector<int> IsTerminal(Tsize, 0);//terminal =1 sleeve =2 juction=3
	std::vector<std::pair<int, int>> OuterEdge(Tsize, { -1,-1 });

	for (int i = 0; i < Tsize; i++) {
		int k0 = _Triangles[i].id[0];
		int k1 = _Triangles[i].id[1];
		int k2 = _Triangles[i].id[2];


		int ct = 0;

		if ((k2 + 1) % numVerts != k1) {
			if (edgeChk[k1][k2] == -1 && edgeChk[k2][k1] == -1) {
				edgeChk[k2][k1] = i;
				edgeChk[k1][k2] = i;
			}
			else {
				Graph[i].push_back({ edgeChk[k1][k2],k2,k1 });
				Graph[edgeChk[k1][k2]].push_back({ i, k2, k1 });
			}
			ct++;
		}
		else {
			OuterEdge[i] = { k2,k1 };
		}

		if ((k1 + 1) % numVerts != k0) {
			if (edgeChk[k1][k0] == -1 && edgeChk[k0][k1] == -1) {
				edgeChk[k0][k1] = i;
				edgeChk[k1][k0] = i;
			}
			else {
				Graph[i].push_back({ edgeChk[k0][k1],k1,k0 });
				Graph[edgeChk[k0][k1]].push_back({ i, k1, k0 });
			}
			ct++;
		}
		else {
			OuterEdge[i] = { k1,k0 };
		}

		if ((k0 + 1) % numVerts != k2) {
			if (edgeChk[k0][k2] == -1 && edgeChk[k2][k0] == -1) {
				edgeChk[k0][k2] = i;
				edgeChk[k2][k0] = i;
			}
			else {
				Graph[i].push_back({ edgeChk[k0][k2],k0,k2 });
				Graph[edgeChk[k0][k2]].push_back({ i, k0, k2 });
			}
			ct++;
		}
		else {
			OuterEdge[i] = { k2,k0 };
		}

		IsTerminal[i] = ct;
	}
	/*
	for (int triid = 0; triid<Graph.size(); ++triid) {
		for (auto adj : Graph[triid]) {
			std::cout << "triid  " << triid << " is adjcent " << adj.adjtri << " edge vert is " << adj.e1 << " " << adj.e2 << std::endl;
		}
	}
	*/
	std::set<DeEdge> wireFrame;

	std::vector<bool> seen(Tsize);
	std::vector<std::vector<bool>> invalidEdge(numVerts, std::vector<bool>(numVerts, false));//juction
	std::vector<int> junctionMidPoint(Tsize, -1);
	std::vector<std::vector<int>> edgeMidPoint(numVerts, std::vector<int>(numVerts, -1));// midpoint id

	for (int i = 0; i < Tsize; ++i) {
		if (IsTerminal[i] != 1)continue;

		std::vector<int> vertOfFanTris;

		std::queue<int> triQ;


		int kari0 = _Triangles[i].id[0];
		int kari1 = _Triangles[i].id[1];
		int kari2 = _Triangles[i].id[2];

		/*
		wireFrame.insert({kari0-3,kari1-3});
		wireFrame.insert({ kari1-3,kari2-3 });
		wireFrame.insert({ kari2-3,kari0 -3});
		*/


		int e1 = Graph[i][0].e1;
		int e2 = Graph[i][0].e2;

		int terVert = _Triangles[i].opposite({ e1,e2 });
		//std::cout << "kari " << kari0 << " " << kari1 << " " << kari2 << std::endl;
		//std::cout << "tri " << e1 << " " << e2 << " " << terVert << std::endl;
		vertOfFanTris.push_back(terVert);
		vertOfFanTris.push_back(e1);
		vertOfFanTris.push_back(e2);


		//wireFrame.insert({ e1-3 ,e2-3  });
		//wireFrame.insert({ e2-3 ,terVert-3  });
		//wireFrame.insert({ terVert-3 ,e1-3 });

		int mididx = -1;


		if (TeddyInCircle({ e1,e2 }, vertOfFanTris)) {
			triQ.push(Graph[i][0].adjtri);
			seen[i] = true;
		}

		else {
			glm::vec2 midvert = _Vertices[e1] + _Vertices[e2];
			midvert /= 2;
			_Vertices.emplace_back(midvert);
			mididx = _Vertices.size() - 1;
			edgeMidPoint[e1][e2] = mididx;
			edgeMidPoint[e2][e1] = mididx;
			_IsChodralAxis[mididx] = true;
		}


		int prev1 = e1;
		int prev2 = e2;

		while (!triQ.empty()) {
			int now = triQ.front();
			triQ.pop();

			if (IsTerminal[now] == 3) {
				glm::vec2 midvert = (_Vertices[_Triangles[now].id[0]] + _Vertices[_Triangles[now].id[1]] + _Vertices[_Triangles[now].id[2]]);
				midvert /= 3;
				_Vertices.push_back(midvert);
				mididx = _Vertices.size() - 1;
				junctionMidPoint[now] = mididx;
				_IsChodralAxis[mididx] = true;
				invalidEdge[prev1][prev2] = true;
				invalidEdge[prev2][prev1] = true;
				break;
			}

			seen[now] = true;
			for (auto& e : Graph[now]) {
				if (seen[e.adjtri])continue;



				if (TeddyInCircle({ e.e1,e.e2 }, vertOfFanTris)) {
					vertOfFanTris.push_back(e.e1);
					vertOfFanTris.push_back(e.e2);
					triQ.push(e.adjtri);
					seen[now] = true;
					prev1 = e.e1;
					prev2 = e.e2;
				}
				else {
					glm::vec2 midvert = (_Vertices[e.e1] + _Vertices[e.e2]);
					midvert /= 2;
					_Vertices.push_back(midvert);
					mididx = _Vertices.size() - 1;
					_IsChodralAxis[mididx] = true;
					edgeMidPoint[e.e1][e.e2] = mididx;
					edgeMidPoint[e.e2][e.e1] = mididx;

					vertOfFanTris.push_back(e.e1);
					vertOfFanTris.push_back(e.e2);
					std::sort(vertOfFanTris.begin(), vertOfFanTris.end());
					vertOfFanTris.erase(std::unique(vertOfFanTris.begin(), vertOfFanTris.end()), vertOfFanTris.end());
					break;
				}
			}


		}
		//‹L˜^
		_IsChodralAxis[mididx] = true;

		/*
		std::cout << "terminal id is " << i << std::endl;;
		for (int p = 0; p < vertOfFanTris.size(); ++p) {
			std::cout << vertOfFanTris[p] << " ";
		}
		std::cout << std::endl;



		for (int chk = 1; chk < vertOfFanTris.size(); ++chk) {

		}
		*/


		//‡”Ô‚É‚·‚é---------------------------------------------------------------
		std::sort(vertOfFanTris.begin(), vertOfFanTris.end());
		int swapid = -1;
		for (int c = 0; c < vertOfFanTris.size() - 1; ++c) {
			if (vertOfFanTris[c] + 1 != vertOfFanTris[c + 1]) {
				swapid = c;
			}
		}
		std::vector<int> tempSwap;
		if (swapid != -1) {
			for (int c = 0; c <= swapid; ++c) {
				tempSwap.push_back(vertOfFanTris[c]);
			}
			vertOfFanTris.erase(vertOfFanTris.begin(), vertOfFanTris.begin() + swapid);
			for (int c = 0; c < tempSwap.size(); ++c) {
				vertOfFanTris.push_back(tempSwap[c]);
			}
		}
		//----------------------------------------------------------------------
		for (int t = 0; t < vertOfFanTris.size(); ++t) {
			float leng = glm::length(_Vertices[vertOfFanTris[t]] - _Vertices[mididx]);
			_SumLengthFromAxis[mididx].push_back(leng);
		}
		for (int t = 1; t < vertOfFanTris.size(); ++t) {
			DeEdge we0 = { vertOfFanTris[t],vertOfFanTris[t - 1] };
			DeEdge we1 = { vertOfFanTris[t],mididx };
			DeEdge we2 = { vertOfFanTris[t - 1],mididx };
			//std::cout << "we0  " << we0.first << " "<<we0.second << std::endl;
			//std::cout << "we1  " << we1.first << " " << we1.second << std::endl;
			//std::cout << "we2  " << we2.first << " " << we2.second << std::endl;
			wireFrame.insert({ we0 });
			wireFrame.insert({ we1 });
			wireFrame.insert({ we2 });

			Triangle fantri = MakeTriangle(mididx, vertOfFanTris[t], vertOfFanTris[t - 1]);
			_TeddyTriangles_Outer.push_back(fantri);

		}

	}

	for (int i = 0; i < Tsize; ++i) {
		if (seen[i])continue;

		/*
		int v0 = _Triangles[i].id[0];
		int v1 = _Triangles[i].id[1];
		int v2 = _Triangles[i].id[2];

		if (invalidEdge[v0][v1] != true) {
			wireFrame.insert({ v0,v1 });
		}
		if (invalidEdge[v1][v2] != true) {
			wireFrame.insert({ v1,v2 });
		}
		if (invalidEdge[v2][v0] != true) {
			wireFrame.insert({ v2,v0 });
		}
		*/


		if (IsTerminal[i] == 3) {

			int v0 = _Triangles[i].id[0];
			int v1 = _Triangles[i].id[1];
			int v2 = _Triangles[i].id[2];

			int vmid = junctionMidPoint[i];
			if (vmid == -1) {
				glm::vec2 newV = _Vertices[v0] + _Vertices[v1] + _Vertices[v2];
				newV /= 3;
				_Vertices.push_back(newV);
				vmid = _Vertices.size() - 1;
				junctionMidPoint[i] = vmid;
				_IsChodralAxis[vmid] = true;
			}
			//length clc
			if (invalidEdge[v0][v1] == !true && invalidEdge[v1][v2] != true) {
				float leng = glm::length(_Vertices[v1] - _Vertices[vmid]);
				_SumLengthFromAxis[vmid].push_back(leng);
			}
			if (invalidEdge[v1][v2] == !true && invalidEdge[v2][v0] != true) {
				float leng = glm::length(_Vertices[v2] - _Vertices[vmid]);
				_SumLengthFromAxis[vmid].push_back(leng);
			}
			if (invalidEdge[v2][v0] == !true && invalidEdge[v0][v1] != true) {
				float leng = glm::length(_Vertices[v0] - _Vertices[vmid]);
				_SumLengthFromAxis[vmid].push_back(leng);
			}
			//------

			if (invalidEdge[v0][v1] != true) {
				int mp = edgeMidPoint[v0][v1];
				if (mp == -1) {
					glm::vec2 newv = _Vertices[v0] + _Vertices[v1];
					newv /= 2;
					_Vertices.push_back(newv);
					int idm = _Vertices.size() - 1;
					edgeMidPoint[v0][v1] = idm;
					edgeMidPoint[v1][v0] = idm;
					_IsChodralAxis[idm] = true;

					float leng1 = glm::length(_Vertices[v0] - _Vertices[idm]);
					_SumLengthFromAxis[idm].push_back(leng1);
					float leng2 = glm::length(_Vertices[v1] - _Vertices[idm]);
					_SumLengthFromAxis[idm].push_back(leng2);
				}
				int emid = edgeMidPoint[v0][v1];
				wireFrame.insert({ v0,emid });
				wireFrame.insert({ v0,vmid });
				wireFrame.insert({ vmid,emid });
				wireFrame.insert({ v1,emid });
				wireFrame.insert({ v1,vmid });

				Triangle tri1 = MakeTriangle(vmid, v0, emid);
				Triangle tri2 = MakeTriangle(vmid, v1, emid);
				_TeddyTriangles_Inner.push_back(tri1);
				_TeddyTriangles_Inner.push_back(tri2);
			}
			if (invalidEdge[v1][v2] != true) {
				int mp = edgeMidPoint[v1][v2];
				if (mp == -1) {
					glm::vec2 newv = _Vertices[v1] + _Vertices[v2];
					newv /= 2;
					_Vertices.push_back(newv);
					int idm = _Vertices.size() - 1;
					edgeMidPoint[v1][v2] = idm;
					edgeMidPoint[v2][v1] = idm;
					_IsChodralAxis[idm] = true;

					float leng1 = glm::length(_Vertices[v1] - _Vertices[idm]);
					_SumLengthFromAxis[idm].push_back(leng1);
					float leng2 = glm::length(_Vertices[v2] - _Vertices[idm]);
					_SumLengthFromAxis[idm].push_back(leng2);
				}
				int emid = edgeMidPoint[v1][v2];
				wireFrame.insert({ v1,emid });
				wireFrame.insert({ v1,vmid });
				wireFrame.insert({ vmid,emid });
				wireFrame.insert({ v2,emid });
				wireFrame.insert({ v2,vmid });

				Triangle tri1 = MakeTriangle(vmid, v1, emid);
				Triangle tri2 = MakeTriangle(vmid, v2, emid);
				_TeddyTriangles_Inner.push_back(tri1);
				_TeddyTriangles_Inner.push_back(tri2);
			}
			if (invalidEdge[v2][v0] != true) {
				int mp = edgeMidPoint[v2][v0];
				if (mp == -1) {
					glm::vec2 newv = _Vertices[v0] + _Vertices[v2];
					newv /= 2;
					_Vertices.push_back(newv);
					int idm = _Vertices.size() - 1;
					edgeMidPoint[v0][v2] = idm;
					edgeMidPoint[v2][v0] = idm;
					_IsChodralAxis[idm] = true;

					float leng1 = glm::length(_Vertices[v2] - _Vertices[idm]);
					_SumLengthFromAxis[idm].push_back(leng1);
					float leng2 = glm::length(_Vertices[v0] - _Vertices[idm]);
					_SumLengthFromAxis[idm].push_back(leng2);
				}
				int emid = edgeMidPoint[v0][v2];
				wireFrame.insert({ v0,emid });
				wireFrame.insert({ v0,vmid });
				wireFrame.insert({ vmid,emid });
				wireFrame.insert({ v2,emid });
				wireFrame.insert({ v2,vmid });

				Triangle tri1 = MakeTriangle(vmid, v2, emid);
				Triangle tri2 = MakeTriangle(vmid, v0, emid);
				_TeddyTriangles_Inner.push_back(tri1);
				_TeddyTriangles_Inner.push_back(tri2);
			}


		}
		else if (IsTerminal[i] == 2) {
			/*
			int v0 = _Triangles[i].id[0];
			int v1 = _Triangles[i].id[1];
			int v2 = _Triangles[i].id[2];

			if (invalidEdge[v0][v1] != true) {
				wireFrame.insert({ v0,v1 });
			}
			if (invalidEdge[v1][v2] != true) {
				wireFrame.insert({ v1,v2 });
			}
			if (invalidEdge[v2][v0] != true) {
				wireFrame.insert({ v2,v0 });
			}
			*/

			std::pair<int, int> ot = OuterEdge[i];
			int v = _Triangles[i].opposite({ ot.first,ot.second });

			int midp1 = -1;
			if (invalidEdge[v][ot.first] != true) {
				midp1 = edgeMidPoint[v][ot.first];
				if (midp1 == -1) {
					glm::vec2 newv = _Vertices[v] + _Vertices[ot.first];
					newv /= 2;
					_Vertices.push_back(newv);
					midp1 = _Vertices.size() - 1;
				}
				wireFrame.insert({ v,midp1 });
				wireFrame.insert({ midp1,ot.first });
				_IsChodralAxis[midp1] = true;
			}

			int midp2 = -1;
			if (invalidEdge[v][ot.second] != true) {
				midp2 = edgeMidPoint[v][ot.second];
				if (midp2 == -1) {
					glm::vec2 newv = _Vertices[v] + _Vertices[ot.second];
					newv /= 2;
					_Vertices.push_back(newv);
					midp2 = _Vertices.size() - 1;
				}
				wireFrame.insert({ v,midp2 });
				wireFrame.insert({ midp2,ot.second });
				_IsChodralAxis[midp2] = true;
			}

			if (midp1 != -1 && midp2 != -1) {
				wireFrame.insert({ midp1,midp2 });
			}
			if (invalidEdge[ot.first][ot.second] != true) {
				wireFrame.insert({ ot.first,ot.second });
				wireFrame.insert({ ot.first,midp1 });
				wireFrame.insert({ ot.second,midp1 });
			}

			if (invalidEdge[v][ot.first] != true && invalidEdge[v][ot.second] != true) {
				Triangle tri1 = MakeTriangle(v, midp1, midp2);
				_TeddyTriangles_Inner.push_back(tri1);
				Triangle tri2 = MakeTriangle(midp1, midp2, ot.first);
				_TeddyTriangles_Inner.push_back(tri2);
				Triangle tri3 = MakeTriangle(midp2, ot.first, ot.second);
				_TeddyTriangles_Outer.push_back(tri3);
			}

		}

	}
	//3Dlize---------------------------------------------------------
	int divNum = 2;
	float coef = 1.0f;
	float thickSize = 0.2;

	std::vector<int> indexOfAxisToIndexOf3D_Pozi(500, -1);//+z
	std::vector<int> indexOfAxisToIndexOf3D_Nega(500, -1);//-z
	std::vector<std::vector<std::vector<int>>> chkEdge(500, std::vector<std::vector<int>>(500, std::vector<int>(divNum, -1)));
	std::set<DeEdge> wf3D;
	for (int i = 0; i < _TeddyTriangles_Inner.size(); ++i) {
		Triangle tri = _TeddyTriangles_Inner[i];
		int notAxisPoint = -1;
		std::vector<int> AxisPoints;
		for (int c = 0; c < 3; ++c) {
			if (!_IsChodralAxis[tri.id[c]]) {
				notAxisPoint = tri.id[c];
			}
			else {
				AxisPoints.push_back(tri.id[c]);
			}
		}

		for (int c = 0; c < 2; ++c) {
			float thick = 0;
			int axisPoint = AxisPoints[c];

			if (_SumLengthFromAxis[axisPoint].size() != 0) {
				for (int s = 0; s < _SumLengthFromAxis[axisPoint].size(); ++s) {
					thick += _SumLengthFromAxis[axisPoint][s];
				}
				thick /= _SumLengthFromAxis[axisPoint].size();
				thick *= coef;
			}

			int vertPozi = indexOfAxisToIndexOf3D_Pozi[axisPoint];
			int vertNega = indexOfAxisToIndexOf3D_Nega[axisPoint];

			if (vertPozi == -1) {
				glm::vec3 newv = { _Vertices[axisPoint].x,_Vertices[axisPoint].y,thickSize };
				_TeddyVertices.push_back(newv);
				vertPozi = _TeddyVertices.size() - 1;
				indexOfAxisToIndexOf3D_Pozi[axisPoint] = vertPozi;
			}
			if (vertNega == -1) {
				glm::vec3 newv = { _Vertices[axisPoint].x,_Vertices[axisPoint].y,-thickSize };
				_TeddyVertices.push_back(newv);
				vertNega = _TeddyVertices.size() - 1;
				indexOfAxisToIndexOf3D_Nega[axisPoint] = vertNega;
			}
		}

		int v1_Pozi = indexOfAxisToIndexOf3D_Pozi[AxisPoints[0]];
		int v1_Nega = indexOfAxisToIndexOf3D_Nega[AxisPoints[0]];
		int v2_Pozi = indexOfAxisToIndexOf3D_Pozi[AxisPoints[1]];
		int v2_Nega = indexOfAxisToIndexOf3D_Nega[AxisPoints[1]];

		for (int edg = 0; edg < divNum; edg++) {
			int v1div = chkEdge[v1_Pozi][notAxisPoint][edg];
			if (v1div == -1) {
				glm::vec3 tem = _TeddyVertices[notAxisPoint] - _TeddyVertices[v1_Pozi];
				float co = (edg + 1.0) / (divNum + 1.0);
				tem *= co;
				glm::vec3 newv = _TeddyVertices[v1_Pozi] + tem;
				_TeddyVertices.push_back(newv);
				int id = _TeddyVertices.size() - 1;
				chkEdge[v1_Pozi][notAxisPoint][edg] = id;
				chkEdge[notAxisPoint][v1_Pozi][edg] = id;
			}
			int v2div = chkEdge[v2_Pozi][notAxisPoint][edg];
			if (v2div == -1) {
				glm::vec3 tem = _TeddyVertices[notAxisPoint] - _TeddyVertices[v2_Pozi];
				float co = (edg + 1.0) / (divNum + 1.0);
				tem *= co;
				glm::vec3 newv = _TeddyVertices[v2_Pozi] + tem;
				_TeddyVertices.push_back(newv);
				int id = _TeddyVertices.size() - 1;
				chkEdge[v2_Pozi][notAxisPoint][edg] = id;
				chkEdge[notAxisPoint][v2_Pozi][edg] = id;
			}
		}
		/*
		for (int edg = 0; edg < divNum; edg++) {
			int v1div = chkEdge[v1_Nega][notAxisPoint][edg];
			if (v1div == -1) {
				glm::vec3 tem = _TeddyVertices[notAxisPoint] - _TeddyVertices[v1_Nega];
				float co = (edg + 1.0) / (divNum + 1.0);
				tem *= co;
				glm::vec3 newv = _TeddyVertices[v1_Nega] + tem;
				_TeddyVertices.push_back(newv);
				int id = _TeddyVertices.size()-1;
				chkEdge[v1_Nega][notAxisPoint][edg] = id;
				chkEdge[notAxisPoint][v1_Nega][edg] = id;
			}
			int v2div = chkEdge[v2_Nega][notAxisPoint][edg];
			if (v2div == -1) {
				glm::vec3 tem = _TeddyVertices[notAxisPoint] - _TeddyVertices[v2_Nega];
				float co = (edg + 1.0) / (divNum + 1.0);
				tem *= co;
				glm::vec3 newv = _TeddyVertices[v2_Nega] + tem;
				_TeddyVertices.push_back(newv);
				int id = _TeddyVertices.size()-1;

				chkEdge[v2_Nega][notAxisPoint][edg] = id;
				chkEdge[notAxisPoint][v2_Nega][edg] = id;

			}

		}
		*/
		
		wf3D.insert({ v1_Pozi,chkEdge[v1_Pozi][notAxisPoint][0] });
		wf3D.insert({ v1_Pozi,v2_Pozi });
		wf3D.insert({ v2_Pozi,chkEdge[v2_Pozi][notAxisPoint][0] });
		//wf3D.insert({ chkEdge[v1_Pozi][notAxisPoint][0],chkEdge[v2_Pozi][notAxisPoint][0] });
		wf3D.insert({ v2_Pozi,chkEdge[v1_Pozi][notAxisPoint][0] });

		for (int itr = 0; itr < divNum - 1; ++itr) {
			wf3D.insert({ chkEdge[v1_Pozi][notAxisPoint][itr],chkEdge[v2_Pozi][notAxisPoint][itr] });
			wf3D.insert({ chkEdge[v1_Pozi][notAxisPoint][itr],chkEdge[v1_Pozi][notAxisPoint][itr+1] });
			wf3D.insert({ chkEdge[v2_Pozi][notAxisPoint][itr],chkEdge[v2_Pozi][notAxisPoint][itr + 1] });
			wf3D.insert({ chkEdge[v2_Pozi][notAxisPoint][itr],chkEdge[v1_Pozi][notAxisPoint][itr + 1] });
		}

		int endid = divNum - 1;
		wf3D.insert({ chkEdge[v1_Pozi][notAxisPoint][endid],chkEdge[v2_Pozi][notAxisPoint][endid] });
		wf3D.insert({notAxisPoint,chkEdge[v1_Pozi][notAxisPoint][endid] });
		wf3D.insert({ notAxisPoint,chkEdge[v2_Pozi][notAxisPoint][endid] });

		/*
		wf3D.insert({ v1_Pozi,v2_Pozi });
		wf3D.insert({ v1_Pozi,notAxisPoint });
		wf3D.insert({ v2_Pozi,notAxisPoint });
		wf3D.insert({ v1_Nega,v2_Nega });
		wf3D.insert({ v1_Nega,notAxisPoint });
		wf3D.insert({ v2_Nega,notAxisPoint });
		Triangle tri1 = MakeTeddyTriangle(v1_Pozi, v2_Pozi,notAxisPoint,false);
		Triangle tri2 = MakeTeddyTriangle(v1_Nega, v2_Nega, notAxisPoint,true);
		_TeddyIndices.push_back(tri1.id[0]);
		_TeddyIndices.push_back(tri1.id[1]);
		_TeddyIndices.push_back(tri1.id[2]);
		_TeddyIndices.push_back(tri2.id[0]);
		_TeddyIndices.push_back(tri2.id[1]);
		_TeddyIndices.push_back(tri2.id[2]);
		*/
		
	}
	
	for (int i = 0; i < _TeddyTriangles_Outer.size(); ++i) {
		Triangle tri = _TeddyTriangles_Outer[i];
		int axisPoint = -1;
		std::vector<int> outerEdgePoint;
		for (int c = 0; c < 3; ++c) {
			if (_IsChodralAxis[tri.id[c]]) {
				axisPoint = tri.id[c];
			}
			else {
				outerEdgePoint.push_back(tri.id[c]);
			}
		}
		float thick = 0;
		if (_SumLengthFromAxis[axisPoint].size() != 0) {
			for (int s = 0; s < _SumLengthFromAxis[axisPoint].size(); ++s) {
				thick += _SumLengthFromAxis[axisPoint][s];
			}
			thick /= _SumLengthFromAxis[axisPoint].size();
			thick *= coef;
		}

		int vertPozi = indexOfAxisToIndexOf3D_Pozi[axisPoint];
		int vertNega = indexOfAxisToIndexOf3D_Nega[axisPoint];

		if (vertPozi == -1) {
			glm::vec3 newv = { _Vertices[axisPoint].x,_Vertices[axisPoint].y,thickSize };
			_TeddyVertices.push_back(newv);
			vertPozi = _TeddyVertices.size() - 1;
			indexOfAxisToIndexOf3D_Pozi[axisPoint] = vertPozi;
		}
		if (vertNega == -1) {
			glm::vec3 newv = { _Vertices[axisPoint].x,_Vertices[axisPoint].y,-thickSize };
			_TeddyVertices.push_back(newv);
			vertNega = _TeddyVertices.size() - 1;
			indexOfAxisToIndexOf3D_Nega[axisPoint] = vertNega;
		}
		wf3D.insert({ vertPozi,outerEdgePoint[0] });
		wf3D.insert({ vertPozi,outerEdgePoint[1] });
		wf3D.insert({ outerEdgePoint[0],outerEdgePoint[1] });
		wf3D.insert({ vertNega,outerEdgePoint[0] });
		wf3D.insert({ vertNega,outerEdgePoint[1] });

		Triangle tri1 = MakeTeddyTriangle(vertPozi, outerEdgePoint[0], outerEdgePoint[1],false);
		Triangle tri2 = MakeTeddyTriangle(vertNega, outerEdgePoint[0], outerEdgePoint[1],true);

		_TeddyIndices.push_back(tri1.id[0]);
		_TeddyIndices.push_back(tri1.id[1]);
		_TeddyIndices.push_back(tri1.id[2]);
		_TeddyIndices.push_back(tri2.id[0]);
		_TeddyIndices.push_back(tri2.id[1]);
		_TeddyIndices.push_back(tri2.id[2]);

	}

	

	for (auto& e : wireFrame) {
		_WireIdx.emplace_back(e.first);
		_WireIdx.emplace_back(e.second);
	}

	for (auto& e : wf3D) {
		_TeddyWireIdx.emplace_back(e.first);
		_TeddyWireIdx.emplace_back(e.second);
	}

	for (int i = 0; i < _TeddyVertices.size(); ++i) {
		float uvx = (_TeddyVertices[i].x * 0.5) + 0.5;
		float uvy = 1.0 - ((_TeddyVertices[i].y * 0.5) + 0.5);

		_TeddyUV.push_back(uvx);
		_TeddyUV.push_back(uvy);
	}

}