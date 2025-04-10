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
#include <queue>

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

template<typename T>
void make_codes(node<T>* n, uint32_t code, uint32_t len, std::unordered_map<T, node<T>*>& map)
{
    if (n->left_ == nullptr) {
        n->code_ = code;
        n->len_ = len;
        map[n->sym_] = n;
    } else {
        make_codes(n->left_, code << 1, len+1, map);
        make_codes(n->right_, (code << 1) | 1, len+1, map);
    }
}

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
    auto f = for_each(begin(v), end(v), frequency<uint8_t>{});

    using nodeptr = node<uint8_t>*;
    auto pred = [](const nodeptr a, const nodeptr b) {
        return a->freq_ > b->freq_;
    };

    std::priority_queue<nodeptr, std::vector<nodeptr>, decltype(pred)> nodes(pred);

    // Tuple unpacking
    for (const auto& [sym, freq] : f.counter_) {
        auto n = new node(sym, freq);
        nodes.push(n);
    }

    while (nodes.size() > 1) {
        auto n1 = nodes.top();
        nodes.pop();
        auto n2 = nodes.top();
        nodes.pop();
        auto n = new node(n1, n2);
        nodes.push(n);
    }

    auto root = nodes.top();
    nodes.pop();


    std::unordered_map<uint8_t, node<uint8_t>* > map;
    make_codes(root, 0, 0, map);

    for (const auto& [sym, n] : map) {
        std::println("{:c} {:0{}b}", sym, n->code_, n->len_);
    }

    std::ofstream os(outfile, std::ios::binary);

    if (!os) {
        std::println(std::cerr, "Error opening output file!");
        exit(EXIT_FAILURE);
    }

    os << "HUFFMAN1";
    uint8_t table_size = static_cast<uint8_t>(map.size());
    os.put(table_size);
    bitwriter bw(os);
    for (const auto& [sym, n] : map) {
        bw(sym, 8);
        bw(n->len_, 5);
        bw(n->code_, n->len_);
    }
    bw(static_cast<uint32_t>(v.size()), 32);
    for (const auto& x : v) {
        auto n = map[x];
        bw(n->code_, n->len_);
    }
}

void decompress(const std::string &infile, const std::string &outfile)
{
    std::ifstream is(infile, std::ios::binary);

    if (!is) {
        std::println(std::cerr, "Error opening input file!");
        exit(EXIT_FAILURE);
    }

    std::string header(8, ' ');
    is.read(header.data(), 8);
    size_t table_len = is.get();

    if (table_len == 0) {
        table_len = 256;
    }

    // typedef std::tuple<uint8_t, uint32_t, uint32_t> table_entry;
    using table_entry = std::tuple<uint8_t, uint32_t, uint32_t>;    // sym, code, len
    std::vector<table_entry> table;

    bitreader br(is);

    for (size_t i = 0; i < table_len; i++) {
        uint32_t sym, code, len;
        br(sym, 8);
        br(len, 5);
        br(code, len);

        table.emplace_back(sym, code, len);
    }

    uint32_t n;
    br(n, 32);

    sort(table.begin(), table.end(),
        [](const table_entry& a, const table_entry& b) {
            return get<2>(a) < get<2>(b);
        });

    std::ofstream os(outfile, std::ios::binary);

    if (!os) {
        std::println(std::cerr, "Error opening output file!");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < n; i++) {
        size_t pos;
        uint32_t code = 0;
        uint32_t len = 0;
        bool found = false;
        for (pos = 0; pos < table_len; pos++) {
            while (len < get<2>(table[pos])) {
                uint32_t bit;
                br(bit, 1);
                code = (code << 1) | bit;
                ++len;
            }
            if (code == get<1>(table[pos])) {
                found = true;
                break;
            }
            ++pos;
        }

        if (!found) {
            std::println(std::cerr, "Symbol not found!");
            exit(EXIT_FAILURE);
        }
		os.put(get<0>(table[pos]));
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