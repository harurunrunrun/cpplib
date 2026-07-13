// competitive-verifier: DISPLAY hidden

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace{

constexpr std::uint64_t MOD = 998244353;

std::filesystem::path output_directory(int argc, char** argv){
    if(argc != 3 || std::string(argv[1]) != "--out-dir"){
        throw std::invalid_argument("usage: generator --out-dir DIRECTORY");
    }
    return argv[2];
}

std::vector<std::uint64_t> naive_prefix(std::size_t limit){
    std::vector<std::uint64_t> phi(limit + 1);
    for(std::size_t value = 0; value <= limit; ++value) phi[value] = value;
    for(std::size_t prime = 2; prime <= limit; ++prime){
        if(phi[prime] != prime) continue;
        for(std::size_t multiple = prime;
            multiple <= limit;
            multiple += prime){
            phi[multiple] -= phi[multiple] / prime;
        }
    }
    std::vector<std::uint64_t> result(limit + 1);
    for(std::size_t value = 1; value <= limit; ++value){
        result[value] = (result[value - 1] + phi[value]) % MOD;
    }
    return result;
}

} // namespace

int main(int argc, char** argv){
    const auto directory = output_directory(argc, argv);
    std::filesystem::create_directories(directory);

    constexpr std::size_t small_limit = 200000;
    const auto small_expected = naive_prefix(small_limit);
    std::vector<std::uint64_t> queries{
        0, 1, 2, 3, 4, 9, 10, 99, 100,
        1'000'000'000ULL,
        9'999'999'999ULL,
        10'000'000'000ULL
    };
    std::mt19937_64 engine(0xb8e1afed6a267e96ULL);
    for(int iteration = 0; iteration < 240; ++iteration){
        queries.push_back(engine() % (small_limit + 1));
    }

    const std::unordered_map<std::uint64_t, std::uint64_t> official_values{
        {1'000'000'000ULL, 422516288ULL},
        {9'999'999'999ULL, 859827177ULL},
        {10'000'000'000ULL, 866849765ULL}
    };

    std::ofstream input(directory / "case_00.in");
    std::ofstream output(directory / "case_00.out");
    input << queries.size() << '\n';
    for(const std::uint64_t value: queries){
        input << value << '\n';
        if(value <= small_limit) output << small_expected[value] << '\n';
        else output << official_values.at(value) << '\n';
    }
}
