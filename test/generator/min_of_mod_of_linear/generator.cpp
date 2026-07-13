// competitive-verifier: DISPLAY hidden

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <random>
#include <stdexcept>
#include <tuple>
#include <string>
#include <vector>

namespace{

using Query = std::tuple<long long, long long, long long, long long>;

long long brute(const Query& query){
    const auto [count, modulus, coefficient, constant] = query;
    long long result = modulus;
    for(long long index = 0; index < count; ++index){
        result = std::min(
            result,
            static_cast<long long>(
                (static_cast<__int128_t>(coefficient) * index + constant) % modulus
            )
        );
    }
    return result;
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
    std::vector<Query> queries{
        {1, 1, 0, 0},
        {1, 1'000'000'000, 999'999'999, 999'999'999},
        {1'000'000'000, 1'000'000'000, 999'999'999, 123'456'789},
        {500'000'000, 1'000'000'000, 0, 987'654'321},
    };
    std::vector<long long> expected{0, 999'999'999, 0, 987'654'321};

    std::mt19937_64 random(20260713);
    for(int iteration = 0; iteration < 800; ++iteration){
        const long long modulus = static_cast<long long>(random() % 5000 + 1);
        const Query query{
            static_cast<long long>(random() % 2000 + 1),
            modulus,
            static_cast<long long>(random() % static_cast<std::uint64_t>(modulus)),
            static_cast<long long>(random() % static_cast<std::uint64_t>(modulus)),
        };
        queries.push_back(query);
        expected.push_back(brute(query));
    }

    std::ofstream input(directory / "case_00.in");
    std::ofstream output(directory / "case_00.out");
    input << queries.size() << '\n';
    for(const auto& [count, modulus, coefficient, constant]: queries){
        input << count << ' ' << modulus << ' ' << coefficient << ' ' << constant << '\n';
    }
    for(long long value: expected) output << value << '\n';
}
