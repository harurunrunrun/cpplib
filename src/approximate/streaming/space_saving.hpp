#ifndef CPPLIB_SRC_APPROXIMATE_STREAMING_SPACE_SAVING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_STREAMING_SPACE_SAVING_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

template<class Key, std::size_t Capacity, class Count = std::uint64_t>
class SpaceSaving {
    static_assert(Capacity > 0, "Capacity must be positive");
    static_assert(std::numeric_limits<Count>::is_integer
                  && !std::numeric_limits<Count>::is_signed
                  && !std::is_same<Count, bool>::value,
                  "Count must be an unsigned integer other than bool");


public:
    struct Candidate {
        Key key{};
        Count estimate{};
        Count error{};
    };

private:
    std::array<Candidate, Capacity> slots_{};
    std::size_t size_ = 0;
    Count total_{};

public:
    void clear() noexcept {
        slots_.fill(Candidate{});
        size_ = 0;
        total_ = Count{};
    }

    void add(const Key& key, Count delta = Count{1}) {
        if(delta == Count{}) return;
        if(delta > static_cast<Count>(std::numeric_limits<Count>::max() - total_)){
            throw std::overflow_error("stream weight overflow");
        }
        total_ = static_cast<Count>(total_ + delta);
        for(std::size_t i = 0; i < size_; ++i){
            if(slots_[i].key == key){
                slots_[i].estimate = static_cast<Count>(slots_[i].estimate + delta);
                return;
            }
        }
        if(size_ < Capacity){
            slots_[size_++] = Candidate{key, delta, Count{}};
            return;
        }
        std::size_t minimum = 0;
        for(std::size_t i = 1; i < Capacity; ++i){
            if(slots_[i].estimate < slots_[minimum].estimate) minimum = i;
        }
        const Count old_estimate = slots_[minimum].estimate;
        slots_[minimum] = Candidate{
            key, static_cast<Count>(old_estimate + delta), old_estimate};
    }

    [[nodiscard]] Candidate estimate(const Key& key) const {
        for(std::size_t i = 0; i < size_; ++i){
            if(slots_[i].key == key) return slots_[i];
        }
        return Candidate{key, Count{}, Count{}};
    }

    [[nodiscard]] std::vector<Candidate> candidates() const {
        std::vector<Candidate> result(slots_.begin(),
                                      slots_.begin() + static_cast<std::ptrdiff_t>(size_));
        std::sort(result.begin(), result.end(), [](const Candidate& first, const Candidate& second) {
            return first.estimate > second.estimate;
        });
        return result;
    }

    [[nodiscard]] Count total() const noexcept { return total_; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] static constexpr std::size_t capacity() noexcept { return Capacity; }
};

#endif  // CPPLIB_SRC_APPROXIMATE_STREAMING_SPACE_SAVING_HPP_INCLUDED
