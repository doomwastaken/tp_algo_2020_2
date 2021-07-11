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

class ListGraph : public IGraph {
 public:
    explicit ListGraph(int n) : adjacency_lists(n) {}
    explicit ListGraph(const IGraph &graph);
    ~ListGraph() {}

    void AddEdge(int from, int to) override;
    int VerticesCount() const override;
    std::vector<int> GetNextVertices(int vertex) const override;
    std::vector<int> GetPrevVertices(int vertex) const override;

 private:
    std::vector<std::vector<int>> adjacency_lists;
};

ListGraph::ListGraph(const IGraph &graph) : adjacency_lists(graph.VerticesCount()) {
    for (unsigned int i = 0; i < adjacency_lists.size(); i++) {
        adjacency_lists[i] = graph.GetNextVertices(i);
    }
}

void ListGraph::AddEdge(int from, int to) {
    assert(from >= 0 && from < this->VerticesCount());
    assert(to >= 0 && to < this->VerticesCount());

    adjacency_lists[from].push_back(to);
}

int ListGraph::VerticesCount() const {
    return (int)adjacency_lists.size();
}

std::vector<int> ListGraph::GetPrevVertices(int vertex) const {
    assert(vertex >= 0 && vertex < this->VerticesCount());

    std::vector<int> prev_vertices;

    for (unsigned int from = 0; from < adjacency_lists.size(); from++) {
        for (int to : adjacency_lists[from]) {
            if (to == vertex) {
                prev_vertices.push_back(from);
            }
        }
    }
    return prev_vertices;
}

std::vector<int> ListGraph::GetNextVertices(int vertex) const {
    assert(0 <= vertex && vertex < this->VerticesCount());
    return adjacency_lists[vertex];
}

class MatrixGraph : public IGraph {
 public:
    explicit MatrixGraph(int n) : adjacency_matrix(n, std::vector<bool>(n, false)) {}
    explicit MatrixGraph(const IGraph &graph);
    ~MatrixGraph() {}

    void AddEdge(int from, int to) override;
    int VerticesCount() const override;
    std::vector<int> GetNextVertices(int vertex) const override;
    std::vector<int> GetPrevVertices(int vertex) const override;

 private:
    std::vector<std::vector<bool>> adjacency_matrix;
};

MatrixGraph::MatrixGraph(const IGraph &graph) : adjacency_matrix(graph.VerticesCount(),
                            std::vector<bool>(graph.VerticesCount(), false)) {
    for (int from = 0; from < graph.VerticesCount(); from++) {
        for (int to : graph.GetNextVertices(from)) {
            this->AddEdge(from, to);
        }
    }
}

int MatrixGraph::VerticesCount() const {
    return (int)adjacency_matrix.size();
}

void MatrixGraph::AddEdge(int from, int to) {
    assert(from >= 0 && from < this->VerticesCount());
    assert(to >= 0 && to < this->VerticesCount());

    adjacency_matrix[from][to] = true;
}

std::vector<int> MatrixGraph::GetNextVertices(int vertex) const {
    assert(vertex >= 0 && vertex < this->VerticesCount());

    std::vector<int> next_vertices;

    for (unsigned int i = 0; i < adjacency_matrix[vertex].size(); i++) {
        if (adjacency_matrix[vertex][i]) {
            next_vertices.push_back(i);
        }
    }

    return next_vertices;
}

std::vector<int> MatrixGraph::GetPrevVertices(int vertex) const {
    assert(vertex >= 0 && vertex < this->VerticesCount());

    std::vector<int> prev_vertices;

    for (unsigned int i = 0; i < adjacency_matrix.size(); i++) {
        if (adjacency_matrix[i][vertex]) {
            prev_vertices.push_back(i);
        }
    }

    return prev_vertices;
}


class SetGraph : public IGraph {
 public:
    explicit SetGraph(int n) : edges(n), number_of_vertices(n) {}
    explicit SetGraph(const IGraph &graph);
    ~SetGraph() {}

    void AddEdge(int from, int to) override;
    int VerticesCount() const override;
    std::vector<int> GetNextVertices(int vertex) const override;
    std::vector<int> GetPrevVertices(int vertex) const override;

 private:
    std::vector<std::unordered_set<int>> edges;
    int number_of_vertices;
};

void print(const IGraph &graph) {
    for (int vertex = 0; vertex < graph.VerticesCount(); vertex++) {
        for (int i : graph.GetNextVertices(vertex)) {
            std::cout << "Edge from " << vertex << " to " << i << std::endl;
        }
    }

    std::cout << std::endl;
}

SetGraph::SetGraph(const IGraph &graph) : edges(graph.VerticesCount()), number_of_vertices(graph.VerticesCount()) {
    for (int from = 0; from < graph.VerticesCount(); from++) {
        for (int to : graph.GetNextVertices(from)) {
            this->AddEdge(from, to);
        }
    }
}

void SetGraph::AddEdge(int from, int to) {
    assert(from >= 0 && from < this->VerticesCount());
    assert(to >= 0 && to < this->VerticesCount());

    edges[from].insert(to);
}

int SetGraph::VerticesCount() const {
    return number_of_vertices;
}

std::vector<int> SetGraph::GetNextVertices(int vertex) const {
    assert(vertex >= 0 && vertex < this->VerticesCount());

    return std::vector<int>(edges[vertex].begin(), edges[vertex].end());
}

std::vector<int> SetGraph::GetPrevVertices(int vertex) const {
    assert(vertex >= 0 && vertex < this->VerticesCount());

    std::vector<int> prev_vertices;

    for (size_t i = 0; i < edges.size(); i++) {
        if (edges[i].find(vertex) != edges[i].end()) {
            prev_vertices.push_back(i);
        }
    }

    return prev_vertices;
}


int main(int argc, const char * argv[]) {
    ListGraph list_graph(8);
    list_graph.AddEdge(0, 1);
    list_graph.AddEdge(1, 2);
    list_graph.AddEdge(1, 3);
    list_graph.AddEdge(1, 6);
    list_graph.AddEdge(3, 2);
    list_graph.AddEdge(3, 4);
    list_graph.AddEdge(3, 6);
    list_graph.AddEdge(5, 6);


    std::cout << "list_graph:" << std::endl;
    print(list_graph);

    // Создание ListGraph из другого графа
    ListGraph list_graph_copy(list_graph);
    std::cout << "copy list_graph:" << std::endl;
    print(list_graph_copy);

    // Создание MatrixGraph из другого графа
    MatrixGraph matrix_graph(list_graph);
    std::cout << "matrix graph:" << std::endl;
    print(matrix_graph);

    // Создание ArcGraph из другого графа
    ArcGraph arc_graph(matrix_graph);
    std::cout << "arc graph:" << std::endl;
    print(arc_graph);

    // Создание SetGraph из другого графа
    SetGraph set_graph(arc_graph);
    std::cout << "set graph:" << std::endl;
    print(set_graph);

    return 0;
}
