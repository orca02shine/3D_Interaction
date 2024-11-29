#include "Delauney.h"

Delauney::Delauney(Contour contour,int texSize):_Contour(contour),_TexSize(texSize)
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

	SetData();

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

		float X = (contour[i].x / (w/2.0))-1.0f;
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

void Delauney::MakeTeddyTempVerts() {

	int numVerts = _Vertices.size()-3;

	std::vector<std::vector<bool>> chk(numVerts, std::vector<bool>(numVerts, false));

	for (int i = 0; i < _Triangles.size(); i++) {
		int k0 = _Triangles[i].id[0];
		int k1 = _Triangles[i].id[1];
		int k2 = _Triangles[i].id[2];

		if (k0 < 3 || k1 < 3 || k2 < 3)continue;

		//³‚µ‚¢’¸“_‚Ìî•ñ
		k0 -= 3;
		k1 -= 3;
		k2 -= 3;
		
		std::vector<int>  vertid(3, -1);

		if ((k2 + 1) % numVerts != k1) {

			chk[k2][k1] = true;
			chk[k1][k2] = true;
		}
		if ((k1 + 1) % numVerts != k0) {

			chk[k1][k0] = true;
			chk[k0][k1] = true;
		}
		if ((k0 + 1) % numVerts != k2) {

			chk[k0][k2] = true;
			chk[k2][k0] = true;
		}
	

	}



}