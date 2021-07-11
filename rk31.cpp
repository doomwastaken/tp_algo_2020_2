#include <set>
#include <vector>
#include <unordered_set>
#include <iostream>
#include <cassert>

struct IGraph {
    virtual ~IGraph() {}

    // Добавление ребра от from к to.
    virtual void AddEdge(int from, int to) = 0;

    virtual int VerticesCount() const  = 0;

    virtual std::vector<int> GetNextVertices(int vertex) const = 0;
    virtual std::vector<int> GetPrevVertices(int vertex) const = 0;
};


class ArcGraph : public IGraph {
 public:
    explicit ArcGraph(int n);
    explicit ArcGraph(const IGraph &graph);
    ~ArcGraph() {}

    void AddEdge(int from, int to) override;
    int VerticesCount() const override;
    std::vector<int> GetNextVertices(int vertex) const override;
    std::vector<int> GetPrevVertices(int vertex) const override;

 private:
    std::set<int> vertices;
    std::vector<std::pair<int, int>> pairs_vector;
};

ArcGraph::ArcGraph(const IGraph &graph) {
    for (int i = 0; i < graph.VerticesCount(); i++) {
        vertices.insert(i);
    }

    for (int from = 0; from < graph.VerticesCount(); from++) {
        for (int to : graph.GetNextVertices(from)) {
            this->AddEdge(from, to);
        }
    }
}

void ArcGraph::AddEdge(int from, int to) {
    assert(from >= 0 && from < this->VerticesCount());
    assert(to >= 0 && to < this->VerticesCount());

    pairs_vector.push_back(std::pair<int, int>(from, to));
}

int ArcGraph::VerticesCount() const {
    return vertices.size();
}

std::vector<int> ArcGraph::GetNextVertices(int vertex) const {
    assert(vertex >= 0 && vertex < this->VerticesCount());

    std::vector<int> next_vertices;

    for (std::pair<int, int> edge : pairs_vector) {
        if (edge.first == vertex) {
            next_vertices.push_back(edge.second);
        }
    }

    return next_vertices;
}

std::vector<int> ArcGraph::GetPrevVertices(int vertex) const {
    assert(vertex >= 0 && vertex < this->VerticesCount());
    std::vector<int> prev_vertices;
    for (std::pair<int, int> edge : pairs_vector) {
        if (edge.second == vertex) {
            prev_vertices.push_back(edge.first);
        }
    }

    return prev_vertices;
}

ArcGraph::ArcGraph(int n) {
    for (int i = 0; i < n; i++) {
        vertices.insert(i);
    }
}

int main() {
    int n = 0;
    int m = 0;
    int a = 0, b = 0;
    std::vector<int> visited;
    std::cin >> n >> m;
    ArcGraph graph(n);
    for (auto i = 0; i < m; ++i)
        graph.AddEdge(n, m);
    if (n > m)
        return 0;
    else {
        for (auto i = 0; i < m; ++i) {
            graph.GetNextVertices(i)
        }

    }
    
    return 0;
}
