#ifndef CPPLIB_SRC_APPROXIMATE_STREAMING_BLOOM_FILTER_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_STREAMING_BLOOM_FILTER_HPP_INCLUDED

#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

#include "hash.hpp"

template<std::size_t BitCount = static_cast<std::size_t>(-1), std::size_t HashCount = static_cast<std::size_t>(-1), class Key = std::uint64_t>
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

template<std::size_t CounterCount = static_cast<std::size_t>(-1), std::size_t HashCount = static_cast<std::size_t>(-1),
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


template<class Key>
class BloomFilter<static_cast<std::size_t>(-1), static_cast<std::size_t>(-1), Key> {
    std::size_t bit_count_;
    std::size_t hash_count_;




    std::size_t word_count_ = (bit_count_ + 63U) / 64U;
    std::vector<std::uint64_t> bits_{};
    std::uint64_t seed_;
    std::uint64_t insertions_ = 0;

    template<class Callback>
    void for_each_index(const Key& key, Callback callback) const {
        const std::uint64_t first = approximate_streaming_detail::key_hash(key, seed_);
        const std::uint64_t second =
            approximate_streaming_detail::splitmix64(first ^ 0xd6e8feb86659fd93ULL) | 1ULL;
        for(std::size_t i = 0; i < hash_count_; ++i){
            const std::uint64_t mixed = first + static_cast<std::uint64_t>(i) * second;
            callback(static_cast<std::size_t>(mixed % bit_count_));
        }
    }

public:
    explicit BloomFilter(std::size_t bit_count, std::size_t hash_count, std::uint64_t seed = 0) : bit_count_(bit_count), hash_count_(hash_count),  seed_(seed) {
        if(bit_count_ == 0) throw std::invalid_argument("dimension must be positive");
        if(hash_count_ == 0) throw std::invalid_argument("dimension must be positive");
        if(bit_count_ > std::numeric_limits<std::size_t>::max() / hash_count_) throw std::length_error("streaming table size overflow");
        if(bit_count_ > std::numeric_limits<std::size_t>::max() - 63) throw std::length_error("bit count overflow");
        bits_.resize(word_count_);
}

    void clear() noexcept {
        std::fill(bits_.begin(), bits_.end(), 0);
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
    [[nodiscard]] std::size_t bit_count() const noexcept { return bit_count_; }
    [[nodiscard]] std::size_t hash_count() const noexcept { return hash_count_; }

    [[nodiscard]] long double estimated_false_positive_probability() const noexcept {
        const long double m = static_cast<long double>(bit_count_);
        const long double k = static_cast<long double>(hash_count_);
        const long double n = static_cast<long double>(insertions_);
        return std::pow(1.0L - std::exp(-k * n / m), k);
    }
};

template<class Counter, class Key>
class CountingBloomFilter<static_cast<std::size_t>(-1), static_cast<std::size_t>(-1), Counter, Key> {
    std::size_t counter_count_;
    std::size_t hash_count_;



    static_assert(std::numeric_limits<Counter>::is_integer && !std::numeric_limits<Counter>::is_signed,
                  "Counter must be an unsigned integer");

    std::vector<Counter> counters_{};
    std::uint64_t seed_;
    std::uint64_t insertions_ = 0;

    template<class Callback>
    void for_each_index(const Key& key, Callback callback) const {
        const std::uint64_t first = approximate_streaming_detail::key_hash(key, seed_);
        const std::uint64_t second =
            approximate_streaming_detail::splitmix64(first ^ 0xd6e8feb86659fd93ULL) | 1ULL;
        for(std::size_t i = 0; i < hash_count_; ++i){
            callback(static_cast<std::size_t>(
                (first + static_cast<std::uint64_t>(i) * second) % counter_count_));
        }
    }

public:
    explicit CountingBloomFilter(std::size_t counter_count, std::size_t hash_count, std::uint64_t seed = 0) : counter_count_(counter_count), hash_count_(hash_count),  seed_(seed) {
        if(counter_count_ == 0) throw std::invalid_argument("dimension must be positive");
        if(hash_count_ == 0) throw std::invalid_argument("dimension must be positive");
        if(counter_count_ > std::numeric_limits<std::size_t>::max() / hash_count_) throw std::length_error("streaming table size overflow");
        counters_.resize(counter_count_);
}

    void clear() noexcept {
        std::fill(counters_.begin(), counters_.end(), Counter{});
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
    [[nodiscard]] std::size_t counter_count() const noexcept { return counter_count_; }
    [[nodiscard]] std::size_t hash_count() const noexcept { return hash_count_; }
};
#endif  // CPPLIB_SRC_APPROXIMATE_STREAMING_BLOOM_FILTER_HPP_INCLUDED
