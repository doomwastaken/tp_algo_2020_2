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
    queue() : head(nullptr), tail(nullptr), size(0)
    {

    }
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
            head = tail = current = newnode;
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

    int cycle() {
        if (isempty())
            return -1;
        previous = current;
        current = current->next;
        if (current == nullptr)
            current = head;

        return 0;
    }

    int kill() {
        if (isempty())
            return -1;

        int val = current->value;
        node *old = current;
        size--;
        previous->next = current->next;
        current = current->next;
        if (current == nullptr) {
            tail = previous;
            current = head;
        }
        if (size == 0)
            head = tail = nullptr;
        delete old;

        return val;
    }

    int getsize() {
        return size;
    }

    bool isempty() const {
        return head == nullptr && tail == nullptr;
    }

 private:
    node *head, *tail, *previous, *current;
    int size;
};


int main(int argc, const char *argv[]) {
    queue myq;
    int n = 0, kill = 0, last = 1, value = 0;
    std::cin >> n;
    std::cin >> kill;

    if (n < 0 || n >10000)
        return -1;

    for (int i = 1; i <= n; ++i) {
        myq.enqueue(i);
    }


    while (!myq.isempty()) {
        if (last == kill) {
            value = myq.kill();
            last = 1;
        } else {
            value = myq.cycle();
            last++;
        }
    }
    std::cout << value << std::endl;
    return 0;
}
