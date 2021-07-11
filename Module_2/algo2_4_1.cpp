#include <iostream>
#include <sstream>
#include <cassert>

template <typename T>
struct Node {
    explicit Node(const T &key)
        : key(key), left(nullptr), right(nullptr), height(1), position(1) {
    }
    T key;
    Node *left;
    Node *right;
    size_t height;
    int position;
};

template <typename T, class Comparator = std::greater<T>>
class AvlTree {
 public:
    AvlTree() : root(nullptr), cmp(Comparator()) { }
    ~AvlTree() {
        destroyTree(root);
    }
    void Add(const T &key, int &position) {
        root = addInternal(root, key, position);
    }
    bool Has(const T &key) {
        Node<T> *tmp = root;
        while (tmp) {
            if (tmp->key == key) {
                return true;
            } else if (tmp->key < key) {
                tmp = tmp->right;
            } else {
                tmp = tmp->left;
            }
        }
        return false;
    }
    void Delete(const int& position) {
        T key = FindNodes(root, position);
        root = deleteInternal(root, key);
    }

    // unsigned int findKey(Node<T> *node, T key, Compare Less = std::less()) {
    //     int ans = 0;
    //     Node<T>* p = node;
    //     while (p != nullptr && IsLess(key, p->value) != 0) {
    //         if (IsLess(key, p->value) == 1) {
    //             ans += (p->right?p->right->ElementsCnt + 2:1);
    //             p = p->left;
    //         } else {
    //             p = p->right;
    //         }
    //     }
    //     ans += (p->right?p->right->ElementsCnt + 1:0);
    //     return ans;
    // }

 private:
    Node<T> *root;
    Comparator cmp;
    int getPosition(Node<T> *node) {
        return node ? node->position : 0;
    }

    void fixPosition(Node<T> *node) {
        node->position = getPosition(node->left) + getPosition(node->right) + 1;
    }

    T FindNodes(Node<T> *node, const int& position) {
        if (position < getPosition(node->right)) {
            (node->position)--;
            return FindNodes(node->right, position);
        }
        if (position > getPosition(node->right)) {
            (node->position)--;
            return FindNodes(node->left, position - getPosition(node->right) - 1);
        }

        return node->key;
    }

    void destroyTree(Node<T> *node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }
    Node<T>* deleteInternal(Node<T> *node, const T &key) {
        if (!node)
            return nullptr;
        if (node->key < key) {
            node->right = deleteInternal(node->right, key);
        } else if (node->key > key) {
            node->left = deleteInternal(node->left, key);
        } else {
            Node<T> *left = node->left;
            Node<T> *right = node->right;
            delete node;

            // сравнить высоты поддеревьев left и right
            // подставляем либо min из right, либо max из left
            // из более глубокого дерева

            // findMin и removeMin заменить на findAndRemoveMin
            if (!left && !right) {
                return nullptr;
            }

            if (getHeight(left) > getHeight(right)) {
                auto *max = new Node<T>(key);
                //  Node *max = findMax(left);
                max = findMax(left);
                max->left = removeMax(left);
                max->right = right;

                return doBalance(max);
            } else {
                auto *min = new Node<T>(key);
                min = findMin(right);
                //  Node *min = findMin(right);
                min->right = removeMin(right);
                min->left = left;

                return doBalance(min);
            }
        }

        return doBalance(node);
    }

    Node<T>* addInternal(Node<T> *node, const T &key, int& position) {
        if (!node)
            return new Node<T>(key);
        (node->position)++;
        if (cmp(node->key, key)) {
            position += getPosition(node->right) + 1;
            node->left = addInternal(node->left, key, position);
        } else {
            node->right = addInternal(node->right, key, position);
        }

        return doBalance(node);
    }

    Node<T>* findMin(Node<T> *node) {
        while (node->left)
            node = node->left;
        return node;
    }

    Node<T>* removeMin(Node<T> *node) {
        if (!node->left)
            return node->right;
        node->left = removeMin(node->left);
        (node->position)--;
        return doBalance(node);
    }

    Node<T>* findMax(Node<T>* node) {
        while (node->right) {
            node = node->right;
        }
        return node;
    }

    Node<T>* removeMax(Node<T>* node) {
        if (!node->right) {
            return node->left;
        }
        node->right = removeMax(node->right);
        (node->position)--;
        return doBalance(node);
    }

    Node<T> FindAndRemoveMax(Node<T>* node) {
        while (node->right) {
            node = node->right;
        }
        if (!node->right) {
            return node->left;
        }
        node->right = removeMax(node->right);
        return doBalance(node)->left;
    }
    Node<T> FindAndRemoveMin(Node<T>* node) {
        while (node->left)
            node = node->left;
        if (!node->left)
            return node->right;
        node->left = removeMin(node->left);
        return doBalance(node)->left;
    }
    size_t getHeight(Node<T> *node) {
        return node ? node->height : 0;
    }

    void fixHeight(Node<T> *node) {
        node->height = std::max(getHeight(node->left), getHeight(node->right)) + 1;
    }

    int getBalance(Node<T> *node) {
        return getHeight(node->right) - getHeight(node->left);
    }

    Node<T>* doBalance(Node<T> *node) {
        fixHeight(node);
        fixPosition(node);

        switch (getBalance(node)) {
            case 2:
            {
                if (getBalance(node->right) < 0)
                    node->right = rotateRight(node->right);
                return rotateLeft(node);
            }
            case -2:
            {
                if (getBalance(node->left) > 0)
                    node->left = rotateLeft(node->left);
                return rotateRight(node);
            }
            default:
                return node;
        }
    }

    Node<T>* rotateLeft(Node<T> *node) {
        Node<T> *tmp = node->right;
        node->right = tmp->left;
        tmp->left = node;
        fixPosition(node);
        fixPosition(tmp);
        fixHeight(node);
        fixHeight(tmp);
        return tmp;
    }

    Node<T>* rotateRight(Node<T> *node) {
        Node<T> *tmp = node->left;
        node->left = tmp->right;
        tmp->right = node;
        fixPosition(node);
        fixPosition(tmp);
        fixHeight(node);
        fixHeight(tmp);
        return tmp;
    }
};

void run(std::istream &in, std::ostream &out) {
    AvlTree<int> avlTree;

    char op;
    int key = 0;
    int n = 0;

    in >> n;
    for (int i = 0; i < n; i++) {
        in >> op >> key;
        switch (op) {
            case '1':
            {
                int position = 0;
                avlTree.Add(key, position);
                out << position << std::endl;
                break;
            }
            case '2':
            {
                avlTree.Delete(key);
                break;
            }
            case '?':
            {
                out << (avlTree.Has(key) ? "OK": "FAIL") << std::endl;
                break;
            }
        }
    }
}

int main(int argc, const char * argv[]) {
    //  test();
    run(std::cin, std::cout);
    return 0;
}
