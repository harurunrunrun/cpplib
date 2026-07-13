#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

#include "hash.hpp"

template<std::size_t Bits = 64, class Key = std::uint64_t>
class SimHash {
    static_assert(Bits > 0 && Bits <= 64, "Bits must be in [1, 64]");

    std::array<long double, Bits> accumulators_{};
    std::uint64_t seed_;

public:
    explicit SimHash(std::uint64_t seed = 0) noexcept: seed_(seed) {}

    void clear() noexcept { accumulators_.fill(0.0L); }

    void add(const Key& feature, long double weight = 1.0L) {
        const std::uint64_t hashed = approximate_streaming_detail::key_hash(feature, seed_);
        for(std::size_t bit = 0; bit < Bits; ++bit){
            accumulators_[bit] += ((hashed >> bit) & 1ULL) != 0ULL ? weight : -weight;
        }
    }

    [[nodiscard]] std::uint64_t signature() const noexcept {
        std::uint64_t result = 0;
        for(std::size_t bit = 0; bit < Bits; ++bit){
            if(accumulators_[bit] >= 0.0L) result |= 1ULL << bit;
        }
        return result;
    }

    [[nodiscard]] static unsigned hamming_distance(std::uint64_t first,
                                                   std::uint64_t second) noexcept {
        std::uint64_t difference = first ^ second;
        if constexpr(Bits < 64) difference &= (1ULL << Bits) - 1ULL;
        return static_cast<unsigned>(__builtin_popcountll(difference));
    }

    [[nodiscard]] long double similarity(const SimHash& other) const {
        if(seed_ != other.seed_) throw std::invalid_argument("cannot compare different seeds");
        return 1.0L - static_cast<long double>(hamming_distance(signature(), other.signature()))
                          / static_cast<long double>(Bits);
    }

    [[nodiscard]] std::uint64_t seed() const noexcept { return seed_; }
    [[nodiscard]] static constexpr std::size_t bit_count() noexcept { return Bits; }
};
