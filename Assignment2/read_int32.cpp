/*
    Exercise 3
    Write a command line program in C++ with this syntax:
    read_int32 <filein.bin> <fileout.txt>
    The first parameter is the name of a binary file containing 32-bit numbers 2’s complement, in little endian. The program must create a new file, with the name passed as the second parameter, with the same numbers saved in decimal text format separated by a new line.
* */

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

int main(int argc, char* argv[]) {
    using std::ranges::copy;

    if (argc != 3) {
        std::println(std::cerr, "Usage: {} input_file output_file", argv[0]);
        return 1;
    }

    std::ifstream input(argv[1], std::ios::binary);

    if (!input) {
        std::println(std::cerr, "Error opening input file");
        return 1;
    }

    std::ofstream output(argv[2]);

    if (!output) {
        std::println(std::cerr, "Error opening output file");
        return 1;
    }

    std::vector<int32_t> numbers{};

    int32_t value;
    while (raw_read(input, value)) {
        numbers.push_back(value);
    }

    copy(numbers, std::ostream_iterator<int>(output, "\n"));

    return 0;
}