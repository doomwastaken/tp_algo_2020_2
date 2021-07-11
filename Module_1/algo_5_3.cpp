#include <sstream>
#include <iostream>
#include <exception>
#include <cstring>
#include <cassert>
#include <functional>

struct  Line {
    Line() : l(0), r(0) {}
    Line(int l, int r) : l(l), r(r) {}
    int l, r;
};

std::istream& operator>>(std::istream &in, Line &line) {
    in >> line.l >> line.r;
    return in;
}

std::ostream& operator<<(std::ostream &out, const Line &line) {
    out << "(" << line.l << ";" << line.r << ")";
    return out;
}

bool operator<(const Line &l, const Line &r) {
    if (l.l == r.l)
        return l.r < r.r;
    return l.l < r.r;
}

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
    Line GetLine(int idx) {
        return arr[idx];
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

template <typename T>
bool line_less(const T &l, const T &r) {
    if (l.l == r.l)
        return l.r < r.r;
    return l.l < r.l;
}

class LineComparator {
 public:
    bool operator()(const Line &l, const Line &r)     {
    if (l.l == r.l)
        return l.r < r.r;
    return l.l < r.l;
    }
};

template <typename T> struct FunctionType {
    typedef std::function<bool(T, T)> Type;
};

template <typename T>
class DefaultComparator {
 public:
    bool operator()(const T &l, const T &r) {
        return l < r;
    }
};

template <typename T, typename Comparator = DefaultComparator<T>>
void mergesort(Array<T> *arr, int l, int r, Comparator cmp = Comparator()) {
    for (int i = l; i < r; ++i) {
        for (int j = l; j < r; ++j) {
            if (cmp((*arr)[j + 1], (*arr)[j])) {
                std::swap((*arr)[j + 1], (*arr)[j]);
            }
        }
    }
}

template <typename T, typename Comparator = DefaultComparator<T>>
void mergesort2(Array<T> *arr, int l, int r, Comparator cmp = Comparator()) {
    if (l < r) {
        int len = r - l + 1;
        int m = len / 2;
        mergesort2(arr, l, l + m - 1, cmp);
        mergesort2(arr, l + m, r, cmp);
        Array<T> tmp = Array<T>();
        merge(arr, l, l + m - 1, l + m, r, &tmp, cmp);
        arrayCopy(arr, l, r, &tmp, 0, tmp.GetLen() - 1);
    }
}

template <typename T>
void arrayCopy(Array<T> *dest, int dBegin, int dEnd,
               Array<T> *src, int sBegin, int sEnd) {
    int i = dBegin;

    for (int j = sBegin; j <= sEnd; j++) {
        (*dest)[i++] = (*src)[j];
    }
}

template <typename T, typename Comparator = DefaultComparator<T>>
void merge(Array<T> *arr, int lBegin, int lEnd, int rBegin, int rEnd,
           Array<T> *dest, Comparator cmp = Comparator()) {
    int lP = lBegin;
    int rP = rBegin;
    while (lP <= lEnd && rP <= rEnd) {
        if (cmp((*arr)[lP], (*arr)[rP])) {
            dest->Add((*arr)[lP++]);
        } else {
            dest->Add((*arr)[rP++]);
        }
    }

    if (lP == lEnd + 1) {
        while (rP <= rEnd) {
            dest->Add((*arr)[rP++]);
        }
    } else {
        while (lP <= lEnd) {
            dest->Add((*arr)[lP++]);
        }
    }
}


template <typename T>
int LenColored(Array<T> *arr) {
    if (arr->GetLen() == 0) {
        return 0;
    }

    Line prev = (*arr)[0];
    int length = prev.r - prev.l;
    for (int i = 1; i < arr->GetLen(); i++) {
        Line curr = (*arr)[i];

        if (curr.l >= prev.l && curr.r <= prev.r) {
            continue;
        } else if (curr.l <= prev.r && curr.r > prev.r) {
            length += curr.r - prev.r;
            prev = curr;
        } else if (curr.l > prev.r) {
            length += curr.r - curr.l;
            prev = curr;
        }
    }

    return length;
}


void run(std::istream &in, std::ostream &out) {
    size_t n = 0;
    in >> n;

    if (n < 1 || n > 10000) {
        std::cerr << "entry error" << std::endl;
    }

    Array<Line> arr = Array<Line>();

    for (size_t i = 0; i < n; i++) {
        Line val;
        in >> val;
        arr.Add(val);
    }

    int answer = 0;
    mergesort2(&arr, 0, arr.GetLen() - 1, LineComparator());
    answer = LenColored(&arr);
    out << answer;
}

void test() {
    {
        std::stringstream input;
        input << "3" << std::endl;
        input << "1 4" << std::endl;
        input << "7 8" << std::endl;
        input << "2 5" << std::endl;

        std::stringstream output;

        run(input, output);
        assert(output.str() == "5");
    }
}

int main() {
    // test();
    run(std::cin, std::cout);
    return 0;
}
