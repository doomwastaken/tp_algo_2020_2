#include <iostream>

int binarysearch(int *buf, int a, int size) {
    int left = 0, right = size - 1, middle = 0;
    while (left <= right) {
        middle = (left + right) / 2;
        if (a < buf[middle]) {
            right = middle - 1;
        } else {
            if (a > buf[middle]) {
                left = middle + 1;
            } else {
            while (buf[left] < a) {
                left++;
            }
            return left;
            }
        }
    }
    if (buf[right] < a)
        return left;
    return middle;  // Если такого элемента нет, выведите n.
}

int main(int argc, const char *argv[]) {
    int size1 = 0, size2 = 0;
    int *buf1 = nullptr;
    int *buf2 = nullptr;

    std::cin >> size1;
    if (size1 < 1 || size1 > 10000)
        return -1;

    buf1 = (int*)malloc(size1 * sizeof(int));

    if (buf1 == nullptr)
        return -1;

    std::cin >> size2;
    if (size2 < 1 || size2 > 10000)
        return -1;

    buf2 = (int*)malloc(size2 * sizeof(int));

    if (buf2 == nullptr)
        return -1;


    for (int i = 0; i < size1; i++) {
        std::cin >> buf1[i];
    }
    for (int i = 0; i < size2; i++) {
        std::cin >> buf2[i];
    }
    int k = 1;
    for (int i = 0; i < size2; i++) {
        while ((k < size1) && (buf2[i] > buf1[k])) {
            k = k*2;
        }
        //std::cout << "k=" << k << std::endl;
        if (k == 1) {
            if (buf2[i] > buf1[size1-1]) {
                std::cout << size1 << std::endl;
            } else {
                std::cout << binarysearch(buf1, buf2[i], size1) << std::endl;
            }
        } else {
            if (buf2[i] > buf1[size1-1]) {
                std::cout << size1 << std::endl;
            } else if (k < size1) {
                std::cout << binarysearch(buf1+k/2, buf2[i], size1 - k/2)+k/2 << std::endl;
            } else {
                std::cout << binarysearch(buf1+k/2, buf2[i], size1 - k/2)+k/2 << std::endl;
            }
        }

        k = 1;
    }

    free(buf1);
    free(buf2);
    return 0;
}
