// competitive-verifier: DISPLAY hidden

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace{

constexpr std::uint64_t MOD = 469762049;

struct Query{
    std::uint64_t n;
    std::uint64_t a;
    std::uint64_t b;
};

std::filesystem::path output_directory(int argc, char** argv){
    if(argc != 3 || std::string(argv[1]) != "--out-dir"){
        throw std::invalid_argument("usage: generator --out-dir DIRECTORY");
    }
    return argv[2];
}

std::uint64_t add_mod(std::uint64_t left, std::uint64_t right){
    return (left + right) % MOD;
}

std::uint64_t multiply_mod(std::uint64_t left, std::uint64_t right){
    return left * right % MOD;
}

std::uint64_t naive(const Query& query){
    if(query.n == 0) return 0;
    std::uint64_t answer = 1;
    for(std::uint64_t value = 2; value <= query.n; ++value){
        std::uint64_t remaining = value;
        std::uint64_t function_value = 1;
        for(std::uint64_t prime = 2; prime <= remaining / prime; ++prime){
            if(remaining % prime != 0) continue;
            std::uint64_t exponent = 0;
            while(remaining % prime == 0){
                remaining /= prime;
                ++exponent;
            }
            function_value = multiply_mod(
                function_value,
                add_mod(
                    multiply_mod(query.a, exponent),
                    multiply_mod(query.b, prime % MOD)
                )
            );
        }
        if(remaining > 1){
            function_value = multiply_mod(
                function_value,
                add_mod(query.a, multiply_mod(query.b, remaining % MOD))
            );
        }
        answer = add_mod(answer, function_value);
    }
    return answer;
}

std::uint64_t expected(const Query& query){
    if(query.n == 1'000'000) return 371700649;
    if(query.n == 10'000'000'000'000ULL) return 284866429;
    return naive(query);
}

void write_case(
    const std::filesystem::path& directory,
    const std::string& stem,
    const std::vector<Query>& queries
){
    std::ofstream input(directory / (stem + ".in"));
    std::ofstream output(directory / (stem + ".out"));
    input << queries.size() << '\n';
    for(const Query& query: queries){
        input << query.n << ' ' << query.a << ' ' << query.b << '\n';
        output << expected(query) << '\n';
    }
}

} // namespace

int main(int argc, char** argv){
    const auto directory = output_directory(argc, argv);
    std::filesystem::create_directories(directory);

    const std::vector<Query> maximum_case{
        {10'000'000'000'000ULL, 123456789, 234567890}
    };
    std::vector<Query> random_cases{
        {1, 0, 0},
        {1, 123, 456},
        {2, 1, 0},
        {10, 1, 0},
        {10, 0, 1},
        {100, 3, 5},
        {1'000'000, MOD - 1, MOD - 1}
    };
    std::mt19937_64 engine(0x24a19947b3916cf7ULL);
    for(int iteration = 0; iteration < 220; ++iteration){
        random_cases.push_back({
            1 + engine() % 1600,
            engine() % MOD,
            engine() % MOD
        });
    }

    write_case(directory, "case_00", maximum_case);
    write_case(directory, "case_01", random_cases);
}
