#ifndef CPPLIB_SRC_APPROXIMATE_STREAMING_MORRIS_COUNTER_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_STREAMING_MORRIS_COUNTER_HPP_INCLUDED

#include <cmath>
#include <cstdint>
#include <limits>
#include <random>
#include <stdexcept>

class MorrisCounter {
    std::uint32_t exponent_ = 0;
    long double base_;
    std::mt19937_64 engine_;

public:
    explicit MorrisCounter(std::uint64_t seed = 0, long double base = 2.0L)
        : base_(base), engine_(seed) {
        if(!(base_ > 1.0L) || !std::isfinite(base_))
            throw std::invalid_argument("base must be finite and greater than one");
    }

    void clear(std::uint64_t seed = 0) noexcept {
        exponent_ = 0;
        engine_.seed(seed);
    }

    void increment() {
        const long double probability = std::pow(base_, -static_cast<long double>(exponent_));
        std::uniform_real_distribution<long double> distribution(0.0L, 1.0L);
        if(distribution(engine_) < probability && exponent_ != std::numeric_limits<std::uint32_t>::max()){
            ++exponent_;
        }
    }

    [[nodiscard]] long double estimate() const noexcept {
        return (std::pow(base_, static_cast<long double>(exponent_)) - 1.0L) / (base_ - 1.0L);
    }
    [[nodiscard]] std::uint32_t exponent() const noexcept { return exponent_; }
    [[nodiscard]] long double base() const noexcept { return base_; }
};

#endif  // CPPLIB_SRC_APPROXIMATE_STREAMING_MORRIS_COUNTER_HPP_INCLUDED
