#include <sstream>
#include <iostream>
#include <exception>
#include <cstring>
#include <cassert>

template <typename T, typename Comparator = std::less<T>>
int partition(T *arr, int l, int r, Comparator cmp = Comparator()) {
    T pivot = rand() % (r - l + 1) + l;
    std::swap(arr[pivot], arr[r]);
    // int i = l + 1;
    // int j = r;

    for (int i = l; i < r; i++) {
        if (cmp(arr[i], arr[r]) || arr[i] == arr[r]) {
            std::swap(arr[l], arr[i]);
            l++;
        }
    }
    std::swap(arr[l], arr[r]);
    return l;
}

template <typename T, typename  Comparator = std::less<T>>
T kth_statistic(T *arr, int k, int l, int r, Comparator cmp = Comparator()) {
    int pivot_pos = partition(arr, l, r, cmp);
    int left = l;
    int right = r;
    while (1) {
        pivot_pos = partition(arr, left, right, cmp);

        if (pivot_pos == k) {
            return arr[pivot_pos];
        } else if (pivot_pos > k) {
            right = pivot_pos;
        } else {
            left = pivot_pos + 1;
        }
    }
    return pivot_pos;
}

void run(std::istream &in, std::ostream &out) {
    int n = 0;
    int k = 0;
    in >> n >> k;

    int *array = new int[n];

    for (int i = 0; i < n; i++) {
        in >> array[i];
    }
    std::cout << kth_statistic(array, k, 0, n-1) <<std::endl;

    delete[] array;
}

void test() {
    {
        std::stringstream input;
        input << "10 4" << std::endl;
        input << "1 2 3 4 5 6 7 8 9 10" << std::endl;

        std::stringstream output;

        run(input, output);
        assert(output.str() == "5");
    }
    {
        std::stringstream input;
        input << "10 0" << std::endl;
        input << "3 6 5 7 2 9 8 10 4 1" << std::endl;

        std::stringstream output;

        run(input, output);
        assert(output.str() == "1");
    }
    {
        std::stringstream input;
        input << "10 9" << std::endl;
        input << "0 0 0 0 0 0 0 0 0 1" << std::endl;

        std::stringstream output;

        run(input, output);
        assert(output.str() == "1");
    }
}

int main() {
    test();
    // run(std::cin, std::cout);
    int n = 0;
    int k = 0;
    std::cin >> n >> k;

    int *array = new int[n];

    for (int i = 0; i < n; i++) {
        std::cin >> array[i];
    }
    std::cout << kth_statistic(array, k, 0, n-1) <<std::endl;

    delete[] array;

    return 0;
}
