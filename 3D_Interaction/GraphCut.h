#pragma once

#include <vector>
#include <queue>
#include <algorithm>


struct Edge {
	int to;
	double cap;
	int rev;
};

class Dinic {

public:
	Dinic(int input_maxv);
	~Dinic();

	void AddEdgeBoth(int from, int to, double cap);
	void AddEdge(int from, int to, double cap);

	double MaxFlow(int s, int t);
	std::vector<bool> GetNodes(int s);
private:

	void bfs(int s);
	double dfs(int v, int t, double f);

	const double INF = 100000000000000;

	int maxv;
	std::vector<std::vector<Edge>> G;
	std::vector<int> level;
	std::vector<int> iter;



};