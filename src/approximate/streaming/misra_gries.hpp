#ifndef CPPLIB_SRC_APPROXIMATE_STREAMING_MISRA_GRIES_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_STREAMING_MISRA_GRIES_HPP_INCLUDED

#include <array>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<class Key, std::size_t Capacity = static_cast<std::size_t>(-1), class Count = std::uint64_t>
class MisraGries {
    static_assert(Capacity > 0, "Capacity must be positive");
    static_assert(std::numeric_limits<Count>::is_integer
                  && !std::numeric_limits<Count>::is_signed
                  && !std::is_same<Count, bool>::value,
                  "Count must be an unsigned integer other than bool");

    [[nodiscard]] static constexpr bool capacity_plus_one_fits() noexcept {
        if constexpr(std::numeric_limits<Count>::digits
                     >= std::numeric_limits<std::size_t>::digits){
            return Capacity < std::numeric_limits<std::size_t>::max();
        }else{
            return Capacity
                < ((std::size_t{1} << std::numeric_limits<Count>::digits) - 1U);
        }
    }
    static_assert(capacity_plus_one_fits(), "Count must represent Capacity + 1");


    struct Slot {
        Key key{};
        Count count{};
        bool used = false;
    };

    std::array<Slot, Capacity> slots_{};
    Count total_{};

public:
    void clear() noexcept {
        for(auto& slot: slots_) slot = Slot{};
        total_ = Count{};
    }

    void add(const Key& key) {
        if(total_ == std::numeric_limits<Count>::max()){
            throw std::overflow_error("stream length overflow");
        }
        ++total_;
        for(auto& slot: slots_){
            if(slot.used && slot.key == key){
                ++slot.count;
                return;
            }
        }
        for(auto& slot: slots_){
            if(!slot.used){
                slot = Slot{key, Count{1}, true};
                return;
            }
        }
        for(auto& slot: slots_){
            --slot.count;
            if(slot.count == Count{}) slot.used = false;
        }
    }

    [[nodiscard]] Count estimate_lower_bound(const Key& key) const {
        for(const auto& slot: slots_){
            if(slot.used && slot.key == key) return slot.count;
        }
        return Count{};
    }

    [[nodiscard]] std::vector<std::pair<Key, Count>> candidates() const {
        std::vector<std::pair<Key, Count>> result;
        result.reserve(Capacity);
        for(const auto& slot: slots_){
            if(slot.used) result.emplace_back(slot.key, slot.count);
        }
        return result;
    }

    [[nodiscard]] Count total() const noexcept { return total_; }
    [[nodiscard]] Count maximum_underestimate() const noexcept {
        return total_ / static_cast<Count>(static_cast<Count>(Capacity) + Count{1});
    }
    [[nodiscard]] static constexpr std::size_t capacity() noexcept { return Capacity; }
};


template<class Key, class Count>
class MisraGries<Key, static_cast<std::size_t>(-1), Count> {
    std::size_t capacity_;


    static_assert(std::numeric_limits<Count>::is_integer
                  && !std::numeric_limits<Count>::is_signed
                  && !std::is_same<Count, bool>::value,
                  "Count must be an unsigned integer other than bool");

    [[nodiscard]] bool capacity_plus_one_fits() const noexcept {
        if constexpr(std::numeric_limits<Count>::digits
                     >= std::numeric_limits<std::size_t>::digits){
            return capacity_ < std::numeric_limits<std::size_t>::max();
        }else{
            return capacity_
                < ((std::size_t{1} << std::numeric_limits<Count>::digits) - 1U);
        }
    }



    struct Slot {
        Key key{};
        Count count{};
        bool used = false;
    };

    std::vector<Slot> slots_{};
    Count total_{};

public:
    explicit MisraGries(std::size_t capacity): capacity_(capacity){
        if(capacity_ == 0) throw std::invalid_argument("dimension must be positive");
        if(!capacity_plus_one_fits()) throw std::invalid_argument("counter cannot represent capacity + 1");
        slots_.resize(capacity_);
    }

    void clear() noexcept {
        for(auto& slot: slots_) slot = Slot{};
        total_ = Count{};
    }

    void add(const Key& key) {
        if(total_ == std::numeric_limits<Count>::max()){
            throw std::overflow_error("stream length overflow");
        }
        ++total_;
        for(auto& slot: slots_){
            if(slot.used && slot.key == key){
                ++slot.count;
                return;
            }
        }
        for(auto& slot: slots_){
            if(!slot.used){
                slot = Slot{key, Count{1}, true};
                return;
            }
        }
        for(auto& slot: slots_){
            --slot.count;
            if(slot.count == Count{}) slot.used = false;
        }
    }

    [[nodiscard]] Count estimate_lower_bound(const Key& key) const {
        for(const auto& slot: slots_){
            if(slot.used && slot.key == key) return slot.count;
        }
        return Count{};
    }

    [[nodiscard]] std::vector<std::pair<Key, Count>> candidates() const {
        std::vector<std::pair<Key, Count>> result;
        result.reserve(capacity_);
        for(const auto& slot: slots_){
            if(slot.used) result.emplace_back(slot.key, slot.count);
        }
        return result;
    }

    [[nodiscard]] Count total() const noexcept { return total_; }
    [[nodiscard]] Count maximum_underestimate() const noexcept {
        return total_ / static_cast<Count>(static_cast<Count>(capacity_) + Count{1});
    }
    [[nodiscard]] std::size_t capacity() const noexcept { return capacity_; }
};
#endif  // CPPLIB_SRC_APPROXIMATE_STREAMING_MISRA_GRIES_HPP_INCLUDED
