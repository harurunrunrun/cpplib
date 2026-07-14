// competitive-verifier: DISPLAY hidden

#include <bit>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace{

bool power_less_equal(std::uint64_t base, unsigned int exponent, std::uint64_t value){
    std::uint64_t power = 1;
    for(unsigned int index = 0; index < exponent; ++index){
        if(base != 0 && power > value / base) return false;
        power *= base;
    }
    return true;
}

std::uint64_t oracle(std::uint64_t value, unsigned int exponent){
    if(exponent == 1 || value <= 1) return value;
    if(exponent >= 64) return 1;
    const unsigned int bits = std::bit_width(value);
    const unsigned int root_bits = (bits + exponent - 1) / exponent;
    std::uint64_t lower = 0;
    std::uint64_t upper = std::uint64_t(1) << root_bits;
    while(lower + 1 < upper){
        const std::uint64_t middle = lower + (upper - lower) / 2;
        if(power_less_equal(middle, exponent, value)) lower = middle;
        else upper = middle;
    }
    return lower;
}

std::filesystem::path output_directory(int argc, char** argv){
    if(argc != 3 || std::string(argv[1]) != "--out-dir"){
        throw std::invalid_argument("usage: generator --out-dir DIRECTORY");
    }
    return argv[2];
}

} // namespace

int main(int argc, char** argv){
    const auto directory = output_directory(argc, argv);
    std::filesystem::create_directories(directory);
    std::vector<std::pair<std::uint64_t, unsigned int>> queries{
        {0, 1}, {0, 64}, {1, 64}, {2, 64},
        {std::numeric_limits<std::uint64_t>::max(), 1},
        {std::numeric_limits<std::uint64_t>::max(), 2},
        {std::numeric_limits<std::uint64_t>::max(), 3},
        {std::numeric_limits<std::uint64_t>::max(), 64},
    };
    std::mt19937_64 random(20260713);
    for(int iteration = 0; iteration < 1200; ++iteration){
        queries.emplace_back(random(), static_cast<unsigned int>(random() % 64 + 1));
    }

    std::ofstream input(directory / "case_00.in");
    std::ofstream output(directory / "case_00.out");
    input << queries.size() << '\n';
    for(const auto& [value, exponent]: queries){
        input << value << ' ' << exponent << '\n';
        output << oracle(value, exponent) << '\n';
    }
}
