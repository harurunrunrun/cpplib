#ifndef CPPLIB_SRC_STRUCTURE_HEAP_PRIORITY_QUEUE_RADIX_HEAP_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_HEAP_PRIORITY_QUEUE_RADIX_HEAP_HPP_INCLUDED

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<class Key, class Value>
class RadixHeap {
    static_assert(std::is_unsigned_v<Key>);
    static constexpr int BITS = std::numeric_limits<Key>::digits;
    std::array<std::vector<std::pair<Key, Value>>, BITS + 1> buckets_;
    Key last_ = 0;
    std::size_t size_ = 0;

    static int bucket_index(Key key, Key last) {
        if (key == last) return 0;
        return BITS - static_cast<int>(std::countl_zero(key ^ last));
    }
    void pull() {
        int index = 1;
        while (buckets_[index].empty()) ++index;
        Key next = std::numeric_limits<Key>::max();
        for (const auto& entry : buckets_[index]) {
            if (entry.first < next) next = entry.first;
        }
        last_ = next;
        auto entries = std::move(buckets_[index]);
        buckets_[index].clear();
        for (auto& entry : entries) {
            buckets_[bucket_index(entry.first, last_)].push_back(
                std::move(entry)
            );
        }
    }

public:
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] Key last_popped_key() const noexcept { return last_; }
    void push(Key key, const Value& value) {
        if (key < last_) throw std::invalid_argument("RadixHeap key decreased");
        buckets_[bucket_index(key, last_)].emplace_back(key, value);
        ++size_;
    }
    void push(Key key, Value&& value) {
        if (key < last_) throw std::invalid_argument("RadixHeap key decreased");
        buckets_[bucket_index(key, last_)].emplace_back(key, std::move(value));
        ++size_;
    }
    const std::pair<Key, Value>& top() {
        if (empty()) throw std::out_of_range("RadixHeap::top on empty heap");
        if (buckets_[0].empty()) pull();
        return buckets_[0].back();
    }
    std::pair<Key, Value> pop() {
        if (empty()) throw std::out_of_range("RadixHeap::pop on empty heap");
        if (buckets_[0].empty()) pull();
        auto result = std::move(buckets_[0].back());
        buckets_[0].pop_back();
        --size_;
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_HEAP_PRIORITY_QUEUE_RADIX_HEAP_HPP_INCLUDED
