#ifndef CPPLIB_SRC_APPROXIMATE_STREAMING_CUCKOO_FILTER_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_STREAMING_CUCKOO_FILTER_HPP_INCLUDED

#include <array>
#include <limits>
#include <vector>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>

#include "hash.hpp"

template<std::size_t BucketCount = static_cast<std::size_t>(-1), std::size_t SlotsPerBucket = static_cast<std::size_t>(-1),
         unsigned FingerprintBits = 12, std::size_t MaxKicks = static_cast<std::size_t>(-1),
         class Key = std::uint64_t>
class CuckooFilter {
    static_assert(BucketCount > 1 && (BucketCount & (BucketCount - 1U)) == 0U,
                  "BucketCount must be a power of two");
    static_assert(SlotsPerBucket > 0, "SlotsPerBucket must be positive");
    static_assert(FingerprintBits > 0 && FingerprintBits <= 16,
                  "FingerprintBits must be in [1, 16]");
    static_assert(MaxKicks > 0, "MaxKicks must be positive");

    using Fingerprint = std::uint16_t;
    static constexpr std::uint32_t fingerprint_mask_ =
        FingerprintBits == 16 ? 0xffffU : ((1U << FingerprintBits) - 1U);

    struct Change {
        std::size_t bucket = 0;
        std::size_t slot = 0;
        Fingerprint old{};
    };

    std::array<std::array<Fingerprint, SlotsPerBucket>, BucketCount> buckets_{};
    std::size_t size_ = 0;
    std::uint64_t seed_;
    std::uint64_t insertion_serial_ = 0;

    [[nodiscard]] Fingerprint fingerprint(std::uint64_t hashed) const noexcept {
        Fingerprint result = static_cast<Fingerprint>(
            approximate_streaming_detail::splitmix64(hashed ^ seed_) & fingerprint_mask_);
        if(result == Fingerprint{}) result = Fingerprint{1};
        return result;
    }

    [[nodiscard]] std::size_t alternate(std::size_t bucket, Fingerprint value) const noexcept {
        const std::size_t difference = static_cast<std::size_t>(
            approximate_streaming_detail::splitmix64(
                static_cast<std::uint64_t>(value) + seed_) & (BucketCount - 1U));
        return bucket ^ difference;
    }

    [[nodiscard]] static bool bucket_contains(
        const std::array<Fingerprint, SlotsPerBucket>& bucket, Fingerprint value) noexcept {
        for(const Fingerprint entry: bucket){
            if(entry == value) return true;
        }
        return false;
    }

    bool insert_empty(std::size_t bucket, Fingerprint value) noexcept {
        for(Fingerprint& entry: buckets_[bucket]){
            if(entry == Fingerprint{}){
                entry = value;
                ++size_;
                return true;
            }
        }
        return false;
    }

public:
    explicit CuckooFilter(std::uint64_t seed = 0) noexcept: seed_(seed) {}

    void clear() noexcept {
        for(auto& bucket: buckets_) bucket.fill(Fingerprint{});
        size_ = 0;
        insertion_serial_ = 0;
    }

    [[nodiscard]] bool add(const Key& key) {
        const std::uint64_t hashed = approximate_streaming_detail::key_hash(key, seed_);
        Fingerprint current = fingerprint(hashed);
        const std::size_t first = static_cast<std::size_t>(hashed & (BucketCount - 1U));
        const std::size_t second = alternate(first, current);
        if(bucket_contains(buckets_[first], current)
           || bucket_contains(buckets_[second], current)) return true;
        if(insert_empty(first, current) || insert_empty(second, current)) return true;

        std::uint64_t random = approximate_streaming_detail::splitmix64(
            hashed + seed_ + insertion_serial_++);
        std::size_t bucket = (random & 1ULL) == 0ULL ? first : second;
        std::array<Change, MaxKicks> changes{};
        std::size_t change_count = 0;
        for(std::size_t kick = 0; kick < MaxKicks; ++kick){
            random = approximate_streaming_detail::splitmix64(random);
            const std::size_t slot = static_cast<std::size_t>(random % SlotsPerBucket);
            changes[change_count++] = Change{bucket, slot, buckets_[bucket][slot]};
            std::swap(current, buckets_[bucket][slot]);
            bucket = alternate(bucket, current);
            if(insert_empty(bucket, current)) return true;
        }
        for(std::size_t i = change_count; i != 0U; --i){
            const Change& change = changes[i - 1U];
            buckets_[change.bucket][change.slot] = change.old;
        }
        return false;
    }

