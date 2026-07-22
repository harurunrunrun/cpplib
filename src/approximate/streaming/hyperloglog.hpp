#ifndef CPPLIB_SRC_APPROXIMATE_STREAMING_HYPERLOGLOG_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_STREAMING_HYPERLOGLOG_HPP_INCLUDED

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

#include "hash.hpp"

template<unsigned Precision = 12, class Key = std::uint64_t>
class HyperLogLog {
    static_assert(4U <= Precision && Precision <= 18U,
                  "Precision must be in [4, 18]");
    static constexpr std::size_t register_count_ = std::size_t{1} << Precision;

    std::array<std::uint8_t, register_count_> registers_{};
    std::uint64_t seed_;

    [[nodiscard]] static long double alpha() noexcept {
        if constexpr(register_count_ == 16U) return 0.673L;
        if constexpr(register_count_ == 32U) return 0.697L;
        if constexpr(register_count_ == 64U) return 0.709L;
        return 0.7213L / (1.0L + 1.079L / static_cast<long double>(register_count_));
    }

public:
    explicit HyperLogLog(std::uint64_t seed = 0) noexcept: seed_(seed) {}

    void clear() noexcept { registers_.fill(0); }

    void add(const Key& key) {
        const std::uint64_t hashed = approximate_streaming_detail::key_hash(key, seed_);
        const std::size_t index = static_cast<std::size_t>(
            hashed & (static_cast<std::uint64_t>(register_count_) - 1ULL));
        const std::uint64_t remaining = hashed >> Precision;
        unsigned rank;
        if(remaining == 0ULL){
            rank = 65U - Precision;
        }else{
            rank = static_cast<unsigned>(__builtin_clzll(remaining)) - Precision + 1U;
        }
        if(registers_[index] < rank) registers_[index] = static_cast<std::uint8_t>(rank);
    }

    [[nodiscard]] long double estimate() const noexcept {
        long double inverse_sum = 0.0L;
        std::size_t zero_count = 0;
        for(const std::uint8_t value: registers_){
            inverse_sum += std::ldexp(1.0L, -static_cast<int>(value));
            if(value == 0U) ++zero_count;
        }
        const long double m = static_cast<long double>(register_count_);
        long double result = alpha() * m * m / inverse_sum;
        if(result <= 2.5L * m && zero_count != 0U){
            result = m * std::log(m / static_cast<long double>(zero_count));
        }
        const long double hash_space = std::ldexp(1.0L, 64);
        if(result > hash_space / 30.0L){
            result = result < hash_space
                ? -hash_space * std::log1p(-result / hash_space)
                : std::numeric_limits<long double>::infinity();
        }
        return result;
    }

    void merge(const HyperLogLog& other) {
        if(seed_ != other.seed_) throw std::invalid_argument("cannot merge different seeds");
        for(std::size_t i = 0; i < register_count_; ++i){
            if(registers_[i] < other.registers_[i]) registers_[i] = other.registers_[i];
        }
    }

    [[nodiscard]] std::uint64_t seed() const noexcept { return seed_; }
    [[nodiscard]] static constexpr std::size_t register_count() noexcept {
        return register_count_;
    }
    [[nodiscard]] static long double relative_standard_error() noexcept {
        return 1.04L / std::sqrt(static_cast<long double>(register_count_));
    }
};

#endif  // CPPLIB_SRC_APPROXIMATE_STREAMING_HYPERLOGLOG_HPP_INCLUDED
