#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

#include "hash.hpp"

template<std::size_t BitCount, class Key = std::uint64_t>
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