    [[nodiscard]] bool contains(const Key& key) const {
        const std::uint64_t hashed = approximate_streaming_detail::key_hash(key, seed_);
        const Fingerprint value = fingerprint(hashed);
        const std::size_t first = static_cast<std::size_t>(hashed & (BucketCount - 1U));
        const std::size_t second = alternate(first, value);
        return bucket_contains(buckets_[first], value) || bucket_contains(buckets_[second], value);
    }

    [[nodiscard]] bool erase(const Key& key) {
        const std::uint64_t hashed = approximate_streaming_detail::key_hash(key, seed_);
        const Fingerprint value = fingerprint(hashed);
        const std::size_t first = static_cast<std::size_t>(hashed & (BucketCount - 1U));
        const std::size_t second = alternate(first, value);
        for(const std::size_t bucket: {first, second}){
            for(Fingerprint& entry: buckets_[bucket]){
                if(entry == value){
                    entry = Fingerprint{};
                    --size_;
                    return true;
                }
            }
        }
        return false;
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] static constexpr std::size_t capacity() noexcept {
        return BucketCount * SlotsPerBucket;
    }
    [[nodiscard]] long double load_factor() const noexcept {
        return static_cast<long double>(size_) / static_cast<long double>(capacity());
    }
};


// Explicit fixed bucket counts retain their former slot/kick defaults.
template<std::size_t Buckets, std::size_t Slots, unsigned Bits, class Key>
class CuckooFilter<Buckets, Slots, Bits, static_cast<std::size_t>(-1), Key>
    : public CuckooFilter<Buckets, Slots, Bits, 500, Key>{
    using Base = CuckooFilter<Buckets, Slots, Bits, 500, Key>;
public:
    using Base::Base;
};
template<std::size_t Buckets, unsigned Bits, class Key>
class CuckooFilter<Buckets, static_cast<std::size_t>(-1), Bits, static_cast<std::size_t>(-1), Key>
    : public CuckooFilter<Buckets, 4, Bits, 500, Key>{
    using Base = CuckooFilter<Buckets, 4, Bits, 500, Key>;
public:
    using Base::Base;
};

template<unsigned FingerprintBits, class Key>
class CuckooFilter<static_cast<std::size_t>(-1), static_cast<std::size_t>(-1), FingerprintBits, static_cast<std::size_t>(-1), Key> {
    std::size_t bucket_count_;
    std::size_t slots_per_bucket_;
    std::size_t maximum_kicks_;



    static_assert(FingerprintBits > 0 && FingerprintBits <= 16,
                  "FingerprintBits must be in [1, 16]");


    using Fingerprint = std::uint16_t;
    static constexpr std::uint32_t fingerprint_mask_ =
        FingerprintBits == 16 ? 0xffffU : ((1U << FingerprintBits) - 1U);

    struct Change {
        std::size_t bucket = 0;
        std::size_t slot = 0;
        Fingerprint old{};
    };

    std::vector<std::vector<Fingerprint>> buckets_{};
    std::size_t size_ = 0;
    std::uint64_t seed_;
    std::uint64_t insertion_serial_ = 0;

    [[nodiscard]] Fingerprint fingerprint(std::uint64_t hashed) const noexcept {
        Fingerprint result = static_cast<Fingerprint>(
            approximate_streaming_detail::splitmix64(hashed ^ seed_) & fingerprint_mask_);
        if(result == Fingerprint{}) result = Fingerprint{1};
        return result;
    }

    [[nodiscard]] std::size_t alternate(std::size_t bucket, Fingerprint value) const noexcept {
        const std::size_t difference = static_cast<std::size_t>(
            approximate_streaming_detail::splitmix64(
                static_cast<std::uint64_t>(value) + seed_) & (bucket_count_ - 1U));
        return bucket ^ difference;
    }

    [[nodiscard]] static bool bucket_contains(
        const std::vector<Fingerprint>& bucket, Fingerprint value) noexcept {
        for(const Fingerprint entry: bucket){
            if(entry == value) return true;
        }
        return false;
    }

