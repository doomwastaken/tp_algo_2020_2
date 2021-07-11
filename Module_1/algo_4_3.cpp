#include <sstream>
#include <functional>
#include <iostream>
#include <cassert>
#include <exception>
/*
На вокзале есть некоторое количество тупиков, куда прибывают электрички.
Этот вокзал является их конечной станцией. Дано расписание движения электричек,
в котором для каждой электрички указано время ее прибытия, а также время отправления
в следующий рейс. Электрички в расписании упорядочены по времени прибытия.
Когда электричка прибывает, ее ставят в свободный тупик с минимальным номером. 
При этом если электричка из какого-то тупика отправилась в момент времени X,
то электричку, которая прибывает в момент времени X, в этот тупик ставить нельзя,
а электричку, прибывающую в момент X+1 — можно. В данный момент на вокзале
достаточное количество тупиков для работы по расписанию. 
Напишите программу, которая по данному расписанию определяет, 
какое минимальное количество тупиков требуется для работы вокзала.

Формат ввода
Вначале вводится n - количество электричек в расписании. 1 ≤ n ≤ 100000. 
Затем вводятся n строк для каждой электрички, в строке - время прибытия
и время отправления. Время - натуральное число от 0 до 1 000 000 000.
Строки в расписании упорядочены по времени прибытия.

Формат вывода
Натуральное число - минимальное количеством тупиков.
*/

template <typename T>
class Array {
 public:
    Array() {
        len = 32;
        curlen = 0;
        // try {
        arr = new T[len];  // std::nothrow
        // } catch (const std::bad_alloc& exc) {
        //    std::cout << exc.what() << std::endl;
        //}
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


template<typename T>
bool my_less(const T& left, const T& right) {
    return left < right;
}

template<typename T> struct FunctionType {
    typedef std::function<bool(T, T)> Type;
};


class MyException: public std::exception {
 public:
    explicit MyException(std::string error) : mError(error) {}
    std::string GetError() {
        return mError;
    }

 private:
    std::string mError;
};

template <typename T>
class Heap {
 public:
    explicit Heap(std::function<bool(T, T)> Comparator) {
        CompF = Comparator;
    }
    ~Heap() {}
    void Insert(int el) {
        arr.Add(el);
        SiftUp(arr.GetLen() - 1);
    }
    int PullRoot() {
        if (arr.GetLen() == 0) {
            throw MyException("Empty");
        }

        int result = arr[0];

        arr[0] = arr[arr.GetLen() - 1];
        arr.DeleteLast();

        if (arr.GetLen() != 0) {
            SiftDown(0);
        }
        return result;
    }

    int PeekRoot() {
        if (arr.GetLen() == 0) {
            throw MyException("Empty");
        }
        return arr[0];
    }

    int GetLen() {
        return arr.GetLen();
    }

 private:
    std::function<bool(T, T)> CompF = std::less<int>();
    Array<T> arr;
    void SiftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (CompF(arr[parent], arr[i]) || arr[parent] == arr[i]) {
                return;
            }
            std::swap(arr[i], arr[parent]);
            i = parent;
        }
    }

    void SiftDown(int i) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        int largest = i;
        if (left < arr.GetLen() && CompF(arr[left], arr[i]))
            largest = left;
        if (right < arr.GetLen() && CompF(arr[right], arr[largest]))
            largest = right;

        if (largest != i) {
            std::swap(arr[i], arr[largest]);
            SiftDown(largest);
        }
    }
};

void run(std::istream &in, std::ostream &out) {
    int n = 0;
    in >> n;

    if (n < 1 || n > 10000) {
        std::cerr << "entry error" << std::endl;
    }
    FunctionType<int>::Type Funcp = my_less<int>;
    Heap<int> heap(Funcp);   // = Heap<int>(Funcp);

    for (int i = 0; i < n; i++) {
        int arrival, departure;
        in >> arrival >> departure;
        if (arrival < 0   || arrival   > 10000000 ||
            departure < 0 || departure > 10000000 ) {
            std::cerr << "entry error" << std::endl;
        }
        try {
            int heap_size = heap.GetLen();
            if (heap_size > 0) {
                int root = heap.PeekRoot();
                if (arrival > root) {
                    heap.PullRoot();
                    heap.Insert(departure);
                } else {
                    heap.Insert(departure);
                }
            } else {
                heap.Insert(departure);
            }
        } catch (MyException &exception) {
            std::cerr << exception.what() << std::endl;
        }
    }

    int deadends = heap.GetLen();
    out << deadends;
}

void test() {
    {
        std::stringstream input;
        input << "1" << std::endl;
        input << "10 20" << std::endl;

        std::stringstream output;
        run(input, output);

        assert(output.str() == "1");
    }
    {
        std::stringstream input;
        input << "2" << std::endl;
        input << "10 20" << std::endl;
        input << "20 25" << std::endl;

        std::stringstream output;

        run(input, output);
        assert(output.str() == "2");
    }
    {
        std::stringstream input;
        input << "3" << std::endl;
        input << "10 20" << std::endl;
        input << "20 25" << std::endl;
        input << "21 30" << std::endl;

        std::stringstream output;

        run(input, output);
        assert(output.str() == "2");
    }
}

int main() {
    test();
    run(std::cin, std::cout);
    return 0;
}
