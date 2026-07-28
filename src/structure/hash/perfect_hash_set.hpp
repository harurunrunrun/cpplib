#ifndef CPPLIB_SRC_STRUCTURE_HASH_PERFECT_HASH_SET_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_HASH_PERFECT_HASH_SET_HPP_INCLUDED

#include <bit>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

template<class Key, class Hash = std::hash<Key>,
         class Equal = std::equal_to<Key>>
class PerfectHashSet {
    struct MultiplyShift {
        std::uint64_t multiplier = 1;
        std::uint64_t addend = 0;
    };
    struct Bucket {
        MultiplyShift family;
        std::vector<std::optional<Key>> slots;
    };

    std::vector<Bucket> buckets_;
    std::size_t size_ = 0;
    MultiplyShift first_family_;
    std::uint64_t random_state_ = 0;
    Hash hash_;
    Equal equal_;

    static std::uint64_t mix(std::uint64_t value) noexcept {
        value ^= value >> 30;
        value *= 0xbf58476d1ce4e5b9ULL;
        value ^= value >> 27;
        value *= 0x94d049bb133111ebULL;
        return value ^ (value >> 31);
    }

    void reseed() {
        std::random_device device;
        std::uint64_t seed = static_cast<std::uint64_t>(device());
        seed ^= static_cast<std::uint64_t>(device()) << 32;
        random_state_ = mix(seed);
    }

    [[nodiscard]] std::uint64_t random_word() noexcept {
        random_state_ += 0x9e3779b97f4a7c15ULL;
        return mix(random_state_);
    }

    [[nodiscard]] MultiplyShift random_family() noexcept {
        return {random_word() | 1ULL, random_word()};
    }

    [[nodiscard]] static std::size_t next_power_of_two(
        std::size_t value
    ) {
        if (value <= 1) return 1;
        constexpr int digits = std::numeric_limits<std::size_t>::digits;
        const std::size_t highest = std::size_t{1} << (digits - 1);
        if (value > highest) {
            throw std::length_error("PerfectHashSet is too large");
        }
        return std::size_t{1} << std::bit_width(value - 1);
    }

    [[nodiscard]] static std::size_t index_with(
        std::uint64_t base_hash,
        const MultiplyShift& family,
        std::size_t table_size
    ) noexcept {
        if (table_size == 1) return 0;
        const int bits = static_cast<int>(std::bit_width(table_size) - 1);
        const std::uint64_t permuted =
            family.multiplier * base_hash + family.addend;
        return static_cast<std::size_t>(permuted >> (64 - bits));
    }

    [[nodiscard]] std::uint64_t base_hash(const Key& key) const {
        return static_cast<std::uint64_t>(hash_(key));
    }

    void make_unique(
        const std::vector<Key>& input,
        std::vector<Key>& keys,
        std::vector<std::uint64_t>& hashes
    ) {
        if (input.empty()) return;
        const std::size_t table_size = next_power_of_two(input.size());
        const MultiplyShift family = random_family();
        std::vector<std::vector<std::size_t>> groups(table_size);
        keys.reserve(input.size());
        hashes.reserve(input.size());
        for (const Key& key : input) {
            const std::uint64_t hash_value = base_hash(key);
            auto& group = groups[index_with(hash_value, family, table_size)];
            bool duplicate = false;
            for (const std::size_t index : group) {
                if (hashes[index] != hash_value) continue;
                if (!equal_(keys[index], key)) {
                    throw std::invalid_argument(
                        "PerfectHashSet requires a separating base Hash"
                    );
                }
                duplicate = true;
                break;
            }
            if (!duplicate) {
                group.push_back(keys.size());
                keys.push_back(key);
                hashes.push_back(hash_value);
            }
        }
    }

    [[nodiscard]] std::size_t second_level_size(
        std::size_t bucket_size
    ) const {
        if (bucket_size <= 1) return 1;
        constexpr std::size_t maximum =
            std::numeric_limits<std::size_t>::max();
        if (bucket_size > maximum / bucket_size) {
            throw std::length_error("PerfectHashSet bucket is too large");
        }
        const std::size_t square = bucket_size * bucket_size;
        if (square > maximum / 4) {
            throw std::length_error("PerfectHashSet bucket is too large");
        }
        return next_power_of_two(4 * square);
    }

public:
    PerfectHashSet() = default;
    explicit PerfectHashSet(const std::vector<Key>& keys) { build(keys); }

    void build(const std::vector<Key>& input) {
        reseed();
        std::vector<Key> keys;
        std::vector<std::uint64_t> hashes;
        make_unique(input, keys, hashes);
        const std::size_t new_size = keys.size();
        if (new_size == 0) {
            buckets_.clear();
            size_ = 0;
            return;
        }

        const std::size_t first_size = next_power_of_two(new_size);
        std::vector<std::vector<std::size_t>> groups;
        MultiplyShift first_family;
        constexpr std::size_t maximum =
            std::numeric_limits<std::size_t>::max();
        const std::size_t square_limit =
            new_size > maximum / 8 ? maximum : 8 * new_size;
        while (true) {
            const MultiplyShift candidate_family = random_family();
            std::vector<std::vector<std::size_t>> candidate(first_size);
            for (std::size_t index = 0; index < new_size; ++index) {
                candidate[index_with(
                    hashes[index], candidate_family, first_size
                )].push_back(index);
            }
            std::size_t square_sum = 0;
            bool excessive = false;
            for (const auto& group : candidate) {
                const std::size_t count = group.size();
                if (count == 0) continue;
                if (count > maximum / count) {
                    excessive = true;
                    break;
                }
                const std::size_t square = count * count;
                if (square > square_limit
                    || square_sum > square_limit - square) {
                    excessive = true;
                    break;
                }
                square_sum += square;
            }
            if (!excessive && square_sum <= square_limit) {
                first_family = candidate_family;
                groups = std::move(candidate);
                break;
            }
        }

        std::vector<Bucket> new_buckets(first_size);
        for (std::size_t i = 0; i < first_size; ++i) {
            const auto& group = groups[i];
            if (group.empty()) continue;
            Bucket& bucket = new_buckets[i];
            if (group.size() == 1) {
                bucket.slots.resize(1);
                bucket.slots[0] = keys[group[0]];
                continue;
            }
            const std::size_t table_size =
                second_level_size(group.size());
            while (true) {
                bucket.family = random_family();
                bucket.slots.assign(table_size, std::nullopt);
                bool collision = false;
                for (const std::size_t key_index : group) {
                    const std::size_t slot = index_with(
                        hashes[key_index], bucket.family, table_size
                    );
                    if (bucket.slots[slot].has_value()) {
                        collision = true;
                        break;
                    }
                    bucket.slots[slot] = keys[key_index];
                }
                if (!collision) break;
            }
        }
        buckets_ = std::move(new_buckets);
        first_family_ = first_family;
        size_ = new_size;
    }

    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }

    [[nodiscard]] bool contains(const Key& key) const {
        if (size_ == 0) return false;
        const std::uint64_t hash_value = base_hash(key);
        const Bucket& bucket = buckets_[
            index_with(hash_value, first_family_, buckets_.size())
        ];
        if (bucket.slots.empty()) return false;
        const auto& slot = bucket.slots[
            index_with(hash_value, bucket.family, bucket.slots.size())
        ];
        return slot.has_value() && equal_(*slot, key);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_HASH_PERFECT_HASH_SET_HPP_INCLUDED
