#ifndef CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_DETAIL_Y_FAST_TRIE_IMPLEMENTATION_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_DETAIL_Y_FAST_TRIE_IMPLEMENTATION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <set>
#include <unordered_map>
#include <utility>

#include "../x_fast_trie.hpp"

template<class UInt, int BITS>
class YFastTrieImplementation {
    static constexpr std::size_t TARGET =
        static_cast<std::size_t>(std::max(BITS, 2));
    XFastTrie<UInt, BITS> representatives_;
    std::unordered_map<UInt, std::set<UInt>> buckets_;
    std::size_t size_ = 0;

    [[nodiscard]] UInt bucket_for(UInt key) const {
        const auto lower = representatives_.lower_bound(key);
        return lower ? *lower : representatives_.maximum();
    }
    void add_bucket(std::set<UInt> values) {
        const UInt representative = *values.rbegin();
        representatives_.insert(representative);
        buckets_.emplace(representative, std::move(values));
    }
    void split_bucket(UInt representative) {
        auto handle = buckets_.extract(representative);
        std::set<UInt> values = std::move(handle.mapped());
        representatives_.erase(representative);
        const std::size_t middle = values.size() / 2;
        std::set<UInt> left, right;
        auto iterator = values.begin();
        for (std::size_t i = 0; i < middle; ++i, ++iterator) {
            left.insert(*iterator);
        }
        right.insert(iterator, values.end());
        add_bucket(std::move(left));
        add_bucket(std::move(right));
    }
    void rebalance_small(UInt representative) {
        if (buckets_.size() <= 1
            || buckets_.at(representative).size() >= TARGET / 2) return;
        auto neighbor = representatives_.successor(representative);
        if (!neighbor) neighbor = representatives_.predecessor(representative);
        if (!neighbor) return;
        const UInt other = *neighbor;
        auto first = buckets_.extract(representative);
        auto second = buckets_.extract(other);
        std::set<UInt> values = std::move(first.mapped());
        values.merge(second.mapped());
        representatives_.erase(representative);
        representatives_.erase(other);
        const UInt merged_representative = *values.rbegin();
        add_bucket(std::move(values));
        if (buckets_.at(merged_representative).size() > 2 * TARGET) {
            split_bucket(merged_representative);
        }
    }

public:
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool contains(UInt key) const {
        (void)representatives_.contains(key);
        return !empty() && buckets_.at(bucket_for(key)).contains(key);
    }
    bool insert(UInt key) {
        (void)representatives_.contains(key);
        if (empty()) {
            add_bucket(std::set<UInt>{key});
            size_ = 1;
            return true;
        }
        UInt representative = bucket_for(key);
        if (!buckets_.at(representative).insert(key).second) return false;
        ++size_;
        if (key > representative) {
            auto handle = buckets_.extract(representative);
            representatives_.erase(representative);
            representative = key;
            representatives_.insert(representative);
            handle.key() = representative;
            buckets_.insert(std::move(handle));
        }
        if (buckets_.at(representative).size() > 2 * TARGET) {
            split_bucket(representative);
        }
        return true;
    }
    bool erase(UInt key) {
        (void)representatives_.contains(key);
        if (empty()) return false;
        UInt representative = bucket_for(key);
        auto& bucket = buckets_.at(representative);
        if (bucket.erase(key) == 0) return false;
        --size_;
        if (bucket.empty()) {
            buckets_.erase(representative);
            representatives_.erase(representative);
            return true;
        }
        if (key == representative) {
            auto handle = buckets_.extract(representative);
            representatives_.erase(representative);
            representative = *handle.mapped().rbegin();
            representatives_.insert(representative);
            handle.key() = representative;
            buckets_.insert(std::move(handle));
        }
        rebalance_small(representative);
        return true;
    }
    [[nodiscard]] UInt minimum() const {
        const UInt representative = representatives_.minimum();
        return *buckets_.at(representative).begin();
    }
    [[nodiscard]] UInt maximum() const { return representatives_.maximum(); }
    [[nodiscard]] std::optional<UInt> lower_bound(UInt key) const {
        (void)representatives_.contains(key);
        if (empty()) return std::nullopt;
        const UInt representative = bucket_for(key);
        const auto& bucket = buckets_.at(representative);
        const auto iterator = bucket.lower_bound(key);
        if (iterator != bucket.end()) return *iterator;
        const auto next = representatives_.successor(representative);
        return next
            ? std::optional<UInt>(*buckets_.at(*next).begin())
            : std::nullopt;
    }
    [[nodiscard]] std::optional<UInt> predecessor(UInt key) const {
        (void)representatives_.contains(key);
        if (empty()) return std::nullopt;
        const UInt representative = bucket_for(key);
        const auto& bucket = buckets_.at(representative);
        auto iterator = bucket.lower_bound(key);
        if (iterator != bucket.begin()) return *--iterator;
        const auto previous = representatives_.predecessor(representative);
        return previous ? std::optional<UInt>(*previous) : std::nullopt;
    }
    [[nodiscard]] std::optional<UInt> successor(UInt key) const {
        (void)representatives_.contains(key);
        if (empty()) return std::nullopt;
        const UInt representative = bucket_for(key);
        const auto& bucket = buckets_.at(representative);
        const auto iterator = bucket.upper_bound(key);
        if (iterator != bucket.end()) return *iterator;
        const auto next = representatives_.successor(representative);
        return next
            ? std::optional<UInt>(*buckets_.at(*next).begin())
            : std::nullopt;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_DETAIL_Y_FAST_TRIE_IMPLEMENTATION_HPP_INCLUDED
