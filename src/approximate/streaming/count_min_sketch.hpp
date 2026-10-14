#ifndef CPPLIB_SRC_APPROXIMATE_STREAMING_COUNT_MIN_SKETCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_STREAMING_COUNT_MIN_SKETCH_HPP_INCLUDED

#include <array>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

#include "hash.hpp"

template<std::size_t Width = static_cast<std::size_t>(-1), std::size_t Depth = static_cast<std::size_t>(-1),
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


template<class Counter, class Key>
class CountMinSketch<static_cast<std::size_t>(-1), static_cast<std::size_t>(-1), Counter, Key> {
    std::size_t width_;
    std::size_t depth_;



    static_assert(std::numeric_limits<Counter>::is_integer && !std::numeric_limits<Counter>::is_signed,
                  "Counter must be an unsigned integer");

    std::vector<Counter> table_{};
    std::uint64_t seed_;
    Counter total_{};

    [[nodiscard]] std::size_t index(const Key& key, std::size_t row) const {
        const std::uint64_t salt = approximate_streaming_detail::splitmix64(
            seed_ + static_cast<std::uint64_t>(row));
        return row * width_ + static_cast<std::size_t>(
            approximate_streaming_detail::key_hash(key, salt) % width_);
    }

public:
    explicit CountMinSketch(std::size_t width, std::size_t depth, std::uint64_t seed = 0) : width_(width), depth_(depth),  seed_(seed) {
        if(width_ == 0) throw std::invalid_argument("dimension must be positive");
        if(depth_ == 0) throw std::invalid_argument("dimension must be positive");
        if(width_ > std::numeric_limits<std::size_t>::max() / depth_) throw std::length_error("streaming table size overflow");
        table_.resize(width_ * depth_);
}

    void clear() noexcept {
        std::fill(table_.begin(), table_.end(), Counter{});
        total_ = Counter{};
    }

    void add(const Key& key, Counter delta = Counter{1}) {
        constexpr Counter maximum = std::numeric_limits<Counter>::max();
        for(std::size_t row = 0; row < depth_; ++row){
            Counter& cell = table_[index(key, row)];
            cell = delta > static_cast<Counter>(maximum - cell) ? maximum
                                                                 : static_cast<Counter>(cell + delta);
        }
        total_ = delta > static_cast<Counter>(maximum - total_) ? maximum
                                                                : static_cast<Counter>(total_ + delta);
    }

    [[nodiscard]] Counter estimate(const Key& key) const {
        Counter result = std::numeric_limits<Counter>::max();
        for(std::size_t row = 0; row < depth_; ++row){
            const Counter value = table_[index(key, row)];
            if(value < result) result = value;
        }
        return result;
    }

    [[nodiscard]] Counter total() const noexcept { return total_; }
    [[nodiscard]] std::size_t width() const noexcept { return width_; }
    [[nodiscard]] std::size_t depth() const noexcept { return depth_; }
    [[nodiscard]] long double additive_error_rate() const noexcept {
        return std::exp(1.0L) / static_cast<long double>(width_);
    }
    [[nodiscard]] long double failure_probability_bound() const noexcept {
        return std::exp(-static_cast<long double>(depth_));
    }
};
#endif  // CPPLIB_SRC_APPROXIMATE_STREAMING_COUNT_MIN_SKETCH_HPP_INCLUDED
