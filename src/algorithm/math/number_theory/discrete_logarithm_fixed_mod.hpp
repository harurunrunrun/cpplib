#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_DISCRETE_LOGARITHM_FIXED_MOD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_DISCRETE_LOGARITHM_FIXED_MOD_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace math{

inline constexpr std::uint32_t discrete_logarithm_fixed_mod_max_modulus =
    1'000'000'000;

class FixedModDiscreteLogarithm{
private:
    static constexpr std::uint32_t direct_table_limit = 1U << 20;
    static constexpr std::uint32_t missing_log =
        std::numeric_limits<std::uint32_t>::max();

    class UIntHashTable{
    private:
        std::vector<std::uint32_t> keys_;
        std::vector<std::uint32_t> values_;
        std::size_t mask_ = 0;

        static std::uint64_t mix(std::uint64_t value){
            value += 0x9e3779b97f4a7c15ULL;
            value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
            value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
            return value ^ (value >> 31);
        }

    public:
        explicit UIntHashTable(std::size_t expected_size){
            std::size_t capacity = 1;
            while(capacity < expected_size * 2 + 1){
                capacity <<= 1;
            }
            keys_.assign(capacity, 0);
            values_.assign(capacity, 0);
            mask_ = capacity - 1;
        }

        void insert_if_absent(std::uint32_t key, std::uint32_t value){
            std::size_t position =
                static_cast<std::size_t>(mix(key)) & mask_;
            while(keys_[position] != 0 && keys_[position] != key){
                position = (position + 1) & mask_;
            }
            if(keys_[position] == 0){
                keys_[position] = key;
                values_[position] = value;
            }
        }

        std::uint32_t find(std::uint32_t key) const{
            std::size_t position =
                static_cast<std::size_t>(mix(key)) & mask_;
            while(keys_[position] != 0){
                if(keys_[position] == key) return values_[position];
                position = (position + 1) & mask_;
            }
            return 0;
        }
    };

    std::uint32_t modulus_;
    std::uint32_t primitive_root_;
    bool direct_;
    std::uint32_t farey_denominator_limit_ = 0;
    std::uint32_t farey_grid_size_ = 0;
    std::vector<std::uint32_t> logarithm_table_;
    std::vector<std::uint32_t> farey_lower_;
    std::vector<std::uint32_t> farey_upper_;

    static std::uint32_t multiply_mod(
        std::uint32_t left,
        std::uint32_t right,
        std::uint32_t modulus
    ){
        return static_cast<std::uint32_t>(
            static_cast<std::uint64_t>(left) * right % modulus
        );
    }

    static std::uint32_t power_mod(
        std::uint32_t base,
        std::uint64_t exponent,
        std::uint32_t modulus
    ){
        std::uint32_t result = 1 % modulus;
        while(exponent != 0){
            if((exponent & 1) != 0){
                result = multiply_mod(result, base, modulus);
            }
            base = multiply_mod(base, base, modulus);
            exponent >>= 1;
        }
        return result;
    }

    static std::uint32_t integer_square_root(std::uint64_t value){
        std::uint32_t low = 0;
        std::uint32_t high = 1;
        while(static_cast<std::uint64_t>(high) * high <= value){
            high <<= 1;
        }
        while(high - low > 1){
            const std::uint32_t middle = low + (high - low) / 2;
            if(static_cast<std::uint64_t>(middle) * middle <= value){
                low = middle;
            }else{
                high = middle;
            }
        }
        return low;
    }

    static std::uint32_t integer_cube_root(std::uint32_t value){
        std::uint32_t result = 0;
        while(
            static_cast<std::uint64_t>(result + 1) *
            (result + 1) * (result + 1) <= value
        ){
            ++result;
        }
        return result;
    }

    static bool is_prime(std::uint32_t value){
        if(value < 2) return false;
        if(value % 2 == 0) return value == 2;
        for(std::uint32_t divisor = 3;
            static_cast<std::uint64_t>(divisor) * divisor <= value;
            divisor += 2){
            if(value % divisor == 0) return false;
        }
        return true;
    }

    static bool is_primitive_root(
        std::uint32_t value,
        std::uint32_t prime
    ){
        std::uint32_t remaining = prime - 1;
        for(std::uint32_t factor = 2;
            static_cast<std::uint64_t>(factor) * factor <= remaining;
            ++factor){
            if(remaining % factor != 0) continue;
            if(power_mod(value, (prime - 1) / factor, prime) == 1){
                return false;
            }
            while(remaining % factor == 0) remaining /= factor;
        }
        if(remaining > 1 &&
           power_mod(value, (prime - 1) / remaining, prime) == 1){
            return false;
        }
        return true;
    }

