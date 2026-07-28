#ifndef CPPLIB_SRC_STRUCTURE_HASH_PERFECT_HASH_SET_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_HASH_PERFECT_HASH_SET_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <stdexcept>
#include <utility>
#include <unordered_set>
#include <vector>

template<class Key, class Hash = std::hash<Key>,
         class Equal = std::equal_to<Key>>
class PerfectHashSet {
    struct Bucket {
        std::uint64_t seed = 0;
        std::vector<Key> fallback;
        std::vector<std::optional<Key>> slots;
    };
    std::vector<Bucket> buckets_;
    std::size_t size_ = 0;
    std::uint64_t first_seed_ = 1;
    Hash hash_;
    Equal equal_;

    static std::uint64_t mix(std::uint64_t value) {
        value ^= value >> 30;
        value *= 0xbf58476d1ce4e5b9ULL;
        value ^= value >> 27;
        value *= 0x94d049bb133111ebULL;
        return value ^ (value >> 31);
    }
    std::size_t hash_with(
        const Key& key, std::uint64_t seed, std::size_t modulus
    ) const {
        return modulus == 0 ? 0 : static_cast<std::size_t>(
            mix(static_cast<std::uint64_t>(hash_(key)) + seed) % modulus
        );
    }

public:
    PerfectHashSet() = default;
    explicit PerfectHashSet(const std::vector<Key>& keys) { build(keys); }
    void build(const std::vector<Key>& input) {
        std::unordered_set<Key, Hash, Equal> unique;
        unique.reserve(input.size());
        for (const Key& key : input) {
            unique.insert(key);
        }
        std::vector<Key> keys(unique.begin(), unique.end());
        size_ = keys.size();
        if (size_ == 0) {
            buckets_.clear();
            return;
        }
        std::vector<std::vector<Key>> groups(size_);
        for (int attempt = 0; ; ++attempt) {
            for (auto& group : groups) group.clear();
            for (const Key& key : keys) {
                groups[hash_with(key, first_seed_, size_)].push_back(key);
            }
            std::size_t square_sum = 0;
            for (const auto& group : groups) {
                square_sum += group.size() * group.size();
            }
            if (square_sum <= 4 * size_ || attempt == 63) break;
            first_seed_ = mix(first_seed_ + 1);
        }
        buckets_.assign(size_, Bucket{});
        for (std::size_t i = 0; i < size_; ++i) {
            const std::size_t square = groups[i].size() * groups[i].size();
            if (square == 0) continue;
            std::unordered_set<std::size_t> base_hashes;
            Bucket& bucket = buckets_[i];
            base_hashes.reserve(groups[i].size());
            bool indistinguishable = false;
            for (const Key& key : groups[i]) {
                if (!base_hashes.insert(hash_(key)).second) {
                    indistinguishable = true;
                    break;
                }
            }
            if (indistinguishable) {
                bucket.fallback = groups[i];
                continue;
            }
            bucket.seed = mix(first_seed_ + i + 1);
            bool built = false;
            for (int attempt = 0; attempt < 64; ++attempt) {
                bucket.slots.assign(square, std::nullopt);
                bool collision = false;
                for (const Key& key : groups[i]) {
                    const std::size_t index =
                        hash_with(key, bucket.seed, square);
                    if (bucket.slots[index]) {
                        collision = true;
                        break;
                    }
                    bucket.slots[index] = key;
                }
                if (!collision) {
                    built = true;
                    break;
                }
                bucket.seed = mix(bucket.seed + 1);
            }
            if (!built) {
                bucket.slots.clear();
                bucket.fallback = groups[i];
            }
        }
    }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool contains(const Key& key) const {
        if (size_ == 0) return false;
        const Bucket& bucket =
            buckets_[hash_with(key, first_seed_, size_)];
        if (!bucket.fallback.empty()) {
            for (const Key& candidate : bucket.fallback) {
                if (equal_(candidate, key)) return true;
            }
            return false;
        }
        if (bucket.slots.empty()) return false;
        const auto& slot = bucket.slots[
            hash_with(key, bucket.seed, bucket.slots.size())
        ];
        return slot && equal_(*slot, key);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_HASH_PERFECT_HASH_SET_HPP_INCLUDED
