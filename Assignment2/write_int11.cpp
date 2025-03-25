/*
    Exercise 4
    Write a command line program in C++ with this syntax:
    write_int11 <filein.txt> <fileout.bin>
    The first parameter is the name of a text file that contains base 10 integers from -1000 to 1000 separated by whitespace. The program must create a new file, with the name passed as the second parameter, with the same numbers saved as 11-bit binary in 2’s complement. The bits are inserted in the file from the most significant to the least significant. The last byte of the file, if incomplete, is filled with bits equal to 0. Since the incomplete byte will have at most 7 padding bits, there’s no risk of interpreting padding as another value.
* */

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <ranges>

template<typename T>
std::ostream& raw_write(std::ostream& output, const T& val, const size_t size = sizeof(val))
{
    return output.write(reinterpret_cast<const char*>(&val), size);
};

// SOLUTION: fill bit-by-bit an 8 bit buffer and write to file when all the char in the byte have been inserted

class bitwriter {
    std::ostream& os_;
    uint8_t buffer_ = 0;
    size_t n_ = 0;

    void writebit(uint32_t bit) {
        buffer_ = (buffer_ << 1) | (bit & 1);
        ++n_;
        if (n_ == 8) {
            raw_write(os_, buffer_);
            n_ = 0;
        }
    }

public:
    // Things are initialized with the same order of how they are declared in the struct, so we maintain the same order in the constructor
    bitwriter(std::ostream& os) : os_(os) {}
    // Initializing the ostream in the constructor list

    ~bitwriter() {
        flush();
    }

    // Write the n least significant bits of u from MSB to LSB
    std::ostream& operator()(uint32_t u, size_t n) {
        while (n --> 0) {
            writebit(u >> n);
        }
        return os_;
    }

    std::ostream& flush(uint32_t bit = 0) {
        while (n_ > 0) {
            writebit(bit);
        }
        return os_;
    }
};

int main(int argc, char* argv[]) {
    using std::ranges::copy;

    if (argc != 3) {
        std::println(std::cerr, "Usage: {} filein.txt fileout.bin", argv[0]);
        return EXIT_FAILURE;
    }

    std::ifstream input(argv[1]);

    if (!input) {
        std::println(std::cerr, "Error opening input file");
        return EXIT_FAILURE;
    }

    std::ofstream output(argv[2], std::ios::binary);

    if (!output) {
        std::println(std::cerr, "Error opening output file");
        return EXIT_FAILURE;
    }

    // No check if the value is greater than 11 bits??
    std::vector<int> numbers{
    std::istream_iterator<int>(input),
    std::istream_iterator<int>()
    };

    // We don't risk on writing on a closed file with flush in the destructor because output is declared before bw, so it will be destroyed later (C++ destroys variable on reverse order from when they're declared)
    bitwriter bw(output);

    for (const auto& number : numbers) {
        bw(number, 11);
    }


    return EXIT_SUCCESS;
}