/*Дано число N < 106 и последовательность пар целых чисел из [-231..231] длиной N.
Построить декартово дерево из N узлов, характеризующихся парами чисел {Xi, Yi}.
Каждая пара чисел {Xi, Yi} определяет ключ Xi и приоритет Yi в декартовом дереве.
Добавление узла в декартово дерево выполняйте второй версией алгоритма, рассказанного на лекции:
При добавлении узла (x, y) выполняйте спуск по ключу до узла P с меньшим приоритетом.
Затем разбейте найденное поддерево по ключу x так, чтобы в первом
поддереве все ключи меньше x, а во втором больше или равны x.
Получившиеся два дерева сделайте дочерними для нового узла (x, y).
 Новый узел вставьте на место узла P.

Построить также наивное дерево поиска по ключам Xi методом из задачи 2.

3_1. Вычислить разницу глубин наивного дерева поиска и декартового дерева.
Разница может быть отрицательна. */

#include <iostream>
#include <sstream>
#include <cassert>
#include <queue>
#include <stack>

template <typename T>
struct Node {
    T value;
    Node *left;
    Node *right;
    int height;
    explicit Node(T value) : value(value), left(nullptr), right(nullptr), height(1) {}
};

template <typename T>
std::ostream& operator<<(std::ostream& out, const Node<T>& node) {
    out << node.value;
    return out;
}

template <typename T, typename comparator = std::less_equal<T>>
class BinTree {
 public:
    BinTree(): root(nullptr) {}
    ~BinTree() {
        destroyTree(root);
    }
    int getDepth() {
        return getDepthInternal(root);
    }
    void Insert(T value) {
        insertInternal(root, value);
    }

    void Print(std::ostream &out) {
        if (!root) {
            return;
        }

        std::stack<Node<T> *> buf;
        buf.push(root);

        while (!buf.empty()) {
            Node<T> *tmp = buf.top();
            buf.pop();

            out << *tmp << ' ';

            if (tmp->right) {
                buf.push(tmp->right);
            }
            if (tmp->left) {
                buf.push(tmp->left);
            }
        }
    }

 private:
    Node<T> *root;
    comparator cmp;
    void destroyTree(Node<T> *node) {
        if (!root) {
            return;
        }

        std::queue<Node<T> *> buf;
        buf.push(root);

        while (!buf.empty()) {
            Node<T> *tmp = buf.front();
            buf.pop();

            if (tmp->left) {
                buf.push(tmp->left);
            }
            if (tmp->right) {
                buf.push(tmp->right);
            }

            delete(tmp);
        }
    }
    void insertInternal(Node<T>* node, const T& value) {
        Node<T> *newNode = new Node(value);

        if (!node) {
            root = newNode;
            return;
        }

        while ( (cmp(node->value, newNode->value) && node->right) || (!cmp(node->value, newNode->value) && node->left) ) {
            node = cmp(node->value, newNode->value) ? node->right : node->left;
            (newNode->height)++;
        }

        (newNode->height)++;
        if ( cmp(node->value, value) ) {
            node->right = newNode;
        } else {
            node->left = newNode;
        }
    }

    int getDepthInternal(Node<T> *node) {
        int maxHeight = 0;

        std::stack<Node<T>*> nodes;

        if (!node) {
            return 0;
        } else {
            nodes.push(node);

            while (!nodes.empty()) {
                node = nodes.top();
                nodes.pop();

                if (node->right != nullptr) {
                    nodes.push(node->right);
                }
                if (node->left != nullptr) {
                    nodes.push(node->left);
                }

                if (maxHeight < node->height) {
                    maxHeight = node->height;
                }
            }
        }

        return maxHeight;
    }
};

template <typename T = int, typename comparator = std::less<T>>
class DecartTree {
    struct Node {
        Node(const T& data, const int& priority)
            : data(data), priority(priority), left(nullptr), right(nullptr) {}

        T data;
        int priority;
        Node *left;
        Node *right;
    };

 public:
    DecartTree() : root(nullptr) {}
    ~DecartTree() {
        destroyTree(root);
    }

    comparator cmp;

    void Insert(const T& data, const int& priority) {
        Node *tmp = new Node(data, priority);
        insertInternal(root, tmp);
    }
    int getDepth() {
        return getDepthInternal(root);
    }

 private:
    Node *root;

    void destroyTree(Node *node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

    // Разделяет найденное дерево узла node на два поддерева left и right, где:
    // left = все элементы < data,
    // right = все элементы >= data.
    void split(Node* node, const int& data, Node* &left, Node* &right) {
        if (!node) {
            left = nullptr;
            right = nullptr;
        } else {
            if (data < node->data) {
                split(node->left, data, left, node->left);
                right = node;
            } else {
                split(node->right, data, node->right, right);
                left = node;
            }
        }
    }

    void insertInternal(Node* &node, Node* tmp) {
        if (!node) {
            node = tmp;
            return;
        }

        if (node->priority < tmp->priority) {
            // если нашли нужный узел с меньшим приоритетом, чем у добавляемого элемента,
            // то разрезаем дерево на два поддерева, которые станут l и r потомками добавляемого узла tmp
            split(node, tmp->data, tmp->left, tmp->right);
            node = tmp;
        } else {
            if ( cmp(tmp->data, node->data) ) {
                insertInternal(node->left, tmp);
            } else {
                insertInternal(node->right, tmp);
            }
        }
    }

    int getDepthInternal(DecartTree::Node *node) {
        if (!node) {
            return 0;
        } else {
            int depthLeft = getDepthInternal(node->left);
            int depthRight = getDepthInternal(node->right);

            return 1 + std::max(depthLeft, depthRight);
        }
    }
};

void run(std::istream &in, std::ostream &out) {
    int n = 0;
    in >> n;
    int tmp = 0;
    int priority = 0;
    BinTree<int> binTree;
    DecartTree<> decartTree;

    for (int i = 0; i < n; i++) {
        in >> tmp >> priority;
        binTree.Insert(tmp);
        decartTree.Insert(tmp, priority);
    }
    //  out << binTree.getDepth() << " " << decartTree.getDepth() << std::endl;
    out << std::abs(binTree.getDepth() - decartTree.getDepth()) << std::endl;
}

void test() {
        std::stringstream input;
        input << "10" << std::endl;
        input << "38" << "19" << std::endl;
        input << "37" << "5"  << std::endl;
        input << "47" << "15" << std::endl;
        input << "35" << "0"  << std::endl;
        input << "12" << "3"  << std::endl;
        input << "0"  << "42" << std::endl;
        input << "31" << "37" << std::endl;
        input << "21" << "45" << std::endl;
        input << "30" << "26" << std::endl;
        input << "41" << "6"  << std::endl;

        std::stringstream output;
        run(input, output);
        std::cout << "Output:\n" << output.str();
        std::stringstream answer;
        answer << "2" << std::endl;
        std::cout << "Answer:\n" << answer.str();
        assert(output.str() == answer.str());
}

int main() {
    // test();
    run(std::cin, std::cout);
    return 0;
}
