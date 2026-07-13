#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

#include "hash.hpp"

template<std::size_t Groups, std::size_t SamplesPerGroup, class Key = std::uint64_t>
class AMSSketch {
    static_assert(Groups > 0, "Groups must be positive");
    static_assert(SamplesPerGroup > 0, "SamplesPerGroup must be positive");

    static constexpr std::size_t sample_count_ = Groups * SamplesPerGroup;
    std::array<long double, sample_count_> sums_{};
    std::array<std::array<std::uint64_t, 4>, sample_count_> coefficients_{};

    [[nodiscard]] static std::uint64_t multiply(std::uint64_t first,
                                                std::uint64_t second) noexcept {
        std::uint64_t result = 0;
        while(second != 0ULL){
            if((second & 1ULL) != 0ULL) result ^= first;
            second >>= 1U;
            const bool carry = (first >> 63U) != 0ULL;
            first <<= 1U;
            if(carry) first ^= 0x1bULL;
        }
        return result;
    }

    [[nodiscard]] static std::uint64_t evaluate(
        const std::array<std::uint64_t, 4>& coefficients, std::uint64_t value) noexcept {
        std::uint64_t result = coefficients[3];
        result = multiply(result, value) ^ coefficients[2];
        result = multiply(result, value) ^ coefficients[1];
        return multiply(result, value) ^ coefficients[0];
    }

public:
    explicit AMSSketch(std::uint64_t seed = 0) noexcept {
        std::uint64_t state = seed;
        for(auto& coefficients: coefficients_){
            for(auto& coefficient: coefficients){
                state = approximate_streaming_detail::splitmix64(state);
                coefficient = state;
            }
        }
    }

    void clear() noexcept { sums_.fill(0.0L); }

    void add(const Key& key, long double delta = 1.0L) {
        const std::uint64_t value = static_cast<std::uint64_t>(std::hash<Key>{}(key));
        for(std::size_t sample = 0; sample < sample_count_; ++sample){
            sums_[sample] += (evaluate(coefficients_[sample], value) & 1ULL) == 0ULL
                                 ? delta
                                 : -delta;
        }
    }

    [[nodiscard]] long double estimate_second_moment() const {
        std::array<long double, Groups> estimates{};
        for(std::size_t group = 0; group < Groups; ++group){
            long double sum = 0.0L;
            for(std::size_t sample = 0; sample < SamplesPerGroup; ++sample){
                const long double value = sums_[group * SamplesPerGroup + sample];
                sum += value * value;
            }
            estimates[group] = sum / static_cast<long double>(SamplesPerGroup);
        }
        const std::size_t middle = Groups / 2U;
        std::nth_element(estimates.begin(), estimates.begin() + static_cast<std::ptrdiff_t>(middle),
                         estimates.end());
        if constexpr((Groups & 1U) != 0U) return estimates[middle];
        const long double upper = estimates[middle];
        const long double lower = *std::max_element(
            estimates.begin(), estimates.begin() + static_cast<std::ptrdiff_t>(middle));
        return (lower + upper) / 2.0L;
    }

    [[nodiscard]] static constexpr std::size_t groups() noexcept { return Groups; }
    [[nodiscard]] static constexpr std::size_t samples_per_group() noexcept {
        return SamplesPerGroup;
    }
};
