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
        // Castare il numero in binario a 11 bit
        uint16_t twos_complement;
        if (number < 0) {
            twos_complement = (1 << 11) + number; // Complemento a due per i numeri negativi
        } else {
            twos_complement = static_cast<uint16_t>(number); // Per numeri positivi
        }

        // Aggiungere i bit nel buffer (dal MSB al LSB)
        for (int i = 10; i >= 0; --i) {
            buffer.push((twos_complement >> i) & 1);
        }

        // Se la coda ha 8 bit scrivila su file
        while (buffer.size() >= 8) {
            char result = 0;
            for (int i = 0; i < 8; ++i) {
                result |= (buffer.front() << (7 - i));
                buffer.pop();
            }
            raw_write(output, result, 1);
        }
    }

    // Se ci sono dei bit rimanenti (meno di 8), completare con 0
    if (!buffer.empty()) {
        char result = 0;
        int remaining_bits = buffer.size();
        for (int i = 0; i < remaining_bits; ++i) {
            result |= (buffer.front() << (7 - i));
            buffer.pop();
        }
        raw_write(output, result, 1); // Scriviamo l'ultimo byte
    }

    return 0;
}