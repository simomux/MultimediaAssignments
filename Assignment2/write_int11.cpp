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

int main(int argc, char* argv[]) {
    using std::ranges::copy;

    if (argc != 3) {
        std::println(std::cerr, "Usage: {} filein.txt fileout.bin", argv[0]);
        return 1;
    }

    std::ifstream input(argv[1]);

    if (!input) {
        std::println(std::cerr, "Error opening input file");
        return 1;
    }

    std::ofstream output(argv[2], std::ios::binary);

    if (!output) {
        std::println(std::cerr, "Error opening output file");
        return 1;
    }

    // No check if the value is greater than 11 bits??
    std::vector<int> numbers{
    std::istream_iterator<int>(input),
    std::istream_iterator<int>()
    };

    std::queue<bool> buffer {};

    size_t size = 11;

    for (const auto& number : numbers) {
        // Castare il mio numero in binario
        bool bits[size];

        uint16_t twos_complement = static_cast<uint16_t>(number) & 0x7FF; // Assicura solo 11 bit
        for (int i = 0; i < 11; i++) {
            bits[10 - i] = (twos_complement >> i) & 1;  // Estrae i bit MSB → LSB
        }


        for (int i = 0; i < size; i++) {
            if (buffer.size() < 8) {
                // Push nel buffer di bit[i]
                buffer.push(bits[i]);
            } else {
                // Write del buffer su file
                // Pulisci il buffer
                char result = 0; // store the result
                size_t j = 0;
                while (!buffer.empty()) {
                    result += (buffer.front() << (7 - j)); // Add the bit shifted value
                    buffer.pop();
                    j++;
                }
                raw_write(output, result, 1);
            }
        }
    }
    return 0;
}