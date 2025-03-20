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

    std::array<char, 11> buffer{};
    while (raw_read(input, buffer, 1)) {

        copy(buffer, std::ostream_iterator<int>(output, "\n"));
    }

    return 0;
}