#include"GraphCut.h"

Dinic::Dinic(int input_maxv):maxv(input_maxv) {
	G.resize(input_maxv);
	level.resize(input_maxv);
	iter.resize(input_maxv);
}
Dinic::~Dinic() {

}

void Dinic::AddEdgeBoth(int from, int to, double cap) {
	const int rev_from = G[from].size();
	const int rev_to = G[to].size();
	G[from].push_back({ to, cap, rev_to });
	G[to].push_back({ from,cap,rev_from });

}

void Dinic::AddEdge(int from, int to, double cap) {
	const int rev_from = G[from].size();
	const int rev_to = G[to].size();
	G[from].push_back({ to, cap, rev_to });
	G[to].push_back({ from,0,rev_from });
}

double Dinic::MaxFlow(int s, int t) {
	double flow = 0;
	for (;;) {
		bfs(s);
		if (level[t] < 0)break;
		std::fill(iter.begin(), iter.end(), 0);
		double f;
		while ((f = dfs(s, t, INF)) > 0); {
			flow += f;
		}
	}
	return flow;

}

std::vector<bool> Dinic::GetNodes(int s) {
	std::vector<bool> ret(maxv);
	std::queue<int> que;
	que.push(s);
	while (!que.empty()) {
		int v = que.front();
		que.pop();
		ret[v] = true;

		for (int i = 0; i < G[v].size(); ++i){
			Edge& e = G[v][i];
			if (e.cap > 0 && !ret[e.to]) {
				que.push(e.to);
			}
		}

	}
	return ret;
}


void Dinic::bfs(int s) {
	fill(level.begin(), level.end(), -1);
	std::queue<int> que;
	level[s] = 0;

	que.push(s);
	while (!que.empty()) 
	{
		int v = que.front();
		que.pop();
		for (int i = 0; i < G[v].size(); ++i) {
			Edge& e = G[v][i];
			if (e.cap > 0 && level[e.to] < 0) {
				level[e.to] = level[v] + 1;
				que.push(e.to);
			}
		}
	}

}

double Dinic::dfs(int v, int t, double f) {
	if (v == t)return f;
	for (int &i= iter[v]; i < G[v].size(); ++i) {
		Edge& e = G[v][i];
		if (e.cap > 0 && level[v] < level[e.to]) {
			double d = dfs(e.to, t, std::min(f, e.cap));

			if (d > 0) {
				e.cap -= d;
				G[e.to][e.rev].cap += d;
				return d;
			}
		}
	}
	return 0;
}