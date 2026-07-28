// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/number_theory/multiplicative/dirichlet_convolution.hpp"

namespace{

class ModInt{
    static constexpr std::uint64_t modulus = 1'000'000'007;
    std::uint64_t value_ = 0;

    static ModInt power(ModInt base, std::uint64_t exponent){
        ModInt result{1};
        while(exponent != 0){
            if((exponent & 1U) != 0) result = result * base;
            base = base * base;
            exponent >>= 1U;
        }
        return result;
    }

public:
    ModInt() = default;
    ModInt(long long value){
        value %= static_cast<long long>(modulus);
        if(value < 0) value += static_cast<long long>(modulus);
        value_ = static_cast<std::uint64_t>(value);
    }

    ModInt& operator+=(const ModInt& other){
        value_ += other.value_;
        if(value_ >= modulus) value_ -= modulus;
        return *this;
    }

    friend ModInt operator*(const ModInt& left, const ModInt& right){
        return ModInt{static_cast<long long>(
            left.value_ * right.value_ % modulus
        )};
    }

    friend ModInt operator/(const ModInt& left, const ModInt& right){
        assert(right.value_ != 0);
        return left * power(right, modulus - 2);
    }

    ModInt operator-() const{
        return value_ == 0
            ? ModInt{}
            : ModInt{static_cast<long long>(modulus - value_)};
    }

    friend bool operator==(const ModInt&, const ModInt&) = default;
};

std::vector<ModInt> naive_inverse(const std::vector<ModInt>& values){
    const std::size_t maximum = values.size() - 1;
    std::vector<ModInt> result(maximum + 1);
    result[1] = ModInt{1} / values[1];
    for(std::size_t value = 2; value <= maximum; ++value){
        ModInt sum;
        for(std::size_t divisor = 2; divisor <= value; ++divisor){
            if(value % divisor == 0){
                sum += values[divisor] * result[value / divisor];
            }
        }
        result[value] = -sum / values[1];
    }
    return result;
}

} // namespace

int main() {
    try{
        (void)math::dirichlet_inverse(std::vector<ModInt>{});
        return 2;
    }catch(const std::invalid_argument&){
    }

    unsigned long long seed;
    int rounds;
    if(!(std::cin >> seed >> rounds)) return 0;
    std::mt19937_64 random(seed);
    for(int round = 0; round < rounds; ++round){
        const std::size_t maximum = 1 + random() % 240;
        std::vector<ModInt> first(maximum + 1);
        std::vector<ModInt> second(maximum + 1);
        for(std::size_t index = 1; index <= maximum; ++index){
            first[index] = ModInt{
                static_cast<long long>(random() % 2'000'000'015)
                - 1'000'000'007
            };
            second[index] = ModInt{
                static_cast<long long>(random() % 2'000'000'015)
                - 1'000'000'007
            };
        }
        const auto result = math::dirichlet_convolution(first, second);
        for(std::size_t value = 1; value <= maximum; ++value){
            ModInt expected;
            for(std::size_t divisor = 1; divisor <= value; ++divisor){
                if(value % divisor == 0){
                    expected += first[divisor] * second[value / divisor];
                }
            }
            assert(result[value] == expected);
        }

        first[1] = ModInt{1 + static_cast<long long>(
            random() % 1'000'000'006
        )};
        const auto inverse = math::dirichlet_inverse(first);
        assert(inverse == naive_inverse(first));
        const auto identity = math::dirichlet_convolution(first, inverse);
        assert(identity[1] == ModInt{1});
        for(std::size_t value = 2; value <= maximum; ++value){
            assert(identity[value] == ModInt{0});
        }
    }
    std::cout << "OK\n";
}
