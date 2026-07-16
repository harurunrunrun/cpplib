// competitive-verifier: DISPLAY hidden

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace{

struct Gaussian{
    long long real = 0;
    long long imag = 0;

    friend bool operator==(const Gaussian&, const Gaussian&) = default;
};

struct PrimePower{
    Gaussian prime;
    int exponent;
};

struct Factorization{
    Gaussian unit{1, 0};
    std::vector<PrimePower> factors;
};

using i128 = __int128_t;
using u128 = __uint128_t;

std::uint64_t norm_u64(Gaussian value){
    const i128 real = value.real;
    const i128 imag = value.imag;
    const u128 real_magnitude = real < 0 ? static_cast<u128>(-real) : static_cast<u128>(real);
    const u128 imag_magnitude = imag < 0 ? static_cast<u128>(-imag) : static_cast<u128>(imag);
    return static_cast<std::uint64_t>(
        real_magnitude * real_magnitude + imag_magnitude * imag_magnitude
    );
}

std::uint64_t multiply_mod(
    std::uint64_t left,
    std::uint64_t right,
    std::uint64_t modulus
){
    return static_cast<std::uint64_t>(static_cast<u128>(left) * right % modulus);
}

std::uint64_t power_mod(
    std::uint64_t base,
    std::uint64_t exponent,
    std::uint64_t modulus
){
    std::uint64_t result = 1 % modulus;
    while(exponent != 0){
        if((exponent & 1U) != 0) result = multiply_mod(result, base, modulus);
        base = multiply_mod(base, base, modulus);
        exponent >>= 1U;
    }
    return result;
}

bool rational_prime(std::uint64_t value){
    if(value < 2) return false;
    for(const std::uint64_t small: {
            2ULL, 3ULL, 5ULL, 7ULL, 11ULL, 13ULL, 17ULL, 19ULL,
            23ULL, 29ULL, 31ULL, 37ULL}){
        if(value % small == 0) return value == small;
    }
    std::uint64_t odd_part = value - 1;
    int exponent_of_two = 0;
    while((odd_part & 1U) == 0){
        odd_part >>= 1U;
        ++exponent_of_two;
    }
    for(const std::uint64_t base: {
            2ULL, 325ULL, 9'375ULL, 28'178ULL, 450'775ULL,
            9'780'504ULL, 1'795'265'022ULL}){
        if(base % value == 0) continue;
        std::uint64_t witness = power_mod(base % value, odd_part, value);
        if(witness == 1 || witness == value - 1) continue;
        bool probably_prime = false;
        for(int squaring = 1; squaring < exponent_of_two; ++squaring){
            witness = multiply_mod(witness, witness, value);
            if(witness == value - 1){
                probably_prime = true;
                break;
            }
        }
        if(!probably_prime) return false;
    }
    return true;
}

bool gaussian_prime(Gaussian value){
    if(value.real == 0 || value.imag == 0){
        const long long component = value.real == 0 ? value.imag : value.real;
        return rational_prime(component) && component % 4 == 3;
    }
    return rational_prime(norm_u64(value));
}

bool divide_exact(Gaussian& value, Gaussian divisor){
    const i128 denominator = static_cast<i128>(divisor.real) * divisor.real
        + static_cast<i128>(divisor.imag) * divisor.imag;
    const i128 real_numerator = static_cast<i128>(value.real) * divisor.real
        + static_cast<i128>(value.imag) * divisor.imag;
    const i128 imag_numerator = static_cast<i128>(value.imag) * divisor.real
        - static_cast<i128>(value.real) * divisor.imag;
    if(real_numerator % denominator != 0 || imag_numerator % denominator != 0){
        return false;
    }
    value = {
        static_cast<long long>(real_numerator / denominator),
        static_cast<long long>(imag_numerator / denominator)
    };
    return true;
}

Factorization brute_factorization(Gaussian value){
    const long long value_norm = static_cast<long long>(norm_u64(value));
    std::vector<Gaussian> primes;
    for(long long real = 1; real * real <= value_norm; ++real){
        for(long long imag = 0; real * real + imag * imag <= value_norm; ++imag){
            const Gaussian candidate{real, imag};
            if(gaussian_prime(candidate)) primes.push_back(candidate);
        }
    }
    std::sort(primes.begin(), primes.end(), [](Gaussian left, Gaussian right){
        return std::pair{left.real, left.imag} < std::pair{right.real, right.imag};
    });

    Factorization result;
    for(const Gaussian prime: primes){
        int exponent = 0;
        while(divide_exact(value, prime)) ++exponent;
        if(exponent != 0) result.factors.push_back({prime, exponent});
    }
    result.unit = value;
    return result;
}

Factorization known_factorization(Gaussian value, std::vector<PrimePower> factors){
    std::sort(factors.begin(), factors.end(), [](const PrimePower& left, const PrimePower& right){
        return std::pair{left.prime.real, left.prime.imag}
            < std::pair{right.prime.real, right.prime.imag};
    });
    Gaussian remaining = value;
    for(const PrimePower& factor: factors){
        if(!gaussian_prime(factor.prime)){
            throw std::logic_error("non-prime in independent expected factorization");
        }
        for(int count = 0; count < factor.exponent; ++count){
            if(!divide_exact(remaining, factor.prime)){
                throw std::logic_error("known factor does not divide generated value");
            }
        }
    }
    if(norm_u64(remaining) != 1){
        throw std::logic_error("known factor list did not leave a unit");
    }
    return {remaining, std::move(factors)};
}

void print_factorization(std::ofstream& output, const Factorization& factorization){
    output << factorization.unit.real << ' ' << factorization.unit.imag << ' '
           << factorization.factors.size();
    for(const PrimePower& factor: factorization.factors){
        output << ' ' << factor.prime.real << ' ' << factor.prime.imag
               << ' ' << factor.exponent;
    }
    output << '\n';
}

std::filesystem::path output_directory(int argc, char** argv){
    if(argc != 3 || std::string(argv[1]) != "--out-dir"){
        throw std::invalid_argument("usage: generator --out-dir DIRECTORY");
    }
    return argv[2];
}

void write_small_cases(const std::filesystem::path& directory){
    std::vector<Gaussian> values{
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        {2, 0}, {3, 0}, {5, 0}, {4, 2}, {-18, 24}, {81, -153}
    };
    std::mt19937 random(0x895U);
    std::uniform_int_distribution<int> component(-70, 70);
    for(int iteration = 0; iteration < 900; ++iteration){
        Gaussian value{component(random), component(random)};
        if(value == Gaussian{}) value = {1, 0};
        values.push_back(value);
    }

    std::ofstream input(directory / "case_00.in");
    std::ofstream output(directory / "case_00.out");
    input << values.size() << '\n';
    for(const Gaussian value: values){
        input << value.real << ' ' << value.imag << '\n';
        print_factorization(output, brute_factorization(value));
    }
}

void write_boundary_cases(const std::filesystem::path& directory){
    const std::vector<Gaussian> values{
        {4'294'967'295LL, 65'536},
        {-65'536, 4'294'967'295LL},
        {4'294'967'291LL, 0},
        {-4'294'967'291LL, 0},
        {1'000'000'000LL, 1'000'000'000LL},
        {-1'000'000'000LL, 1'000'000'000LL}
    };
    const std::vector<Factorization> expected{
        known_factorization(values[0], {{{4'294'967'295LL, 65'536}, 1}}),
        known_factorization(values[1], {{{4'294'967'295LL, 65'536}, 1}}),
        known_factorization(values[2], {{{4'294'967'291LL, 0}, 1}}),
        known_factorization(values[3], {{{4'294'967'291LL, 0}, 1}}),
        known_factorization(values[4], {
            {{1, 1}, 19}, {{1, 2}, 9}, {{2, 1}, 9}
        }),
        known_factorization(values[5], {
            {{1, 1}, 19}, {{1, 2}, 9}, {{2, 1}, 9}
        })
    };

    std::ofstream input(directory / "case_01.in");
    std::ofstream output(directory / "case_01.out");
    input << values.size() << '\n';
    for(std::size_t index = 0; index < values.size(); ++index){
        input << values[index].real << ' ' << values[index].imag << '\n';
        print_factorization(output, expected[index]);
    }
}

} // namespace

int main(int argc, char** argv){
    const std::filesystem::path directory = output_directory(argc, argv);
    std::filesystem::create_directories(directory);
    write_small_cases(directory);
    write_boundary_cases(directory);
}
