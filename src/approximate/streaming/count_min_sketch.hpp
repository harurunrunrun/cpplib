#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

#include "hash.hpp"

template<std::size_t Width, std::size_t Depth,
         class Counter = std::uint64_t, class Key = std::uint64_t>
class CountMinSketch {
    static_assert(Width > 0, "Width must be positive");
    static_assert(Depth > 0, "Depth must be positive");
    static_assert(std::numeric_limits<Counter>::is_integer && !std::numeric_limits<Counter>::is_signed,
                  "Counter must be an unsigned integer");

    std::array<Counter, Width * Depth> table_{};
    std::uint64_t seed_;
    Counter total_{};

    [[nodiscard]] std::size_t index(const Key& key, std::size_t row) const {
        const std::uint64_t salt = approximate_streaming_detail::splitmix64(
            seed_ + static_cast<std::uint64_t>(row));
        return row * Width + static_cast<std::size_t>(
            approximate_streaming_detail::key_hash(key, salt) % Width);
    }

public:
    explicit CountMinSketch(std::uint64_t seed = 0) noexcept: seed_(seed) {}

    void clear() noexcept {
        table_.fill(Counter{});
        total_ = Counter{};
    }

    void add(const Key& key, Counter delta = Counter{1}) {
        constexpr Counter maximum = std::numeric_limits<Counter>::max();
        for(std::size_t row = 0; row < Depth; ++row){
            Counter& cell = table_[index(key, row)];
            cell = delta > static_cast<Counter>(maximum - cell) ? maximum
                                                                 : static_cast<Counter>(cell + delta);
        }
        total_ = delta > static_cast<Counter>(maximum - total_) ? maximum
                                                                : static_cast<Counter>(total_ + delta);
    }

    [[nodiscard]] Counter estimate(const Key& key) const {
        Counter result = std::numeric_limits<Counter>::max();
        for(std::size_t row = 0; row < Depth; ++row){
            const Counter value = table_[index(key, row)];
            if(value < result) result = value;
        }
        return result;
    }

    [[nodiscard]] Counter total() const noexcept { return total_; }
    [[nodiscard]] static constexpr std::size_t width() noexcept { return Width; }
    [[nodiscard]] static constexpr std::size_t depth() noexcept { return Depth; }
    [[nodiscard]] static long double additive_error_rate() noexcept {
        return std::exp(1.0L) / static_cast<long double>(Width);
    }
    [[nodiscard]] static long double failure_probability_bound() noexcept {
        return std::exp(-static_cast<long double>(Depth));
    }
};
