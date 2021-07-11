#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <memory>
#include <queue>

class ListGraph {
 public:
    explicit ListGraph(int n) : adjacency_lists(n) {}
    ~ListGraph() {}

    void AddEdge(int from, int to) {
        assert(from >= 0 && from < this->VerticesCount());
        assert(to >= 0 && to < this->VerticesCount());

        adjacency_lists[from].push_back(to);
    }

    int VerticesCount() const {
        return (int)adjacency_lists.size();
    }

    std::vector<int> GetNextVertices(int vertex) const {
        assert(0 <= vertex && vertex < this->VerticesCount());
        return adjacency_lists[vertex];
    }

    std::vector<int> GetPrevVertices(int vertex) const {
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

 private:
    std::vector<std::vector<int>> adjacency_lists;
};

int numOfMinPath(const ListGraph &graph, unsigned short int from, unsigned short int to) {
    assert(from >= 0 && from < graph.VerticesCount());
    assert(to >= 0 && to < graph.VerticesCount());

    std::vector<bool> visited(graph.VerticesCount(), false);
    std::vector<int> min_path(graph.VerticesCount(), INT8_MAX);
    std::vector<int> count_of_paths(graph.VerticesCount(), -1);
    count_of_paths[from] = 1;
    min_path[from] = 0;

    std::queue<unsigned short int> queue;
    queue.push(from);

    while (!queue.empty()) {
        unsigned short int cur = queue.front();
        queue.pop();
        visited[cur] = true;

        for (unsigned short int vertex : graph.GetNextVertices(cur)) {
            if (visited[vertex]) {
                if (min_path[vertex] == min_path[cur] + 1) {
                    count_of_paths[vertex] += count_of_paths[cur];
                } else if (min_path[vertex] > min_path[cur] + 1) {
                    count_of_paths[vertex] = count_of_paths[cur];
                    min_path[vertex] = min_path[cur] + 1;
                }
            } else {
                queue.push(vertex);
                min_path[vertex] = min_path[cur] + 1;
                count_of_paths[vertex] = count_of_paths[cur];
                visited[vertex] = true;
            }
        }
    }

    return count_of_paths[to] == -1 ? 0 : count_of_paths[to];
}

void run(std::istream &in, std::ostream &out) {
    unsigned short int num_of_vertices = 0;
    int num_of_edges = 0;
    in >> num_of_vertices >> num_of_edges;

    ListGraph graph = ListGraph(num_of_vertices);

    for (int i = 0; i < num_of_edges; i++) {
        int edge_from = 0;
        int edge_to = 0;
        in >> edge_from >> edge_to;

        graph.AddEdge(edge_from, edge_to);
        graph.AddEdge(edge_to, edge_from);
    }

    int from = 0;
    int to = 0;
    in >> from >> to;

    out << numOfMinPath(graph, from, to);
}

void test() {
    {
        std::stringstream input;
        input << "4" << std::endl;
        input << "5" << std::endl;
        input << "0 1" << std::endl;
        input << "0 2" << std::endl;
        input << "1 2" << std::endl;
        input << "1 3" << std::endl;
        input << "2 3" << std::endl;
        input << std::endl;
        input << "0 3" << std::endl;

        std::stringstream output;

        run(input, output);

        std::cout << "Output:\n" << output.str() << std::endl;

        std::stringstream answer;
        answer << "2";

        std::cout << "Answer:\n" << answer.str() << std::endl;

        assert(output.str() == answer.str());
    }
}

int main() {
    // test();
    run(std::cin, std::cout);
    return 0;
}
