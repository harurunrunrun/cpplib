// competitive-verifier: DISPLAY hidden

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace{

std::filesystem::path output_directory(int argc, char** argv){
    if(argc != 3 || std::string(argv[1]) != "--out-dir"){
        throw std::invalid_argument("usage: generator --out-dir DIRECTORY");
    }
    return argv[2];
}

std::uint32_t multiply_mod(
    std::uint32_t left,
    std::uint32_t right,
    std::uint32_t modulus
){
    return static_cast<std::uint32_t>(
        static_cast<std::uint64_t>(left) * right % modulus
    );
}

std::uint32_t power_mod(
    std::uint32_t base,
    std::uint32_t exponent,
    std::uint32_t modulus
){
    std::uint32_t result = 1;
    while(exponent != 0){
        if((exponent & 1) != 0){
            result = multiply_mod(result, base, modulus);
        }
        base = multiply_mod(base, base, modulus);
        exponent >>= 1;
    }
    return result;
}

std::uint32_t inverse_mod(std::uint32_t value, std::uint32_t prime){
    return power_mod(value, prime - 2, prime);
}

std::uint32_t bsgs_oracle(
    std::uint32_t prime,
    std::uint32_t root,
    std::uint32_t value
){
    const std::uint32_t block =
        static_cast<std::uint32_t>(std::sqrt(prime)) + 1;
    std::unordered_map<std::uint32_t, std::uint32_t> baby;
    baby.reserve(static_cast<std::size_t>(block) * 2);
    std::uint32_t current = 1;
    for(std::uint32_t exponent = 0; exponent < block; ++exponent){
        baby.emplace(current, exponent);
        current = multiply_mod(current, root, prime);
    }

    const std::uint32_t ratio =
        inverse_mod(power_mod(root, block, prime), prime);
    current = value;
    for(std::uint32_t giant = 0; giant <= block; ++giant){
        const auto iterator = baby.find(current);
        if(iterator != baby.end()){
            return static_cast<std::uint32_t>(
                (static_cast<std::uint64_t>(giant) * block +
                 iterator->second) % (prime - 1)
            );
        }
        current = multiply_mod(current, ratio, prime);
    }
    throw std::runtime_error("oracle failed");
}

void write_case(
    const std::filesystem::path& directory,
    const std::string& stem,
    std::uint32_t prime,
    std::uint32_t root,
    const std::vector<std::pair<std::uint32_t, std::uint32_t>>& queries
){
    std::ofstream input(directory / (stem + ".in"));
    std::ofstream output(directory / (stem + ".out"));
    input << prime << ' ' << root << ' ' << queries.size() << '\n';
    for(const auto& [value, expected]: queries){
        input << value << '\n';
        output << expected << '\n';
    }
}

void write_maximum_case(const std::filesystem::path& directory){
    constexpr std::uint32_t prime = 999999937;
    constexpr std::uint32_t root = 11;
    constexpr std::uint32_t query_count = 1'000'000;

    std::ofstream input(directory / "case_02.in");
    std::ofstream output(directory / "case_02.out");
    input << prime << ' ' << root << ' ' << query_count << '\n';

    std::uint32_t value = 1;
    for(std::uint32_t exponent = 0;
        exponent < query_count;
        ++exponent){
        input << value << '\n';
        output << exponent << '\n';
        value = multiply_mod(value, root, prime);
    }
}

} // namespace

int main(int argc, char** argv){
    const auto directory = output_directory(argc, argv);
    std::filesystem::create_directories(directory);

    constexpr std::uint32_t sample_prime = 998244353;
    constexpr std::uint32_t sample_root = 3;
    const std::vector<std::uint32_t> sample_values{
        1, 3, 9, 27, 578373382, 100
    };
    std::vector<std::pair<std::uint32_t, std::uint32_t>> sample_queries;
    for(std::uint32_t value: sample_values){
        sample_queries.push_back({
            value,
            bsgs_oracle(sample_prime, sample_root, value)
        });
    }
    write_case(
        directory,
        "case_00",
        sample_prime,
        sample_root,
        sample_queries
    );

    constexpr std::uint32_t small_prime = 101;
    constexpr std::uint32_t small_root = 2;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> random_queries;
    std::mt19937 engine(0x8c712d4fU);
    for(int iteration = 0; iteration < 500; ++iteration){
        const std::uint32_t exponent = static_cast<std::uint32_t>(
            engine() % (small_prime - 1)
        );
        random_queries.push_back({
            power_mod(small_root, exponent, small_prime),
            exponent
        });
    }
    write_case(
        directory,
        "case_01",
        small_prime,
        small_root,
        random_queries
    );

    write_maximum_case(directory);
}
