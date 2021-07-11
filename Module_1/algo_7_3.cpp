#include <sstream>
#include <iostream>
#include <cstring>
#include <cassert>

template <typename T>
class Array {
 public:
    Array() {
        len = 32;
        curlen = 0;
        arr = new T[len];  // std::nothrow
    }

    ~Array() {
        delete [] arr;
    }

    T& operator[] (const int index) {
        return arr[index];
    }

    void Add(T elem) {
        if (curlen == len) {
            ExpandArray(len * 2);
        }

        arr[curlen++] = elem;
    }

    int GetLen() {
        return curlen;
    }

    void DeleteLast() {
        curlen--;
    }

    void Delete(unsigned int pos) {
        if (curlen == 1) {
            curlen--;
        } else {
            for (unsigned int i=pos; i < curlen - 1; i++) {
                arr[i] = arr[i+1];
                curlen--;
            }
        }
    }

    T& Getlast() {
        return arr[curlen - 1];
    }

    bool IsEmpty() const {
        return curlen == -1;
    }

    void Swap(int l, int r) {
        T tmp = arr[l];
        arr[l] = arr[r];
        arr[r] = tmp;
    }

 private:
    T *arr;
    int len;
    int curlen;

    void ExpandArray(int size) {
        T *newarr = new T[size];

        for (int i = 0; i < curlen; i++) {
            newarr[i] = arr[i];
        }

        delete [] arr;
        arr = newarr;
        len = size;
    }
};

template <typename T>
bool getBit(const T val, unsigned int bitNum) {
    return (((val >> bitNum) & 1) == 1);
}

template <typename T>
void BitPartition(Array<T> *arr, int l, int r, unsigned int bitNumber) {
    if (l < r) {
        int i = l;
        int j = r;

        while (i != j) {
            while (!getBit((*arr)[i], bitNumber) && i < j)
                i++;
            while (getBit((*arr)[j], bitNumber) && j > i)
                j--;
            if (i != j) {
                arr->Swap(i, j);
            }
        }
        if (!getBit((*arr)[r], bitNumber) || getBit((*arr)[l], bitNumber)) {
            BitPartition(arr, l, r, bitNumber - 1);
            return;
        }
        BitPartition(arr, l, j - 1, bitNumber - 1);
        BitPartition(arr, j, r, bitNumber - 1);
    }
}

template <typename T>
void binQuickSort(Array<T> *arr, int l, int r) {
    if (l < r) {
        unsigned int highest_bit_n = sizeof(T) * 8 - 1;
        BitPartition(arr, l, r, highest_bit_n);
    }
}


template <typename T>
int BitPartition2(Array<T> *arr, int l, int r, unsigned int bitNumber) {
    long long pivot = (*arr)[r];
    int i = (l - 1);
    for (int j = l; j <= r - 1; j++) {
        if ((*arr)[j] < pivot) {
            i++;
            arr->Swap(i, j);
        }
    }
    arr->Swap(i+1, r);
    return (i + 1);
}

template <typename T>
void binQuickSort2(Array<T> *arr, int l, int r) {
    if (l < r) {
        unsigned int highest_bit_n = sizeof(T) * 8 - 1;
        int pi = BitPartition2(arr, l, r, highest_bit_n);
        binQuickSort2(arr, l, pi - 1);
        binQuickSort2(arr, pi + 1, r);
    }
}

void run(std::istream &in, std::ostream &out) {
    size_t n = 0;
    in >> n;
    assert(n < 1000000);

    Array<long long> arr = Array<long long>();

    for (size_t i = 0; i < n; i++) {
        long long val = 0;
        in >> val;

        arr.Add(val);
    }

    binQuickSort(&arr, 0, arr.GetLen() - 1);
    for (int i = 0; i < arr.GetLen(); i++)
        out << arr[i] << ' ';
}

void test() {
    {
        std::stringstream input;
        input << "3" << std::endl;
        input << "4 1000000 7 " << std::endl;

        std::stringstream output;

        run(input, output);
        assert(output.str() == "4 7 1000000 ");
    }
}

int main() {
    // test();
    run(std::cin, std::cout);
    return 0;
}
