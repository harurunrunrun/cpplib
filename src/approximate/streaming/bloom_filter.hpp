#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

#include "hash.hpp"

template<std::size_t BitCount, std::size_t HashCount, class Key = std::uint64_t>
class BloomFilter {
    static_assert(BitCount > 0, "BitCount must be positive");
    static_assert(HashCount > 0, "HashCount must be positive");

    static constexpr std::size_t word_count_ = (BitCount + 63U) / 64U;
    std::array<std::uint64_t, word_count_> bits_{};
    std::uint64_t seed_;
    std::uint64_t insertions_ = 0;

    template<class Callback>
    void for_each_index(const Key& key, Callback callback) const {
        const std::uint64_t first = approximate_streaming_detail::key_hash(key, seed_);
        const std::uint64_t second =
            approximate_streaming_detail::splitmix64(first ^ 0xd6e8feb86659fd93ULL) | 1ULL;
        for(std::size_t i = 0; i < HashCount; ++i){
            const std::uint64_t mixed = first + static_cast<std::uint64_t>(i) * second;
            callback(static_cast<std::size_t>(mixed % BitCount));
        }
    }

public:
    explicit BloomFilter(std::uint64_t seed = 0) noexcept: seed_(seed) {}

    void clear() noexcept {
        bits_.fill(0);
        insertions_ = 0;
    }

    void add(const Key& key) noexcept(noexcept(approximate_streaming_detail::key_hash(key, seed_))) {
        for_each_index(key, [&](std::size_t index) {
            bits_[index >> 6U] |= 1ULL << (index & 63U);
        });
        ++insertions_;
    }

    [[nodiscard]] bool contains(const Key& key) const
        noexcept(noexcept(approximate_streaming_detail::key_hash(key, seed_))) {
        bool result = true;
        for_each_index(key, [&](std::size_t index) {
            result = result && ((bits_[index >> 6U] >> (index & 63U)) & 1ULL) != 0ULL;
        });
        return result;
    }

    [[nodiscard]] std::uint64_t insertions() const noexcept { return insertions_; }
    [[nodiscard]] static constexpr std::size_t bit_count() noexcept { return BitCount; }
    [[nodiscard]] static constexpr std::size_t hash_count() noexcept { return HashCount; }

    [[nodiscard]] long double estimated_false_positive_probability() const noexcept {
        const long double m = static_cast<long double>(BitCount);
        const long double k = static_cast<long double>(HashCount);
        const long double n = static_cast<long double>(insertions_);
        return std::pow(1.0L - std::exp(-k * n / m), k);
    }
};

template<std::size_t CounterCount, std::size_t HashCount,
         class Counter = std::uint16_t, class Key = std::uint64_t>
class CountingBloomFilter {
    static_assert(CounterCount > 0, "CounterCount must be positive");
    static_assert(HashCount > 0, "HashCount must be positive");
    static_assert(std::numeric_limits<Counter>::is_integer && !std::numeric_limits<Counter>::is_signed,
                  "Counter must be an unsigned integer");

    std::array<Counter, CounterCount> counters_{};
    std::uint64_t seed_;
    std::uint64_t insertions_ = 0;

    template<class Callback>
    void for_each_index(const Key& key, Callback callback) const {
        const std::uint64_t first = approximate_streaming_detail::key_hash(key, seed_);
        const std::uint64_t second =
            approximate_streaming_detail::splitmix64(first ^ 0xd6e8feb86659fd93ULL) | 1ULL;
        for(std::size_t i = 0; i < HashCount; ++i){
            callback(static_cast<std::size_t>(
                (first + static_cast<std::uint64_t>(i) * second) % CounterCount));
        }
    }

public:
    explicit CountingBloomFilter(std::uint64_t seed = 0) noexcept: seed_(seed) {}

    void clear() noexcept {
        counters_.fill(Counter{});
        insertions_ = 0;
    }

    void add(const Key& key) {
        constexpr Counter maximum = std::numeric_limits<Counter>::max();
        for_each_index(key, [&](std::size_t index) {
            if(counters_[index] != maximum) ++counters_[index];
        });
        ++insertions_;
    }

    [[nodiscard]] bool remove(const Key& key) {
        if(!contains(key)) return false;
        for_each_index(key, [&](std::size_t index) { --counters_[index]; });
        if(insertions_ != 0) --insertions_;
        return true;
    }

    [[nodiscard]] bool contains(const Key& key) const {
        Counter result = std::numeric_limits<Counter>::max();
        for_each_index(key, [&](std::size_t index) {
            if(counters_[index] < result) result = counters_[index];
        });
        return result != Counter{};
    }

    [[nodiscard]] Counter estimate_multiplicity(const Key& key) const {
        Counter result = std::numeric_limits<Counter>::max();
        for_each_index(key, [&](std::size_t index) {
            if(counters_[index] < result) result = counters_[index];
        });
        return result;
    }

    [[nodiscard]] std::uint64_t insertions() const noexcept { return insertions_; }
    [[nodiscard]] static constexpr std::size_t counter_count() noexcept { return CounterCount; }
    [[nodiscard]] static constexpr std::size_t hash_count() noexcept { return HashCount; }
};
