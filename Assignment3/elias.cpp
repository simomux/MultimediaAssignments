
#include <string>
#include <fstream>
#include <iostream>
#include <string>
#include <bit>
#include <vector>
#include <iterator>

template <typename T>
std::istream& raw_read(std::istream& input, T& val, const size_t size = sizeof(T))
{
    return input.read(reinterpret_cast<char*>(&val), size);
}

template<typename T>
std::ostream& raw_write(std::ostream& output, const T& val, const size_t size = sizeof(val))
{
    return output.write(reinterpret_cast<const char*>(&val), size);
};

class bitwriter {
private:
    std::ostream& os_;
    uint8_t buffer_ = 0;
    size_t n_ = 0;

    void writebit(uint32_t bit){
        buffer_ = (buffer_ << 1) | (bit & 1) ;
        ++n_;
        if (n_ == 8) {
            raw_write(os_, buffer_);
            n_ = 0;
        }
    }
public:
    bitwriter(std::ostream& os) : os_(os) {};

    ~bitwriter(){
        flush();
    };

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

class bitreader {
private:
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

    std::istream& operator()(uint32_t& u, size_t n) {
        u = 0;
        while (n --> 0) {
            u = (u << 1) | readbit();
        }
        return is_;
    }

    operator bool() const {
        return is_.good();
    }
};

uint32_t map(int32_t x)
{

}

int unmap(uint32_t x)
{

}

void compress(const std::string &infile, const std::string &outfile)
{
    std::ifstream is(infile);

    if (!is) {
        std::println(std::cerr, "Error opening input file!");
        exit(EXIT_FAILURE);
    }

    std::vector<int> numbers{
        std::istream_iterator<int>{is},
        std::istream_iterator<int>{}
    };

    std::ofstream os(outfile, std::ios::binary);

    if (!os) {
        std::println(std::cerr, "Error opening output file!");
        exit(EXIT_FAILURE);
    }

    bitwriter bw(os);

    for (const auto& it : numbers) {
        int val = map(it);
        bw(val, std::bit_width(val)*2-1);
    }
}

void decompress(const std::string &infile, const std::string &outfile)
{
    std::ifstream is(infile, std::ios::binary);

    if (!is) {
        std::println(std::cerr, "Error opening input file!");
        exit(EXIT_FAILURE);
    }

    std::ofstream os(outfile);

    if (!os) {
        std::println(std::cerr, "Error opening output file!");
        exit(EXIT_FAILURE);
    }

    bitreader br(is);

    while (true) {
        int nbits = 0;
        uint32_t u;

        while (br(u, 1)) {
            if (u == 1) {
                break;
            }
            ++nbits;
            if (!is) {
                break;
            }
        }
        br(u, nbits);
        uint32_t val = (1<<nbits) | u;
        std::print(os, "{}", unmap(val));
    }
}

int main(int argc, const char * argv[]) {
    using namespace std::string_literals;

    if (argv[1] == "c"s) {
        compress(argv[2], argv[3]);
    }

    if (argv[1] == "d"s) {
        decompress(argv[2], argv[3]);
    }
}