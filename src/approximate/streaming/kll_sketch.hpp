#ifndef CPPLIB_SRC_APPROXIMATE_STREAMING_KLL_SKETCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_STREAMING_KLL_SKETCH_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, std::size_t K = static_cast<std::size_t>(-1), std::size_t MaxLevels = static_cast<std::size_t>(-1),
         class Compare = std::less<T>>
class KLLSketch {
    static_assert(K >= 2 && (K & 1U) == 0U, "K must be positive and even");
    static_assert(MaxLevels > 0 && MaxLevels < 64, "MaxLevels must be in [1, 63]");

    std::array<std::array<T, K>, MaxLevels> levels_{};
    std::array<std::size_t, MaxLevels> sizes_{};
    std::uint64_t count_ = 0;
    std::mt19937_64 engine_;
    Compare compare_{};

    void insert_at(std::size_t level, const T& value) {
        if(level == MaxLevels) throw std::overflow_error("KLLSketch level capacity exceeded");
        levels_[level][sizes_[level]++] = value;
        if(sizes_[level] != K) return;

        auto& values = levels_[level];
        std::sort(values.begin(), values.end(), compare_);
        const std::size_t parity = static_cast<std::size_t>(engine_() & 1ULL);
        std::array<T, K / 2U> promoted{};
        for(std::size_t i = 0; i < K / 2U; ++i) promoted[i] = values[i * 2U + parity];
        sizes_[level] = 0;
        for(const T& promoted_value: promoted) insert_at(level + 1U, promoted_value);
    }

public:
    explicit KLLSketch(std::uint64_t seed = 0, Compare compare = Compare{})
        : engine_(seed), compare_(std::move(compare)) {}

    void clear(std::uint64_t seed = 0) noexcept {
        sizes_.fill(0);
        count_ = 0;
        engine_.seed(seed);
    }

    void add(const T& value) {
        if(count_ == std::numeric_limits<std::uint64_t>::max()){
            throw std::overflow_error("KLLSketch item count overflow");
        }
        insert_at(0, value);
        ++count_;
    }

    [[nodiscard]] T quantile(long double probability) const {
        if(count_ == 0) throw std::logic_error("quantile of an empty KLLSketch");
        if(!(0.0L <= probability && probability <= 1.0L)){
            throw std::out_of_range("probability must be in [0, 1]");
        }
        struct WeightedValue {
            T value;
            std::uint64_t weight;
        };
        std::vector<WeightedValue> values;
        std::size_t retained = 0;
        for(const std::size_t size: sizes_) retained += size;
        values.reserve(retained);
        for(std::size_t level = 0; level < MaxLevels; ++level){
            const std::uint64_t weight = 1ULL << level;
            for(std::size_t i = 0; i < sizes_[level]; ++i){
                values.push_back(WeightedValue{levels_[level][i], weight});
            }
        }
        std::sort(values.begin(), values.end(), [&](const WeightedValue& first,
                                                    const WeightedValue& second) {
            return compare_(first.value, second.value);
        });
        const std::uint64_t target = static_cast<std::uint64_t>(
            std::floor(probability * static_cast<long double>(count_ - 1ULL)));
        std::uint64_t prefix = 0;
        for(const auto& value: values){
            prefix += value.weight;
            if(target < prefix) return value.value;
        }
        return values.back().value;
    }

    [[nodiscard]] std::uint64_t size() const noexcept { return count_; }
    [[nodiscard]] std::size_t retained_items() const noexcept {
        std::size_t result = 0;
        for(const std::size_t size: sizes_) result += size;
        return result;
    }
    [[nodiscard]] static constexpr std::size_t capacity_per_level() noexcept { return K; }
    [[nodiscard]] static constexpr std::size_t maximum_levels() noexcept { return MaxLevels; }
    [[nodiscard]] static long double typical_rank_error_rate() noexcept {
        return 1.0L / std::sqrt(static_cast<long double>(K));
    }
};


// Preserve the former default level count for explicit fixed per-level capacities.
template<class T, std::size_t K, class Compare>
class KLLSketch<T, K, static_cast<std::size_t>(-1), Compare>
    : public KLLSketch<T, K, 32, Compare>{
    using Base = KLLSketch<T, K, 32, Compare>;
public:
    using Base::Base;
};

