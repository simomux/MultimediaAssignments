#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cassert>
#include <utility>
#include <vector>
#include <algorithm>

namespace mdp {

    template<typename T>
    struct vector {
        size_t size_, capacity_;
        T* data_;
        // Copy and swap idiom --> how to avoid writing two separate assignment operators
        vector() { // default constructor
            printf("default constructor\n");
            size_ = 0;
            capacity_ = 10;
            // data_ = (T *)malloc(capacity_ * sizeof(T));
            // If you want to allocate memory for non-pod types you also need to call the constructor (you need to use `new` instead of `malloc`)
            data_ = new T[capacity_];
        }
        vector(const vector &other) {    // copy constructor
            printf("copy constructor\n");
            size_ = other.size_;
            capacity_ = other.capacity_;
            //data_ = (T *)malloc(capacity_ * sizeof(T));
            data_ = new T[capacity_];
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
                //data_ = (T *)malloc(capacity_ * sizeof(T));
                data_ = new T[capacity_];
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
            // free(data_);
            // When working with non-pod types `free` doesn't work either, the destructor will never be called, you need to use `delete`
            delete[] data_;
        }
        void push_back(const T& num) {
            if (size_ == capacity_) {
                capacity_ *= 2;
                // data_ = (T*)realloc(data_, capacity_ * sizeof(T));

                // Realloc requires a more complex implementation
                T* tmp = new T[capacity_];
                for (size_t i = 0; i < size_; ++i) {
                    tmp[i] = data_[i];
                }
                delete[] data_;

                data_ = tmp;
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

}

void print(FILE *f, const std::vector<int> &v)
{
    /*for (size_t i = 0; i < v.size(); i++) {
        fprintf(f, "%d\n", v[i]);
    }*/

    // When iterators were trash

    /*std::vector<int>::const_iterator start = v.begin();
    std::vector<int>::const_iterator end = v.end();
    std::vector<int>::const_iterator it = start;

    while (it != end) {
        int x = *it;
        fprintf(f, "%d ", x);
        ++it;
    }*/

    // Post C++ 11 with the introduction of auto

    /*auto start = v.begin();
    auto end = v.end();
    auto it = start;

    while (it != end) {
        const auto& x = *it;
        fprintf(f, "%d ", x);
        ++it;
    }*/

    // Some cursed ahh for loop

    /*for (auto it = v.begin(), stop = v.end(); it != stop; ++it) {
        const auto& x = *it;
        fprintf(f, "%d ", x);
    }*/

    for (const auto& it : v) {
        fprintf(f, "%d\n", it);
    }
}

std::vector<int> read(FILE *f)
{
    using std::vector;
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

auto compare(int a, int b)
{
    return a < b;
}

// functor (or function object) for comparing with more complex criterias

struct comparator {
    // your state information

    auto operator()(int a, int b) const{
        return a > b;
    }
};

int main(int argc, char *argv[])
{
    using std::vector;
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

    // qsort(numbers.data(), numbers.size(), sizeof(int), compare);

    // std::sort(numbers.begin(), numbers.end());

    // Default implementation requires an additional function for predicate (2 args implementation uses less)
    // Sort uses quicksort for more than 32 elements or insertion sort if less

    // I don't need to specify the namespace because ADL (Argument Dependant Lookup) finds it automatically
    // sort(numbers.begin(), numbers.end(), comparator());

    // Sort with lambda function
    sort(numbers.begin(), numbers.end(),
        [](int a, int b){
            return a > b;
        }
    );
    print(output, numbers);

    fclose(output);
    return 0;
}