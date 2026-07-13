#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

#include "hash.hpp"

template<std::size_t SignatureSize, class Key = std::uint64_t>
class MinHash {
    static_assert(SignatureSize > 0, "SignatureSize must be positive");

    std::array<std::uint64_t, SignatureSize> signature_{};
    std::uint64_t seed_;
    bool empty_ = true;

public:
    explicit MinHash(std::uint64_t seed = 0) noexcept: seed_(seed) { clear(); }

    void clear() noexcept {
        signature_.fill(std::numeric_limits<std::uint64_t>::max());
        empty_ = true;
    }

    void add(const Key& key) {
        const std::uint64_t base = approximate_streaming_detail::key_hash(key, seed_);
        for(std::size_t i = 0; i < SignatureSize; ++i){
            const std::uint64_t value = approximate_streaming_detail::splitmix64(
                base + approximate_streaming_detail::splitmix64(
                           seed_ + static_cast<std::uint64_t>(i)));
            if(value < signature_[i]) signature_[i] = value;
        }
        empty_ = false;
    }

    [[nodiscard]] long double similarity(const MinHash& other) const {
        if(seed_ != other.seed_) throw std::invalid_argument("cannot compare different seeds");
        if(empty_ && other.empty_) return 1.0L;
        if(empty_ || other.empty_) return 0.0L;
        std::size_t equal = 0;
        for(std::size_t i = 0; i < SignatureSize; ++i){
            if(signature_[i] == other.signature_[i]) ++equal;
        }
        return static_cast<long double>(equal) / static_cast<long double>(SignatureSize);
    }

    void merge(const MinHash& other) {
        if(seed_ != other.seed_) throw std::invalid_argument("cannot merge different seeds");
        for(std::size_t i = 0; i < SignatureSize; ++i){
            if(other.signature_[i] < signature_[i]) signature_[i] = other.signature_[i];
        }
        empty_ = empty_ && other.empty_;
    }

    [[nodiscard]] const std::array<std::uint64_t, SignatureSize>& signature() const noexcept {
        return signature_;
    }
    [[nodiscard]] std::uint64_t seed() const noexcept { return seed_; }
    [[nodiscard]] bool empty() const noexcept { return empty_; }
    [[nodiscard]] static constexpr std::size_t signature_size() noexcept {
        return SignatureSize;
    }
};
