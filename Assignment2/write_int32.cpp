/*
* Exercise 2
Write a command line program in C++ with this syntax:
write_int32 <filein.txt> <fileout.bin>
The first parameter is the name of a text file that contains signed base 10 integers from -1000 to 1000 separated by whitespace. The program must create a new file, with the name passed as the second parameter, with the same numbers saved as 32-bit binary little endian numbers in 2's complement.
 * */
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>

template<typename T>
std::ostream& raw_write(std::ostream& output, const T& val, const size_t size = sizeof(val))
{
  output.write(reinterpret_cast<const char*>(&val), size);
  return output;
};

int main(int argc, char* argv[]) {

  if (argc != 3) {
    std::println(std::cerr, "Usage: {} input_file output_file", argv[0]);
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

  std::vector<int32_t> numbers{
    std::istream_iterator<int32_t>(input),
    std::istream_iterator<int32_t>()
  };

  // Manual version (machine independent)
  /*for (const auto& it : numbers) {
    output.put((it >> 0) & 0xFF);
    output.put((it >> 8) & 0xFF);
    output.put((it >> 16) & 0xFF);
    output.put((it >> 24) & 0xFF);
  }*/


  // Machine dependant version (different for little endian and big endian)

  /* 1st version
   * for (const int32_t& it : numbers) {
      const int32_t* p = &it;
      const char* c = reinterpret_cast<const char*>(p);
      output.write(c, 4);
    }
   */


  /* 2n version
   *for (const int32_t& it : numbers) {
    output.write(reinterpret_cast<const char*>(&it), 4);
  }*/

  // 3rd version
  // output.write(reinterpret_cast<const char*>(numbers.data()), numbers.size() * sizeof(int32_t));

  for (const auto& number : numbers) {
    raw_write(output, number);
  }

  return 0;
}