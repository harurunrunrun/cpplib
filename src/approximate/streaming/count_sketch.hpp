#ifndef CPPLIB_SRC_APPROXIMATE_STREAMING_COUNT_SKETCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_STREAMING_COUNT_SKETCH_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

#include "hash.hpp"

template<std::size_t Width, std::size_t Depth,
         class Counter = std::int64_t, class Key = std::uint64_t>
class CountSketch {
    static_assert(Width > 0, "Width must be positive");
    static_assert(Depth > 0, "Depth must be positive");
    static_assert(std::numeric_limits<Counter>::is_integer && std::numeric_limits<Counter>::is_signed,
                  "Counter must be a signed integer");

    std::array<Counter, Width * Depth> table_{};
    std::uint64_t seed_;

    [[nodiscard]] std::uint64_t hash(const Key& key, std::size_t row) const {
        return approximate_streaming_detail::key_hash(
            key, approximate_streaming_detail::splitmix64(
                     seed_ + static_cast<std::uint64_t>(row)));
    }

    [[nodiscard]] static Counter saturating_negate(Counter value) noexcept {
        if(value == std::numeric_limits<Counter>::min()){
            return std::numeric_limits<Counter>::max();
        }
        return static_cast<Counter>(-value);
    }

    [[nodiscard]] static Counter saturating_add(Counter first, Counter second) noexcept {
        constexpr Counter minimum = std::numeric_limits<Counter>::min();
        constexpr Counter maximum = std::numeric_limits<Counter>::max();
        if(second > Counter{} && first > static_cast<Counter>(maximum - second)) return maximum;
        if(second < Counter{} && first < static_cast<Counter>(minimum - second)) return minimum;
        return static_cast<Counter>(first + second);
    }

public:
    explicit CountSketch(std::uint64_t seed = 0) noexcept: seed_(seed) {}

    void clear() noexcept { table_.fill(Counter{}); }

    void add(const Key& key, Counter delta = Counter{1}) {
        for(std::size_t row = 0; row < Depth; ++row){
            const std::uint64_t hashed = hash(key, row);
            const std::size_t column = static_cast<std::size_t>(hashed % Width);
            const Counter signed_delta =
                (hashed >> 63U) == 0ULL ? delta : saturating_negate(delta);
            Counter& cell = table_[row * Width + column];
            cell = saturating_add(cell, signed_delta);
        }
    }

    [[nodiscard]] Counter estimate(const Key& key) const {
        std::array<Counter, Depth> estimates{};
        for(std::size_t row = 0; row < Depth; ++row){
            const std::uint64_t hashed = hash(key, row);
            const std::size_t column = static_cast<std::size_t>(hashed % Width);
            const Counter value = table_[row * Width + column];
            estimates[row] = (hashed >> 63U) == 0ULL ? value : saturating_negate(value);
        }
        const std::size_t middle = Depth / 2U;
        std::nth_element(estimates.begin(), estimates.begin() + static_cast<std::ptrdiff_t>(middle),
                         estimates.end());
        if constexpr((Depth & 1U) != 0U){
            return estimates[middle];
        }else{
            const Counter upper = estimates[middle];
            const Counter lower = *std::max_element(
                estimates.begin(), estimates.begin() + static_cast<std::ptrdiff_t>(middle));
            return static_cast<Counter>(lower / Counter{2} + upper / Counter{2}
                + (lower % Counter{2} + upper % Counter{2}) / Counter{2});
        }
    }

    [[nodiscard]] static constexpr std::size_t width() noexcept { return Width; }
    [[nodiscard]] static constexpr std::size_t depth() noexcept { return Depth; }
};

#endif  // CPPLIB_SRC_APPROXIMATE_STREAMING_COUNT_SKETCH_HPP_INCLUDED