    void preprocess_direct(){
        logarithm_table_.assign(modulus_, missing_log);
        std::uint32_t current = 1;
        for(std::uint32_t exponent = 0;
            exponent < modulus_ - 1;
            ++exponent){
            if(logarithm_table_[current] != missing_log){
                throw std::invalid_argument(
                    "primitive_root is not a primitive root"
                );
            }
            logarithm_table_[current] = exponent;
            current = multiply_mod(current, primitive_root_, modulus_);
        }
        for(std::uint32_t value = 1; value < modulus_; ++value){
            if(logarithm_table_[value] == missing_log){
                throw std::invalid_argument(
                    "primitive_root is not a primitive root"
                );
            }
        }
    }

    static std::vector<std::uint32_t> prime_logarithms(
        std::uint32_t modulus,
        std::uint32_t primitive_root,
        const std::vector<std::uint32_t>& primes
    ){
        const std::uint32_t block_size =
            integer_square_root(modulus / primes.size()) + 2;
        const std::uint32_t giant_count = modulus / block_size + 3;
        UIntHashTable giant_steps(giant_count);

        const std::uint32_t giant_ratio =
            power_mod(primitive_root, block_size, modulus);
        std::uint32_t current_giant = giant_ratio;
        for(std::uint32_t giant = 1;
            giant <= giant_count;
            ++giant){
            giant_steps.insert_if_absent(current_giant, giant);
            current_giant =
                multiply_mod(current_giant, giant_ratio, modulus);
        }

        std::vector<std::uint32_t> result(primes.size(), missing_log);
        std::size_t remaining = primes.size();
        std::uint32_t current_baby = 1;
        for(std::uint32_t baby = 0;
            baby < block_size && remaining != 0;
            ++baby){
            for(std::size_t index = 0; index < primes.size(); ++index){
                if(result[index] != missing_log) continue;
                const std::uint32_t key =
                    multiply_mod(primes[index], current_baby, modulus);
                const std::uint32_t giant = giant_steps.find(key);
                if(giant == 0) continue;
                const std::uint64_t exponent =
                    static_cast<std::uint64_t>(giant) * block_size - baby;
                result[index] = static_cast<std::uint32_t>(
                    exponent % (modulus - 1)
                );
                --remaining;
            }
            current_baby =
                multiply_mod(current_baby, primitive_root, modulus);
        }
        if(remaining != 0){
            throw std::runtime_error(
                "fixed-mod discrete logarithm preprocessing failed"
            );
        }
        return result;
    }

    static std::uint32_t pack_fraction(
        std::uint32_t numerator,
        std::uint32_t denominator
    ){
        return (numerator << 16) | denominator;
    }

    static std::pair<std::uint32_t, std::uint32_t> unpack_fraction(
        std::uint32_t packed
    ){
        return {packed >> 16, packed & 0xffffU};
    }

    void preprocess_farey_tables(){
        const std::uint32_t cube_root = integer_cube_root(modulus_);
        farey_denominator_limit_ = 1;
        while(farey_denominator_limit_ <= cube_root){
            farey_denominator_limit_ <<= 1;
        }
        farey_grid_size_ =
            farey_denominator_limit_ * farey_denominator_limit_;

        std::vector<std::uint32_t> exact(farey_grid_size_ + 1, 0);
        for(std::uint32_t numerator = 0;
            numerator <= farey_denominator_limit_;
            ++numerator){
            const std::uint32_t first_denominator =
                numerator == 1 ? 1 : numerator + 1;
            for(std::uint32_t denominator = first_denominator;
                denominator <= farey_denominator_limit_;
                ++denominator){
                const std::uint32_t coordinate =
                    static_cast<std::uint32_t>(
                        static_cast<std::uint64_t>(numerator) *
                        farey_grid_size_ / denominator
                    );
                if(exact[coordinate] == 0){
                    exact[coordinate] =
                        pack_fraction(numerator, denominator);
                }
            }
        }

        farey_upper_.resize(exact.size());
        std::uint32_t nearest = 0;
        for(std::size_t index = exact.size(); index-- > 0;){
            if(exact[index] != 0) nearest = exact[index];
            farey_upper_[index] = nearest;
        }

        nearest = 0;
        for(std::size_t index = 0; index < exact.size(); ++index){
            if(exact[index] != 0) nearest = exact[index];
            exact[index] = nearest;
        }
        farey_lower_ = std::move(exact);
    }

