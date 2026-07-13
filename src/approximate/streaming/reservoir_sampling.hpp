#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, std::size_t Capacity>
class ReservoirSampler {
    static_assert(Capacity > 0, "Capacity must be positive");

    std::array<T, Capacity> samples_{};
    std::size_t size_ = 0;
    std::uint64_t seen_ = 0;
    std::mt19937_64 engine_;

public:
    explicit ReservoirSampler(std::uint64_t seed = 0): engine_(seed) {}

    void clear(std::uint64_t seed = 0) {
        size_ = 0;
        seen_ = 0;
        engine_.seed(seed);
    }

    void add(const T& value) {
        if(seen_ == std::numeric_limits<std::uint64_t>::max()){
            throw std::overflow_error("stream length overflow");
        }
        ++seen_;
        if(size_ < Capacity){
            samples_[size_++] = value;
            return;
        }
        std::uniform_int_distribution<std::uint64_t> distribution(0, seen_ - 1ULL);
        const std::uint64_t position = distribution(engine_);
        if(position < Capacity) samples_[static_cast<std::size_t>(position)] = value;
    }

    [[nodiscard]] std::vector<T> samples() const {
        return std::vector<T>(samples_.begin(), samples_.begin() + static_cast<std::ptrdiff_t>(size_));
    }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] std::uint64_t seen() const noexcept { return seen_; }
    [[nodiscard]] static constexpr std::size_t capacity() noexcept { return Capacity; }
};

template<class T, std::size_t Capacity>
class WeightedReservoirSampler {
    static_assert(Capacity > 0, "Capacity must be positive");

    struct Entry {
        long double priority = 0.0L;
        T value{};
    };

    std::array<Entry, Capacity> heap_{};
    std::size_t size_ = 0;
    std::uint64_t seen_ = 0;
    std::mt19937_64 engine_;

    void sift_up(std::size_t index) {
        while(index != 0U){
            const std::size_t parent = (index - 1U) >> 1U;
            if(heap_[parent].priority <= heap_[index].priority) break;
            std::swap(heap_[parent], heap_[index]);
            index = parent;
        }
    }

    void sift_down(std::size_t index) {
        while(true){
            const std::size_t left = index * 2U + 1U;
            if(left >= size_) return;
            const std::size_t right = left + 1U;
            std::size_t child = left;
            if(right < size_ && heap_[right].priority < heap_[left].priority) child = right;
            if(heap_[index].priority <= heap_[child].priority) return;
            std::swap(heap_[index], heap_[child]);
            index = child;
        }
    }

public:
    explicit WeightedReservoirSampler(std::uint64_t seed = 0): engine_(seed) {}

    void clear(std::uint64_t seed = 0) {
        size_ = 0;
        seen_ = 0;
        engine_.seed(seed);
    }

    void add(const T& value, long double weight) {
        if(!(weight > 0.0L) || !std::isfinite(weight)){
            throw std::invalid_argument("weight must be finite and positive");
        }
        if(seen_ == std::numeric_limits<std::uint64_t>::max()) throw std::overflow_error("stream length overflow");
        ++seen_;
        std::uniform_real_distribution<long double> distribution(0.0L, 1.0L);
        long double uniform = distribution(engine_);
        while(uniform == 0.0L) uniform = distribution(engine_);
        const long double priority = std::log(uniform) / weight;
        if(size_ < Capacity){
            heap_[size_] = Entry{priority, value};
            sift_up(size_++);
        }else if(heap_[0].priority < priority){
            heap_[0] = Entry{priority, value};
            sift_down(0);
        }
    }

    [[nodiscard]] std::vector<T> samples() const {
        std::vector<T> result;
        result.reserve(size_);
        for(std::size_t i = 0; i < size_; ++i) result.push_back(heap_[i].value);
        return result;
    }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] std::uint64_t seen() const noexcept { return seen_; }
    [[nodiscard]] static constexpr std::size_t capacity() noexcept { return Capacity; }
};
