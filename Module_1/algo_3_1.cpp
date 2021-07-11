#include <iostream>
#include <vector>
#include <string>

struct node {
    node(int val) : value(val), next(nullptr) {}
    int value;
    node *next;
};

class queue {
 public:
    queue() : head(nullptr), tail(nullptr), size(0) {}
    queue(const queue &other) = delete;
    queue(queue &&other) = delete;
    ~queue() {
        while (!isempty())
            dequeue();
    }

    queue& operator=(const queue &other) = delete;
    queue& operator=(queue &&other) = delete;

    void enqueue(int val) {
        node *newnode = new node(val);

        size++;
        if (isempty()) {
            head = tail = newnode;
            return;
        }

        tail->next = newnode;
        tail = newnode;
    }
    int dequeue() {
        if (isempty())
            return -1;

        int val = head->value;
        node *oldhead = head;

        size--;

        head = head->next;
        if (head == nullptr)
            tail = nullptr;

        delete oldhead;

        return val;
    }

    bool isempty() const {
        return head == nullptr && tail == nullptr;
    }

 private:
    node *head, *tail;
    int size;
};


int main(int argc, const char *argv[]) {
    queue myq;
    int n = 0;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        int operation = 0, value = 0;
        std::cin >> operation >> value;
        switch (operation) {
            case 2: {
                int element = myq.dequeue();
                if (element != value) {
                    std::cout << "NO";
                    return 0;
                }
                break;
            }
            case 3: {
                myq.enqueue(value);
                break;
            }
        }
    }
    std::cout << "YES";
}
