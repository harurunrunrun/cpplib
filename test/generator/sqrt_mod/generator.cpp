// competitive-verifier: DISPLAY hidden

#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace{

using u64 = std::uint64_t;
using u128 = __uint128_t;

bool is_prime(u64 value){
    if(value < 2) return false;
    for(u64 divisor = 2; divisor * divisor <= value; ++divisor){
        if(value % divisor == 0) return false;
    }
    return true;
}

std::optional<u64> brute(u64 value, u64 prime){
    for(u64 root = 0; root < prime; ++root){
        if(root * root % prime == value) return root;
    }
    return std::nullopt;
}

u64 power_mod(u64 base, u64 exponent, u64 modulus){
    u64 result = 1;
    while(exponent != 0){
        if((exponent & 1U) != 0){
            result = static_cast<u64>(static_cast<u128>(result) * base % modulus);
        }
        base = static_cast<u64>(static_cast<u128>(base) * base % modulus);
        exponent >>= 1U;
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
    std::vector<std::pair<u64, u64>> queries;
    std::vector<long long> expected;
    for(u64 prime = 2; prime <= 300; ++prime){
        if(!is_prime(prime)) continue;
        for(u64 value = 0; value < prime; ++value){
            queries.emplace_back(value, prime);
            const auto root = brute(value, prime);
            expected.push_back(root ? static_cast<long long>(*root) : -1);
        }
    }

    for(u64 prime: {998244353ULL, 1'000'000'007ULL}){
        const std::array<u64, 6> roots{{0, 1, 2, 1234567, prime / 2, prime - 1}};
        for(u64 root: roots){
            const u64 value = static_cast<u64>(static_cast<u128>(root) * root % prime);
            queries.emplace_back(value, prime);
            expected.push_back(static_cast<long long>(std::min(root, prime - root)));
        }
        u64 non_residue = 2;
        while(power_mod(non_residue, (prime - 1) / 2, prime) != prime - 1){
            ++non_residue;
        }
        queries.emplace_back(non_residue, prime);
        expected.push_back(-1);
    }

    std::ofstream input(directory / "case_00.in");
    std::ofstream output(directory / "case_00.out");
    input << queries.size() << '\n';
    for(std::size_t index = 0; index < queries.size(); ++index){
        input << queries[index].first << ' ' << queries[index].second << '\n';
        output << expected[index] << '\n';
    }
}
