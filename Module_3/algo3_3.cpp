
#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <queue>

typedef std::pair<int, int> edge;

class EdgeComparator {
 public:
    int operator() (const edge &e1, const edge &e2) {
        return e1.second > e2.second;
    }
};

class ListGraph {
 public:
    explicit ListGraph(int n) : adjacency_lists(n) {}
    ~ListGraph() {}

    void AddEdge(int from, int to, int val) {
        assert(from >= 0 && from < this->VerticesCount());
        assert(to >= 0 && to < this->VerticesCount());

        adjacency_lists[from].push_back(edge(to, val));
    }

    int VerticesCount() const {
        return (int)adjacency_lists.size();
    }

    std::vector<edge> GetNextVertices(int vertex) const {
        assert(0 <= vertex && vertex < this->VerticesCount());
        return adjacency_lists[vertex];
    }

    std::vector<edge> GetPrevVertices(int vertex) const {
        assert(vertex >= 0 && vertex < this->VerticesCount());

        std::vector<edge> prev_vertices;

        for (unsigned int from = 0; from < adjacency_lists.size(); from++) {
            for (edge to : adjacency_lists[from])  {
                if (to.first == vertex) {
                    prev_vertices.push_back(edge(from, to.second));
                }
            }
        }
        return prev_vertices;
    }

 private:
    std::vector<std::vector<edge>> adjacency_lists;
};

int MinPath(const ListGraph &graph, int from, int to) {
    assert(from >= 0 && from < graph.VerticesCount());
    assert(to >= 0 && to < graph.VerticesCount());

    std::vector<int> min_path(graph.VerticesCount(), INT32_MAX);
    min_path[from] = 0;
    std::vector<int> from_vertex(graph.VerticesCount(), -1);

    std::priority_queue<edge, std::vector<edge>, EdgeComparator> min_heap;
    min_heap.push(edge(from, 0));

    while (!min_heap.empty()) {
        edge cur = min_heap.top();
        min_heap.pop();

        for (edge next : graph.GetNextVertices(cur.first)) {
            if (min_path[next.first] == INT32_MAX) {
                min_path[next.first] = min_path[cur.first] + next.second;
                from_vertex[next.first] = cur.first;
                min_heap.push(edge(next.first, min_path[next.first]));
            } else if (min_path[next.first] > min_path[cur.first] + next.second) {
                min_path[next.first] = min_path[cur.first] + next.second;
                from_vertex[next.first] = cur.first;
                min_heap.push(edge(next.first, min_path[next.first]));
            }
        }
    }

    return min_path[to];
}

void run(std::istream &in, std::ostream &out) {
    unsigned short int num_of_vertices = 0;
    int num_of_edges = 0;
    in >> num_of_vertices >> num_of_edges;

    ListGraph graph = ListGraph(num_of_vertices);

    for (int i = 0; i < num_of_edges; i++) {
        int edge_from = 0;
        int edge_to = 0;
        int edge_val = 0;
        in >> edge_from >> edge_to >> edge_val;

        graph.AddEdge(edge_from, edge_to, edge_val);
        graph.AddEdge(edge_to, edge_from, edge_val);
    }

    int from = 0;
    int to = 0;
    in >> from >> to;

    out << MinPath(graph, from, to);
}

void test() {
    {
        std::stringstream input;
        input << "6" << std::endl;
        input << "9" << std::endl;
        input << "0 3 1" << std::endl;
        input << "0 4 2" << std::endl;
        input << "1 2 7" << std::endl;
        input << "1 3 2" << std::endl;
        input << "1 4 3" << std::endl;
        input << "1 5 3" << std::endl;
        input << "2 5 3" << std::endl;
        input << "3 4 4" << std::endl;
        input << "3 5 6" << std::endl;
        input << "0 2" << std::endl;

        std::stringstream output;

        run(input, output);

        std::cout << "Output:\n" << output.str() << std::endl;

        std::stringstream answer;
        answer << "9";

        std::cout << "Answer:\n" << answer.str() << std::endl;

        assert(output.str() == answer.str());
    }
}

int main() {
    // test();
    run(std::cin, std::cout);
    return 0;
}
