#define DEFAULT_SIZE 8
const double ALPHA = 0.75;

#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <cstddef>

enum State {
    EMPTY,
    DELETED,
    FILLED
};

template <typename T>
struct Node {
    T key;
    State state;
    explicit Node(const T &val): key(val), state(FILLED) {}
    Node(): key(), state(EMPTY) {}
};

size_t HashOne(const std::string &s) {
    size_t hash = 0;
    for (size_t i = 0; i < s.size(); i++) {
        hash = (hash * 13 + s[i]);
    }

    return hash;
}

size_t HashTwo(const std::string &s) {
    size_t hash = 0;
    for (size_t i = 0; i < s.size(); i++) {
        hash = (hash * 37 + s[i]);
    }

    return hash;
}

template <typename T>
class HashTable {
 public:
    explicit HashTable(size_t tableSize): table(tableSize, Node<T>()), size(0) {}
    ~HashTable() {}
    bool Add(const T &key) {
        if (size >= table.size() * ALPHA) {
            Expand();
        }
        size++;
        size_t hashMain = HashOne(key);
        size_t hashEx = HashTwo(key);
        size_t hash = hashMain % table.size();
        size_t lastHash = hash % table.size();
        T tmp = key;
        bool found = false;
        unsigned int probeN = 1;

        while (table.size() > probeN) {
            if (table[hash].state == EMPTY) {
                table[hash] = Node<T>(tmp);
                return true;
            } else if (table[hash].state == DELETED) {
                if (found) {
                    lastHash = hash;
                }
                hash = (hashMain + probeN * (2 * hashEx + 1)) % table.size();
                probeN++;
            } else if (table[hash].state == FILLED && table[hash].key == key) {
                return false;
            } else {
                found = true;
                hash = (hashMain + probeN * (2 * hashEx + 1)) % table.size();
                probeN++;
            }
        }

        table[lastHash] = Node<T>(tmp);
        return true;
    }


    bool Delete(const T &key) {
        size_t hashMain = HashOne(key);
        size_t hashEx = HashTwo(key);
        size_t hash = hashMain % table.size();

        unsigned int probeN = 1;

        while (table.size() > probeN) {
            if (table[hash].state == FILLED && table[hash].key == key) {
                table[hash] = Node<T>();
                table[hash].state = DELETED;
                size--;
                return true;
            }
            if (table[hash].state == EMPTY) {
                return false;
            } else {
                hash = (hashMain + probeN * (2 * hashEx + 1)) % table.size();
                probeN++;
            }
        }

        return false;
    }
    bool Has(const T &key) {
        size_t hashMain = HashOne(key);
        size_t hashEx = HashTwo(key);
        size_t hash = hashMain % table.size();

        unsigned int probeN = 1;
        while (table.size() > probeN) {
            if (table[hash].state == FILLED && table[hash].key == key) {
                return true;
            }
            if (table[hash].state == EMPTY) {
                return false;
            } else {
                hash = (hashMain + probeN * (2 * hashEx + 1)) % table.size();
                probeN++;
            }
        }
        return false;
    }

 private:
    std::vector<Node<T>> table;
    size_t size;
    void Expand() {
        auto oldTable = std::move(table);
        table = std::vector<Node<T>>(oldTable.size() * 2, Node<T>());

        for (size_t i = 0; i < oldTable.size(); i++) {
            if (oldTable[i].state == FILLED) {
                T key = oldTable[i].key;
                this->Add(key);
            }
        }
    }
};


void run(std::istream &in, std::ostream &out) {
    std::cout << "test" <<std::endl;
    std::cerr << "done" << std::endl;
    char op;
    std::string key;
    HashTable<std::string> table(8);

    while (in >> op >> key) {
        switch (op) {
            case '?':
            {
                out << (table.Has(key) ? "OK" : "FAIL") << std::endl;
                break;
            }
            case '+':
            {
                out << (table.Add(key) ? "OK" : "FAIL") << std::endl;
                break;
            }
            case '-':
            {
                out << (table.Delete(key) ? "OK" : "FAIL") << std::endl;
                break;
            }
        }
    }
}

void test() {
        std::stringstream input;
        input << "+ hello" << std::endl;
        input << "+ bye" << std::endl;
        input << "? bye" << std::endl;
        input << "+ bye" << std::endl;
        input << "- bye" << std::endl;
        input << "? bye" << std::endl;
        input << "? hello" << std::endl;
        std::stringstream output;
        run(input, output);

        std::stringstream answer;
        answer << "OK" << std::endl;
        answer << "OK" << std::endl;
        answer << "OK" << std::endl;
        answer << "FAIL" << std::endl;
        answer << "OK" << std::endl;
        answer << "FAIL" << std::endl;
        answer << "OK" << std::endl;

        assert(output.str() == answer.str());
}

int main() {
    //  test();
    run(std::cin, std::cout);
    return 0;
}