    bool insert_empty(std::size_t bucket, Fingerprint value) noexcept {
        for(Fingerprint& entry: buckets_[bucket]){
            if(entry == Fingerprint{}){
                entry = value;
                ++size_;
                return true;
            }
        }
        return false;
    }

public:
    explicit CuckooFilter(std::size_t bucket_count, std::size_t slots_per_bucket = 4, std::size_t maximum_kicks = 500, std::uint64_t seed = 0) : bucket_count_(bucket_count), slots_per_bucket_(slots_per_bucket), maximum_kicks_(maximum_kicks),  seed_(seed) {
        if(bucket_count_ == 0) throw std::invalid_argument("dimension must be positive");
        if(slots_per_bucket_ == 0) throw std::invalid_argument("dimension must be positive");
        if(maximum_kicks_ == 0) throw std::invalid_argument("dimension must be positive");
        if(bucket_count_ < 2 || (bucket_count_ & (bucket_count_ - 1))) throw std::invalid_argument("bucket count must be a power of two");
        if(bucket_count_ > std::numeric_limits<std::size_t>::max() / slots_per_bucket_) throw std::length_error("filter size overflow");
        buckets_.resize(bucket_count_);
        for(auto& row: buckets_) row.resize(slots_per_bucket_);
}

    void clear() noexcept {
        for(auto& bucket: buckets_) std::fill(bucket.begin(), bucket.end(), Fingerprint{});
        size_ = 0;
        insertion_serial_ = 0;
    }

    [[nodiscard]] bool add(const Key& key) {
        const std::uint64_t hashed = approximate_streaming_detail::key_hash(key, seed_);
        Fingerprint current = fingerprint(hashed);
        const std::size_t first = static_cast<std::size_t>(hashed & (bucket_count_ - 1U));
        const std::size_t second = alternate(first, current);
        if(bucket_contains(buckets_[first], current)
           || bucket_contains(buckets_[second], current)) return true;
        if(insert_empty(first, current) || insert_empty(second, current)) return true;

        std::uint64_t random = approximate_streaming_detail::splitmix64(
            hashed + seed_ + insertion_serial_++);
        std::size_t bucket = (random & 1ULL) == 0ULL ? first : second;
        std::vector<Change> changes(maximum_kicks_);
        std::size_t change_count = 0;
        for(std::size_t kick = 0; kick < maximum_kicks_; ++kick){
            random = approximate_streaming_detail::splitmix64(random);
            const std::size_t slot = static_cast<std::size_t>(random % slots_per_bucket_);
            changes[change_count++] = Change{bucket, slot, buckets_[bucket][slot]};
            std::swap(current, buckets_[bucket][slot]);
            bucket = alternate(bucket, current);
            if(insert_empty(bucket, current)) return true;
        }
        for(std::size_t i = change_count; i != 0U; --i){
            const Change& change = changes[i - 1U];
            buckets_[change.bucket][change.slot] = change.old;
        }
        return false;
    }

    [[nodiscard]] bool contains(const Key& key) const {
        const std::uint64_t hashed = approximate_streaming_detail::key_hash(key, seed_);
        const Fingerprint value = fingerprint(hashed);
        const std::size_t first = static_cast<std::size_t>(hashed & (bucket_count_ - 1U));
        const std::size_t second = alternate(first, value);
        return bucket_contains(buckets_[first], value) || bucket_contains(buckets_[second], value);
    }

    [[nodiscard]] bool erase(const Key& key) {
        const std::uint64_t hashed = approximate_streaming_detail::key_hash(key, seed_);
        const Fingerprint value = fingerprint(hashed);
        const std::size_t first = static_cast<std::size_t>(hashed & (bucket_count_ - 1U));
        const std::size_t second = alternate(first, value);
        for(const std::size_t bucket: {first, second}){
            for(Fingerprint& entry: buckets_[bucket]){
                if(entry == value){
                    entry = Fingerprint{};
                    --size_;
                    return true;
                }
            }
        }
        return false;
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] std::size_t capacity() const noexcept {
        return bucket_count_ * slots_per_bucket_;
    }
    [[nodiscard]] long double load_factor() const noexcept {
        return static_cast<long double>(size_) / static_cast<long double>(capacity());
    }
};
#endif  // CPPLIB_SRC_APPROXIMATE_STREAMING_CUCKOO_FILTER_HPP_INCLUDED
