#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <assert.h>


class CDSU {
 public:
	explicit CDSU(int n);
	int find(int u);
	void merge(int u, int v);

 private:
	std::vector<int> parent;
	std::vector<int> rank;
};

CDSU::CDSU(int n) : parent(n), rank(n, 1) {
	for (int i = 0; i < n; ++i) {
		parent[i] = i;
	}
}

int CDSU::find(int u) {
	if (parent[u] == u) {
		return u;
	}

	return parent[u] = find(parent[u]);
}

void CDSU::merge(int u, int v) {
	const int rootU = find(u);
	const int rootV = find(v);

	if (rank[rootU] < rank[rootV]) {
		parent[rootU] = rootV;
	} else {
		if (rank[rootU] == rank[rootV]) {
			++rank[rootU];
		}

		parent[rootV] = rootU;
	}
}

class ArcGraphM {
 public:
	explicit ArcGraphM(int n) : numEdges(n) {}

	void addEdge(int u, int v, double w);

	bool setWeight(int u, int v, double w);
	double getWeight(int u, int v);
	const int getNumEdges() const;

	double Kruskal();		// возвращает вес МОД, полученного методом Крускала
	double bruteforceMST();		// возвращает вес МОД, полученного методом перебора

 private:
	int numEdges;
	std::vector<std::pair<double, std::pair<int, int>>> edges;
};

void ArcGraphM::addEdge(int u, int v, double w) {
	edges.push_back({ w, {u, v} });
}

bool ArcGraphM::setWeight(int u, int v, double w) {
	for (auto& it : edges) {
		if ((u == it.second.first && v == it.second.second)
		 || (u == it.second.second && v == it.second.first)) {
			it.first = w;
			return true;
		}
	}

	return false;
}

double ArcGraphM::getWeight(int u, int v) {
	for (auto& it : edges) {
		if ((u == it.second.first && v == it.second.second) || (u == it.second.second &&
										  v == it.second.first)) {
			return it.first;
		}
	}

	return -1;
}

const int ArcGraphM::getNumEdges() const {
	return numEdges;
}

double ArcGraphM::Kruskal() {
	double mstWeight = 0;
	std::sort(edges.begin(), edges.end());
	CDSU ds(numEdges);

	for (auto i : edges) {
		int u = i.second.first;
		int v = i.second.second;

		int setU = ds.find(u);
		int setV = ds.find(v);

		if (setU != setV) {
			mstWeight += i.first;
			ds.merge(setU, setV);
		}
	}

	return mstWeight;
}

double ArcGraphM::bruteforceMST() {
	std::vector<int> vertexContainer;
	for (int j = 0; j < numEdges; j++) {
		vertexContainer.push_back(j);
	}

	std::vector<double> weights;

	int treeFactor = 0;
	do {
		double weight = 0;
		for (size_t i = 0; i < vertexContainer.size() - 1; ++i) {
			double res = getWeight(vertexContainer[i], vertexContainer[i + 1]);
			if (res != -1) {
				weight += res;
				++treeFactor;
			} else {
				break;
			}
		}

		if (treeFactor == numEdges - 1) {
			weights.push_back(weight);
		}

		treeFactor = 0;
	} while (std::next_permutation(vertexContainer.begin(), vertexContainer.end()));

	return *std::min_element(weights.begin(), weights.end());
}

int main() {
	int n = 0;
	assert(std::cin >> n && n >= 1 && n <= 20000);

	int m = 0;
	assert(std::cin >> m && m >= 0 && m <= 100000);

	ArcGraphM graph(n + 1);

	for (int i = 0; i < m; ++i) {
		int from = 0;
		int to = 0;
		int weight = 0;
		assert(std::cin >> from >> to >> weight);
		graph.addEdge(from, to, weight);
	}

	std::cout << graph.Kruskal();

	return 0;
}
