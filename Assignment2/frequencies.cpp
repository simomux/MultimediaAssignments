#include <iostream>
#include <print>
#include <fstream>
#include <iterator>
#include <map>

int main(int argc, char *argv[])
{
  if (argc < 3) {
    std::println(std::cerr, "Usage: {} input_file output_file", argv[0]);
    return 1;
  }

  std::ifstream input(argv[1], std::ios::binary);

  if (!input) {
    std::println(std::cerr, "Error opening input file\n");
    return 1;
  }


  std::ofstream output(argv[2], std::ios::binary);
  if (!output) {
    std::println(std::cerr, "Error opening output file\n");
    return 1;
  }

  std::map <uint8_t, int> occurrencies{};

  while (!input.eof()) {
    occurrencies[input.get()]++;
  }

  for (auto& [key, value] : occurrencies) {
    output << key << "\t" << value << "\n";
  }

}