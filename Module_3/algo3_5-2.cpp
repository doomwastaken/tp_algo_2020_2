#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <assert.h>
#include <ctime>
#include <cmath>
#include <chrono>
#include <time.h>

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
	double Kruskal();
	double Brute();

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

double ArcGraphM::Brute() {
	std::vector<int> edges;
	for (int j = 0; j < numEdges; j++) {
		edges.push_back(j);
	}
	std::vector<double> weights;
	int treeFactor = 0;

	do {
		double weight = 0;
		for (size_t i = 0; i < edges.size() - 1; ++i) {
			double res = getWeight(edges[i], edges[i + 1]);
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
	} while (std::next_permutation(edges.begin(), edges.end()));

	return *std::min_element(weights.begin(), weights.end());
}

const int ArcGraphM::getNumEdges() const {
	return numEdges;
}

double ArcGraphM::Kruskal() {
	double weight = 0;
	std::sort(edges.begin(), edges.end());
	CDSU ds(numEdges);

	for (auto i : edges) {
		int u = i.second.first;
		int v = i.second.second;

		int setU = ds.find(u);
		int setV = ds.find(v);

		if (setU != setV) {
			weight += i.first;
			ds.merge(setU, setV);
		}
	}

	return weight;
}

double ranf() {
	return ((double)std::rand() / RAND_MAX);
}

double boxMueller(float seed, float s) {
	double x1 = 0;
	double x2 = 0;
	double w = 0;
	double y1 = 0;
	double y2 = 0;
	bool last = false;

	if (last) {
		y1 = y2;
		last = false;
	} else {
		do {
			x1 = 2.0 * ranf() - 1.0;
			x2 = 2.0 * ranf() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while (w >= 1.0);

		w = std::sqrt((-2.0 * std::log(w)) / w);
		y1 = x1 * w;
		y2 = x2 * w;
		last = true;
	}
	return (seed + y1 * s);
}

std::pair<double, double> test(int test_num) {
	assert(test_num >= 2 && test_num <= 13);

	ArcGraphM graph(test_num);
	srand(time(NULL));
	int availablePairs = rand() % 50;
	static int maxWeight = 100;
	for (int j = 0; j < graph.getNumEdges() - 1; ++j) {
		graph.addEdge(j, j + 1, maxWeight);
	}

	for (int j = 0; j <= availablePairs; ++j) {
		int a = rand() % graph.getNumEdges();
		int b = rand() % graph.getNumEdges();
		double w = std::fabs(boxMueller(42, 1));
		while (a == b) {
			b = rand() % graph.getNumEdges();
		}
		if (!graph.setWeight(a, b, w)) {
			graph.addEdge(a, b, w);
		}
	}

	return { graph.Brute(), graph.Kruskal() };
}

int main() {
	auto start = std::chrono::system_clock::now();
	std::vector<std::pair<double, double>> results;  // Brute Kruskal

	for (int i = 2; i < 13; ++i) {
		results.push_back(test(i));

		int mean = 0;
		for (auto data : results) {
			mean += data.first - data.second;
		}
		mean /= results.size();
		int deviation = 0;
		for (auto data : results) {
			int cur = data.second - data.second;
			cur -= mean;
			cur = cur * cur;
			deviation += cur;
		}

		if (mean) {
			deviation = std::sqrt(deviation /= mean);
		} else {
			deviation = 0;
		}

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-start;
		std::cout << elapsed_seconds.count() << " Test #" << i << ". Mean: " << mean << ", Deviation: " << deviation << std::endl;
		start = std::chrono::system_clock::now();
	}

	return 0;
}