template<class T, class Compare>
class KLLSketch<T, static_cast<std::size_t>(-1), static_cast<std::size_t>(-1), Compare> {
    std::size_t capacity_;
    std::size_t maximum_levels_;




    std::vector<std::vector<T>> levels_{};
    std::vector<std::size_t> sizes_{};
    std::uint64_t count_ = 0;
    std::mt19937_64 engine_;
    Compare compare_{};

    void insert_at(std::size_t level, const T& value) {
        if(level == maximum_levels_) throw std::overflow_error("KLLSketch level capacity exceeded");
        levels_[level][sizes_[level]++] = value;
        if(sizes_[level] != capacity_) return;

        auto& values = levels_[level];
        std::sort(values.begin(), values.end(), compare_);
        const std::size_t parity = static_cast<std::size_t>(engine_() & 1ULL);
        std::vector<T> promoted(capacity_ / 2U);
        for(std::size_t i = 0; i < capacity_ / 2U; ++i) promoted[i] = values[i * 2U + parity];
        sizes_[level] = 0;
        for(const T& promoted_value: promoted) insert_at(level + 1U, promoted_value);
    }

public:
    explicit KLLSketch(std::size_t capacity = 200, std::size_t maximum_levels = 32, std::uint64_t seed = 0, Compare compare = Compare{})
        : capacity_(capacity), maximum_levels_(maximum_levels),  engine_(seed), compare_(std::move(compare)) {
        if(capacity_ == 0) throw std::invalid_argument("dimension must be positive");
        if(maximum_levels_ == 0) throw std::invalid_argument("dimension must be positive");
        if(capacity_ > std::numeric_limits<std::size_t>::max() / maximum_levels_) throw std::length_error("streaming table size overflow");
        if(capacity_ < 2 || (capacity_ & 1U) || maximum_levels_ >= 64) throw std::invalid_argument("KLL needs even capacity >= 2 and 1..63 levels");
        levels_.resize(maximum_levels_);
        for(auto& row: levels_) row.resize(capacity_);
        sizes_.resize(maximum_levels_);
}

    void clear(std::uint64_t seed = 0) noexcept {
        std::fill(sizes_.begin(), sizes_.end(), 0);
        count_ = 0;
        engine_.seed(seed);
    }

    void add(const T& value) {
        if(count_ == std::numeric_limits<std::uint64_t>::max()){
            throw std::overflow_error("KLLSketch item count overflow");
        }
        insert_at(0, value);
        ++count_;
    }

    [[nodiscard]] T quantile(long double probability) const {
        if(count_ == 0) throw std::logic_error("quantile of an empty KLLSketch");
        if(!(0.0L <= probability && probability <= 1.0L)){
            throw std::out_of_range("probability must be in [0, 1]");
        }
        struct WeightedValue {
            T value;
            std::uint64_t weight;
        };
        std::vector<WeightedValue> values;
        std::size_t retained = 0;
        for(const std::size_t size: sizes_) retained += size;
        values.reserve(retained);
        for(std::size_t level = 0; level < maximum_levels_; ++level){
            const std::uint64_t weight = 1ULL << level;
            for(std::size_t i = 0; i < sizes_[level]; ++i){
                values.push_back(WeightedValue{levels_[level][i], weight});
            }
        }
        std::sort(values.begin(), values.end(), [&](const WeightedValue& first,
                                                    const WeightedValue& second) {
            return compare_(first.value, second.value);
        });
        const std::uint64_t target = static_cast<std::uint64_t>(
            std::floor(probability * static_cast<long double>(count_ - 1ULL)));
        std::uint64_t prefix = 0;
        for(const auto& value: values){
            prefix += value.weight;
            if(target < prefix) return value.value;
        }
        return values.back().value;
    }

    [[nodiscard]] std::uint64_t size() const noexcept { return count_; }
    [[nodiscard]] std::size_t retained_items() const noexcept {
        std::size_t result = 0;
        for(const std::size_t size: sizes_) result += size;
        return result;
    }
    [[nodiscard]] std::size_t capacity_per_level() const noexcept { return capacity_; }
    [[nodiscard]] std::size_t maximum_levels() const noexcept { return maximum_levels_; }
    [[nodiscard]] long double typical_rank_error_rate() const noexcept {
        return 1.0L / std::sqrt(static_cast<long double>(capacity_));
    }
};
#endif  // CPPLIB_SRC_APPROXIMATE_STREAMING_KLL_SKETCH_HPP_INCLUDED
