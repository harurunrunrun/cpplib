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

std::filesystem::path output_directory(int argc, char** argv){
    if(argc != 3 || std::string(argv[1]) != "--out-dir"){
        throw std::invalid_argument("usage: generator --out-dir DIRECTORY");
    }
    return argv[2];
}

std::vector<std::uint64_t> naive_prefix(std::size_t limit){
    std::vector<bool> squarefree(limit + 1, true);
    squarefree[0] = false;
    for(std::size_t prime = 2; prime <= limit / prime; ++prime){
        const std::size_t square = prime * prime;
        for(std::size_t multiple = square;
            multiple <= limit;
            multiple += square){
            squarefree[multiple] = false;
        }
    }
    std::vector<std::uint64_t> result(limit + 1);
    for(std::size_t value = 1; value <= limit; ++value){
        result[value] = result[value - 1] + (squarefree[value] ? 1U : 0U);
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
        0, 1, 2, 3, 4, 8, 9, 10, 99, 100,
        1'000'000'000'000ULL,
        999'999'999'999'999'989ULL,
        1'000'000'000'000'000'000ULL
    };
    std::mt19937_64 engine(0xd1310ba698dfb5acULL);
    for(int iteration = 0; iteration < 240; ++iteration){
        queries.push_back(engine() % (small_limit + 1));
    }

    const std::unordered_map<std::uint64_t, std::uint64_t> official_values{
        {1'000'000'000'000ULL, 607927102274ULL},
        {999'999'999'999'999'989ULL, 607927101854022744ULL},
        {1'000'000'000'000'000'000ULL, 607927101854022750ULL}
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
