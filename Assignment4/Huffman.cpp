#include <string>
#include <fstream>
#include <iostream>
#include <string>
#include <bit>
#include <vector>
#include <iterator>
#include <map>
#include <unordered_map>
#include <algorithm>

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

template<typename T>
struct frequency {
    std::unordered_map<T, uint32_t> counter_;

    void operator()(const T& val) {
        ++counter_[val];
    }


};

template<typename T>
struct node {
    T sym_;
    uint32_t freq_;
    uint32_t code_, len_;

    node* left_ = nullptr;
    node* right_ = nullptr;

    node(T sym, uint32_t freq) : sym_(std::move(sym)), freq_(freq) {}
    node(node* left, node* right) : freq_(left->freq_ + right->freq_), left_(left), right_(right) {}

};

void compress(const std::string &infile, const std::string &outfile)
{
    std::ifstream is(infile);

    if (!is) {
        std::println(std::cerr, "Error opening input file!");
        exit(EXIT_FAILURE);
    }

    // Fastest version for reading data from file

    is.seekg(0, std::ios::end);
    size_t filesize = is.tellg();
    is.seekg(0);    // Go back to beginning of the file

    std::vector<uint8_t> v(filesize);

    raw_read(is, v[0], filesize);

    /*frequency<uint8_t> freq;
    for (const auto& x : v) {
        freq(x);
    }*/

    // Same thing as above, just more fancy
    auto f = std::for_each(begin(v), end(v), frequency<uint32_t>{});

    std::vector<node<uint8_t>*> nodes;

    // Tuple unpacking
    for (const auto& [sym, freq] : f.counter_) {
        std::println("{}: {}", sym, freq);
        auto n = new node(sym, freq);
        nodes.push_back(n);
    }

    auto pred = [](node<uint8_t>* a, node<uint8_t>* b) {
        return a->freq_ > b->freq_;
    };

    sort(nodes.begin(), nodes.end(), pred);

    while (nodes.size() > 1) {
        auto n1 = nodes.back();
        nodes.pop_back();
        auto n2 = nodes.back();
        nodes.pop_back();
        auto n = new node(n1, n2);


        // Binary search on random access ordered sequences
        auto it = std::lower_bound(nodes.begin(), nodes.end(), n, pred);

        nodes.insert(it, n);
    }

    std::ofstream os(outfile, std::ios::binary);

    if (!os) {
        std::println(std::cerr, "Error opening output file!");
        exit(EXIT_FAILURE);
    }

    bitwriter bw(os);
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