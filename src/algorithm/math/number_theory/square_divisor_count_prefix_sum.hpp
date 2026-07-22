#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_SQUARE_DIVISOR_COUNT_PREFIX_SUM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_SQUARE_DIVISOR_COUNT_PREFIX_SUM_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace math{

class SquareDivisorCountPrefixSum{
public:
    static constexpr std::uint64_t maximum_argument = 1'000'000'000'000ULL;

    explicit SquareDivisorCountPrefixSum(std::uint64_t maximum_query):
        maximum_query_(maximum_query), limit_(choose_limit(maximum_query)){
        if(maximum_query > maximum_argument){
            throw std::invalid_argument(
                "SquareDivisorCountPrefixSum requires maximum_query <= 10^12");
        }
        build_prefixes();
    }

    std::uint64_t query(std::uint64_t n) const{
        if(n > maximum_query_){
            throw std::out_of_range("SquareDivisorCountPrefixSum query exceeds maximum_query");
        }
        if(n == 0) return 0;

        unsigned __int128 answer = 0;
        std::uint64_t previous_squarefree_count = 0;
        for(std::uint64_t left = 1, right; left <= n; left = right + 1){
            const std::uint64_t quotient = n / left;
            right = n / quotient;
            const std::uint64_t current_squarefree_count = squarefree_prefix(right);
            answer += static_cast<unsigned __int128>(
                          current_squarefree_count - previous_squarefree_count) *
                      divisor_prefix(quotient);
            previous_squarefree_count = current_squarefree_count;
        }
        if(answer > std::numeric_limits<std::uint64_t>::max()){
            throw std::overflow_error("square divisor count prefix sum does not fit uint64_t");
        }
        return static_cast<std::uint64_t>(answer);
    }

    std::vector<std::uint64_t> query_all(const std::vector<std::uint64_t>& queries) const{
        std::vector<std::uint64_t> result;
        result.reserve(queries.size());
        for(const auto n: queries) result.push_back(query(n));
        return result;
    }

private:
    static constexpr std::uint64_t precomputation_cap = 16'000'000;

    std::uint64_t maximum_query_;
    std::uint64_t limit_;
    std::vector<std::int8_t> mobius_;
    std::vector<std::uint32_t> squarefree_prefix_;
    std::vector<std::uint32_t> divisor_prefix_;

    static std::uint64_t integer_square_root(std::uint64_t n){
        auto root = static_cast<std::uint64_t>(std::sqrt(static_cast<long double>(n)));
        while(static_cast<unsigned __int128>(root + 1) * (root + 1) <= n) ++root;
        while(static_cast<unsigned __int128>(root) * root > n) --root;
        return root;
    }

    static std::uint64_t integer_cube_root_ceiling(std::uint64_t n){
        if(n == 0) return 0;
        auto root = static_cast<std::uint64_t>(std::cbrt(static_cast<long double>(n)));
        while(static_cast<unsigned __int128>(root) * root * root < n) ++root;
        while(root > 0 && static_cast<unsigned __int128>(root - 1) * (root - 1) *
                              (root - 1) >= n){
            --root;
        }
        return root;
    }

    static std::uint64_t choose_limit(std::uint64_t maximum_query){
        if(maximum_query > maximum_argument){
            throw std::invalid_argument(
                "SquareDivisorCountPrefixSum requires maximum_query <= 10^12");
        }
        if(maximum_query < 2) return 2;
        const std::uint64_t cube_root = integer_cube_root_ceiling(maximum_query);
        const std::uint64_t ideal = cube_root * cube_root + 1;
        const std::uint64_t minimum = integer_square_root(maximum_query) + 1;
        return std::min(maximum_query + 1,
                        std::max(minimum, std::min(ideal, precomputation_cap)));
    }

    void build_prefixes(){
        const auto size = static_cast<std::size_t>(limit_);
        mobius_.assign(size, 0);
        squarefree_prefix_.assign(size, 0);
        divisor_prefix_.assign(size, 0);
        std::vector<std::uint16_t> divisor_count(size, 0);
        std::vector<std::uint8_t> least_prime_exponent(size, 0);
        std::vector<std::uint32_t> primes;
        if(limit_ > 16) primes.reserve(static_cast<std::size_t>(limit_ / 12));

        mobius_[1] = 1;
        divisor_count[1] = 1;
        squarefree_prefix_[1] = 1;
        divisor_prefix_[1] = 1;
        for(std::uint64_t value = 2; value < limit_; ++value){
            const auto index = static_cast<std::size_t>(value);
            if(divisor_count[index] == 0){
                primes.push_back(static_cast<std::uint32_t>(value));
                mobius_[index] = -1;
                divisor_count[index] = 2;
                least_prime_exponent[index] = 1;
            }
            squarefree_prefix_[index] = squarefree_prefix_[index - 1] +
                                        static_cast<std::uint32_t>(mobius_[index] != 0);
            const std::uint64_t divisor_sum =
                static_cast<std::uint64_t>(divisor_prefix_[index - 1]) + divisor_count[index];
            if(divisor_sum > std::numeric_limits<std::uint32_t>::max()){
                throw std::overflow_error("divisor prefix precomputation exceeds uint32_t");
            }
            divisor_prefix_[index] = static_cast<std::uint32_t>(divisor_sum);

            for(const auto prime: primes){
                const std::uint64_t product = value * prime;
                if(product >= limit_) break;
                const auto product_index = static_cast<std::size_t>(product);
                if(value % prime == 0){
                    const auto exponent = static_cast<std::uint8_t>(
                        least_prime_exponent[index] + 1);
                    least_prime_exponent[product_index] = exponent;
                    divisor_count[product_index] = static_cast<std::uint16_t>(
                        divisor_count[index] /
                        (static_cast<std::uint16_t>(least_prime_exponent[index]) + 1) *
                        (static_cast<std::uint16_t>(exponent) + 1));
                    mobius_[product_index] = 0;
                    break;
                }
                least_prime_exponent[product_index] = 1;
                divisor_count[product_index] = static_cast<std::uint16_t>(
                    divisor_count[index] * 2);
                mobius_[product_index] = static_cast<std::int8_t>(-mobius_[index]);
            }
        }
    }

    std::uint64_t squarefree_prefix(std::uint64_t n) const{
        if(n < limit_) return squarefree_prefix_[static_cast<std::size_t>(n)];
        std::int64_t result = 0;
        const std::uint64_t root = integer_square_root(n);
        for(std::uint64_t i = 1; i <= root; ++i){
            result += static_cast<std::int64_t>(mobius_[static_cast<std::size_t>(i)]) *
                      static_cast<std::int64_t>(n / i / i);
        }
        return static_cast<std::uint64_t>(result);
    }

    std::uint64_t divisor_prefix(std::uint64_t n) const{
        if(n < limit_) return divisor_prefix_[static_cast<std::size_t>(n)];
        const std::uint64_t root = integer_square_root(n);
        unsigned __int128 half = 0;
        for(std::uint64_t i = 1; i <= root; ++i) half += n / i;
        const unsigned __int128 result =
            half * 2 - static_cast<unsigned __int128>(root) * root;
        return static_cast<std::uint64_t>(result);
    }
};

inline std::vector<std::uint64_t> square_divisor_count_prefix_sums(
    const std::vector<std::uint64_t>& queries){
    const auto maximum = queries.empty()
                             ? std::uint64_t{0}
                             : *std::max_element(queries.begin(), queries.end());
    return SquareDivisorCountPrefixSum(maximum).query_all(queries);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_SQUARE_DIVISOR_COUNT_PREFIX_SUM_HPP_INCLUDED
