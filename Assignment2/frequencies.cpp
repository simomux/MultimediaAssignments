#include <iostream>
#include <print>
#include <fstream>
#include <iomanip>

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

  double d = 3.14;
  double e = static_cast<int>(d);

  const unsigned char arr[4] = { 0xaa, 0xbb, 0xcc, 0xdd };
  const int *p = reinterpret_cast<const int *>(arr);
  int* m = const_cast<int *>(p);>
  *m = 0;



  std::array<size_t, 256> count{};

  input.unsetf(std::ios::skipws);
  char c;
  while (input >> c) {
    uint8_t u = c;  // Implicit cast
    ++count[u];
  }

  for (int i = 0; i < 256; ++i) {
    if (count[i] > 0) {
      output << std::hex << std::setw(2) << std::setfill('0') << i
        << std::dec << '\t' << count[i] << '\n';
    }
  }

  return 0;
}
