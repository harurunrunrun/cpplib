#ifndef CPPLIB_SRC_APPROXIMATE_STREAMING_LINEAR_COUNTING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_STREAMING_LINEAR_COUNTING_HPP_INCLUDED

#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

#include "hash.hpp"

template<std::size_t BitCount = static_cast<std::size_t>(-1), class Key = std::uint64_t>
class LinearCounting {
    static_assert(BitCount > 0, "BitCount must be positive");
    static constexpr std::size_t word_count_ = (BitCount + 63U) / 64U;

    std::array<std::uint64_t, word_count_> bits_{};
    std::size_t occupied_ = 0;
    std::uint64_t seed_;

public:
    explicit LinearCounting(std::uint64_t seed = 0) noexcept: seed_(seed) {}

    void clear() noexcept {
        bits_.fill(0);
        occupied_ = 0;
    }

    void add(const Key& key) {
        const std::size_t index = static_cast<std::size_t>(
            approximate_streaming_detail::key_hash(key, seed_) % BitCount);
        const std::uint64_t mask = 1ULL << (index & 63U);
        std::uint64_t& word = bits_[index >> 6U];
        if((word & mask) == 0ULL){
            word |= mask;
            ++occupied_;
        }
    }

    [[nodiscard]] long double estimate() const noexcept {
        if(occupied_ == BitCount) return std::numeric_limits<long double>::infinity();
        const long double m = static_cast<long double>(BitCount);
        const long double empty = static_cast<long double>(BitCount - occupied_);
        return -m * std::log(empty / m);
    }

    void merge(const LinearCounting& other) {
        if(seed_ != other.seed_) throw std::invalid_argument("cannot merge different seeds");
        occupied_ = 0;
        for(std::size_t i = 0; i < word_count_; ++i){
            bits_[i] |= other.bits_[i];
            occupied_ += static_cast<std::size_t>(__builtin_popcountll(bits_[i]));
        }
        if constexpr((BitCount & 63U) != 0U){
            const std::size_t extra = 64U - (BitCount & 63U);
            occupied_ -= static_cast<std::size_t>(
                __builtin_popcountll(bits_.back() >> (64U - extra)));
        }
    }

    [[nodiscard]] std::uint64_t seed() const noexcept { return seed_; }
    [[nodiscard]] std::size_t occupied_bits() const noexcept { return occupied_; }
    [[nodiscard]] static constexpr std::size_t bit_count() noexcept { return BitCount; }
};


template<class Key>
class LinearCounting<static_cast<std::size_t>(-1), Key> {
    std::size_t bit_count_;


    std::size_t word_count_ = (bit_count_ + 63U) / 64U;

    std::vector<std::uint64_t> bits_{};
    std::size_t occupied_ = 0;
    std::uint64_t seed_;

public:
    explicit LinearCounting(std::size_t bit_count, std::uint64_t seed = 0) : bit_count_(bit_count),  seed_(seed) {
        if(bit_count_ == 0) throw std::invalid_argument("dimension must be positive");
        if(bit_count_ > std::numeric_limits<std::size_t>::max() - 63) throw std::length_error("bit count overflow");
        bits_.resize(word_count_);
}

    void clear() noexcept {
        std::fill(bits_.begin(), bits_.end(), 0);
        occupied_ = 0;
    }

    void add(const Key& key) {
        const std::size_t index = static_cast<std::size_t>(
            approximate_streaming_detail::key_hash(key, seed_) % bit_count_);
        const std::uint64_t mask = 1ULL << (index & 63U);
        std::uint64_t& word = bits_[index >> 6U];
        if((word & mask) == 0ULL){
            word |= mask;
            ++occupied_;
        }
    }

    [[nodiscard]] long double estimate() const noexcept {
        if(occupied_ == bit_count_) return std::numeric_limits<long double>::infinity();
        const long double m = static_cast<long double>(bit_count_);
        const long double empty = static_cast<long double>(bit_count_ - occupied_);
        return -m * std::log(empty / m);
    }

    void merge(const LinearCounting& other) {
        if(bit_count_ != other.bit_count_ || seed_ != other.seed_) throw std::invalid_argument("cannot merge different seeds");
        occupied_ = 0;
        for(std::size_t i = 0; i < word_count_; ++i){
            bits_[i] |= other.bits_[i];
            occupied_ += static_cast<std::size_t>(__builtin_popcountll(bits_[i]));
        }
        if((bit_count_ & 63U) != 0U){
            const std::size_t extra = 64U - (bit_count_ & 63U);
            occupied_ -= static_cast<std::size_t>(
                __builtin_popcountll(bits_.back() >> (64U - extra)));
        }
    }

    [[nodiscard]] std::uint64_t seed() const noexcept { return seed_; }
    [[nodiscard]] std::size_t occupied_bits() const noexcept { return occupied_; }
    [[nodiscard]] std::size_t bit_count() const noexcept { return bit_count_; }
};
#endif  // CPPLIB_SRC_APPROXIMATE_STREAMING_LINEAR_COUNTING_HPP_INCLUDED
