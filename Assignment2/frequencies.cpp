#include <iostream>
#include <print>
#include <fstream>
#include <iomanip>
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

  // std::array<size_t, 256> count{};
  std::map<uint8_t, size_t> count {};

  input.unsetf(std::ios::skipws);
  char c;
  while (input >> c) {
    uint8_t u = c;  // Implicit cast
    count[u]++;
  }


  for (const auto& it : count) {

    output << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(it.first) /* +it.first // Implicit cast with '+' operator (just to flex)*/
      << std::dec << '\t' << it.second << '\n';
  }

  return 0;
}
