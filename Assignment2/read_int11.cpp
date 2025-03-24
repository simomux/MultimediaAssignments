/*
    Exercise 5
    Write a command line program in C ++ with this syntax:
    read_int11 <filein.bin> <fileout.txt>
    The first parameter is the name of a binary file that contains 11-bit numbers in 2’s complement, with the bits sorted from most significant to least significant. The program must create a new file, with the name passed as the second parameter, with the same numbers saved in decimal text format separated by a new line. Ignore any excess bits in the last byte.
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <ranges>

template <typename T>
std::istream& raw_read(std::istream& input, T& val, const size_t size = sizeof(T))
{
    return input.read(reinterpret_cast<char*>(&val), size);
}

class bitreader {
    std::istream& is_;
    uint8_t buffer_ = 0;
    size_t n_ = 0;

    uint32_t readbit() {
        if (n_ == 0) {
            raw_read(is_, buffer_);
            n_ = 8;
        }
        --n_;

        return (buffer_ >> n_) & 1;
    }

public:
    bitreader(std::istream& is) : is_(is) {}

    // Read n bits into u from MSB to LSB
    std::istream& operator()(uint32_t &u, size_t n) {
        u = 0;
        while (n --> 0) {
            u = (u << 1) | readbit();
        }
        return is_;
    }
};

int main(int argc, char* argv[]) {
    using std::ranges::copy;

    if (argc != 3) {
        std::println(std::cerr, "Usage: {} input_file output_file", argv[0]);
        return EXIT_FAILURE;
    }

    std::ifstream input(argv[1], std::ios::binary);

    if (!input) {
        std::println(std::cerr, "Error opening input file");
        return EXIT_FAILURE;
    }

    std::ofstream output(argv[2]);

    if (!output) {
        std::println(std::cerr, "Error opening output file");
        return EXIT_FAILURE;
    }


    bitreader br(input);

    std::vector<int32_t> numbers{};
    uint32_t value;
    while (br(value, 11)) {
        int32_t realnumber = value;
        if (value > 1023) {
            realnumber = value - 2048;
        }
        numbers.push_back(realnumber);
    }

    copy(numbers, std::ostream_iterator<int>(output, "\n"));

    return EXIT_SUCCESS;
}