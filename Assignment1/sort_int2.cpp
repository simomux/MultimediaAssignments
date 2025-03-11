#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cassert>
#include <utility>

// Comparison function for qsort
int compare(const void *a, const void *b) 
{
    if (*(int *)a < *(int *)b) {
        return -1;
    }
    if (*(int *)a > *(int *)b) {
        return 1;
    }
    return 0;
}

template<typename T>
struct vector {
    size_t size_, capacity_;
    T* data_;
    // Copy and swap idiom --> how to avoid writing two separate assignment operators
    vector() { // default constructor
        printf("default constructor\n");
        size_ = 0;
        capacity_ = 10;
        data_ = (T *)malloc(capacity_ * sizeof(T));
    }
    vector(const vector &other) {    // copy constructor
        printf("copy constructor\n");
        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = (T *)malloc(capacity_ * sizeof(T));
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }
    vector(vector &&other) {    // move constructor // rvalue reference (making it const makes no sense)
        printf("move constructor\n");
        size_ = other.size_;
        capacity_ = other.capacity_;
        data_ = other.data_;
        other.data_ = nullptr;
    }

    /* The result of an assignment is the address of the object that you have assigned
    vector &operator=(const vector &other) {
        if (this == &other) {
            return *this;
        }
        free(data_);
        size_ = other.size_;
       capacity_ = other.capacity_;
        data_ = (int *)malloc(capacity_ * sizeof(int));
       for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
        return *this;
    }*/

    vector& operator=(const vector &other) {    // copy assignment
        printf("copy assignment\n");
        if (capacity_ < other.size_) {
            free(data_);
            capacity_ = other.capacity_;
            data_ = (T *)malloc(capacity_ * sizeof(T));
        }
        size_ = other.size_;
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
        return *this;
    }
    vector& operator=(vector &&other) {    // move assignment
        printf("move assignment\n");
        size_ = other.size_;
        capacity_ = other.capacity_;
        std::swap(data_, other.data_);
        return *this;
    }
    ~vector() {    // destructor
        printf("destructor\n");
        free(data_);
    }
    void push_back(const T& num) {
        if (size_ == capacity_) {
            capacity_ *= 2;
            data_ = (T*)realloc(data_, capacity_ * sizeof(T));
        }
        data_[size_] = num;
        size_++;
    }
    size_t size() const {
        return size_;
    }
    const T& at(size_t index) const {
        assert(index < size_);
        return data_[index];
    }
    // T& operator[](const vector<T> *this, size_t index) {
    const T& operator[](size_t index) const {
        return data_[index];
    }
    // I can do this overloading since the implicit parameter is different
    // When compiling code the compiler does name mangling of the functions basically adding an hashing with the parameters type after the function name so that the linker has 2 functions with different names if I'm overloading.
    // T& operator[](vector<T> *this, size_t index) {
    T& operator[](size_t index) {   // Overloaded version of [] for managing assignments
        return data_[index];
    }
};

void print(FILE *f, const vector<int> &v)
{
    for (size_t i = 0; i < v.size(); i++) {
        fprintf(f, "%d\n", v[i]);
    }
}

vector<int> read(FILE *f)
{
    if (f == NULL) {
        return vector<int>();
    }

    vector<int> v;
    int num;
    while (fscanf(f, "%d", &num) == 1) {
        v.push_back(num);
    }
    return v;
}

int main(int argc, char *argv[])
{
    {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
            return 1;
        }

        FILE *input = fopen(argv[1], "r");
        if (!input) {
            perror("Error opening input file");
            return 1;
        }

        FILE *output = fopen(argv[2], "w");
        if (!output) {
            perror("Error opening output file");
            fclose(input);
            return 1;
        }

        vector<int> numbers;
        numbers = read(input);
        fclose(input);

        qsort(numbers.data_, numbers.size(), sizeof(int), compare);

        print(output, numbers);

        fclose(output);
    }
    return 0;
}