    void preprocess_large(){
        preprocess_farey_tables();

        const std::uint32_t square_root =
            integer_square_root(modulus_) + 2;
        std::vector<std::uint32_t> least_prime(square_root + 1, 0);
        std::vector<std::uint32_t> primes;
        for(std::uint32_t value = 2; value <= square_root; ++value){
            if(least_prime[value] != 0) continue;
            least_prime[value] = value;
            primes.push_back(value);
            if(value > square_root / value) continue;
            for(std::uint32_t multiple = value * value;
                multiple <= square_root;
                multiple += value){
                if(least_prime[multiple] == 0){
                    least_prime[multiple] = value;
                }
            }
        }

        const std::vector<std::uint32_t> prime_logs =
            prime_logarithms(modulus_, primitive_root_, primes);
        logarithm_table_.assign(farey_grid_size_ + 1, 0);
        for(std::size_t index = 0; index < primes.size(); ++index){
            logarithm_table_[primes[index]] = prime_logs[index];
        }
        logarithm_table_[1] = 0;

        const std::uint32_t order = modulus_ - 1;
        for(std::uint32_t value = 2; value <= square_root; ++value){
            if(least_prime[value] == value) continue;
            const std::uint64_t sum =
                static_cast<std::uint64_t>(
                    logarithm_table_[least_prime[value]]
                ) + logarithm_table_[value / least_prime[value]];
            logarithm_table_[value] =
                static_cast<std::uint32_t>(sum % order);
        }

        const std::uint32_t minus_one_log = order / 2;
        for(std::uint32_t value = square_root + 1;
            value <= farey_grid_size_;
            ++value){
            const std::uint32_t quotient = modulus_ / value;
            const std::uint32_t remainder = modulus_ % value;
            const std::uint64_t result =
                static_cast<std::uint64_t>(minus_one_log) +
                logarithm_table_[remainder] + order -
                logarithm_table_[quotient];
            logarithm_table_[value] =
                static_cast<std::uint32_t>(result % order);
        }
    }

public:
    FixedModDiscreteLogarithm(
        std::uint32_t prime_modulus,
        std::uint32_t primitive_root
    )
        : modulus_(prime_modulus),
          primitive_root_(primitive_root),
          direct_(prime_modulus <= direct_table_limit){
        if(prime_modulus > discrete_logarithm_fixed_mod_max_modulus ||
           !is_prime(prime_modulus)){
            throw std::invalid_argument(
                "prime_modulus must be a prime in [2, 1000000000]"
            );
        }
        if(primitive_root == 0 || primitive_root >= prime_modulus ||
           !is_primitive_root(primitive_root, prime_modulus)){
            throw std::invalid_argument(
                "primitive_root is not a primitive root"
            );
        }

        if(direct_){
            preprocess_direct();
        }else{
            preprocess_large();
        }
    }

    std::uint32_t modulus() const noexcept{
        return modulus_;
    }

    std::uint32_t primitive_root() const noexcept{
        return primitive_root_;
    }

    std::uint32_t log(std::uint32_t value) const{
        if(value == 0 || value >= modulus_){
            throw std::invalid_argument(
                "value must be in [1, prime_modulus)"
            );
        }
        if(direct_) return logarithm_table_[value];

        const std::uint32_t coordinate =
            static_cast<std::uint32_t>(
                static_cast<std::uint64_t>(value) *
                farey_grid_size_ / modulus_
            );
        auto [numerator, denominator] =
            unpack_fraction(farey_lower_[coordinate]);
        std::int64_t difference =
            static_cast<std::int64_t>(value) * denominator -
            static_cast<std::int64_t>(modulus_) * numerator;
        const std::int64_t absolute_lower =
            difference < 0 ? -difference : difference;
        if(absolute_lower > farey_grid_size_){
            const auto upper =
                unpack_fraction(farey_upper_[coordinate]);
            numerator = upper.first;
            denominator = upper.second;
            difference =
                static_cast<std::int64_t>(value) * denominator -
                static_cast<std::int64_t>(modulus_) * numerator;
        }

        const std::uint32_t absolute_difference =
            static_cast<std::uint32_t>(
                difference < 0 ? -difference : difference
            );
        if(absolute_difference == 0 ||
           absolute_difference > farey_grid_size_){
            throw std::runtime_error(
                "fixed-mod discrete logarithm approximation failed"
            );
        }

        const std::uint32_t order = modulus_ - 1;
        std::uint64_t result = logarithm_table_[absolute_difference];
        if(difference < 0) result += order / 2;
        result += order - logarithm_table_[denominator];
        return static_cast<std::uint32_t>(result % order);
    }

    std::uint32_t operator()(std::uint32_t value) const{
        return log(value);
    }

    std::vector<std::uint32_t> logs(
        const std::vector<std::uint32_t>& values
    ) const{
        std::vector<std::uint32_t> result;
        result.reserve(values.size());
        for(std::uint32_t value: values){
            result.push_back(log(value));
        }
        return result;
    }
};

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_DISCRETE_LOGARITHM_FIXED_MOD_HPP_INCLUDED
