#ifndef CPPLIB_SRC_STRUCTURE_HASH_CUCKOO_HASH_TABLE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_HASH_CUCKOO_HASH_TABLE_HPP_INCLUDED

#include <bit>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

template<class Key, class Value, class Hash = std::hash<Key>,
         class Equal = std::equal_to<Key>>
class CuckooHashTable {
    struct Entry { Key key; Value value; };
    std::vector<std::optional<Entry>> first_;
    std::vector<std::optional<Entry>> second_;
    std::vector<Entry> fallback_;
    std::size_t size_ = 0;
    bool fallback_mode_ = false;
    std::uint64_t seed_ = 0x9e3779b97f4a7c15ULL;
    Hash hash_;
    Equal equal_;

    static std::uint64_t mix(std::uint64_t value) {
        value ^= value >> 30;
        value *= 0xbf58476d1ce4e5b9ULL;
        value ^= value >> 27;
        value *= 0x94d049bb133111ebULL;
        return value ^ (value >> 31);
    }
    std::size_t index(const Key& key, int table) const {
        const std::uint64_t value =
            static_cast<std::uint64_t>(hash_(key));
        return static_cast<std::size_t>(
            mix(value + seed_ + static_cast<std::uint64_t>(table)
                * 0x517cc1b727220a95ULL)
        ) & (first_.size() - 1);
    }
    std::optional<Entry> place(Entry entry) {
        int table = 0;
        for (std::size_t step = 0; step < 4 * first_.size(); ++step) {
            auto& slot = table == 0
                ? first_[index(entry.key, 0)]
                : second_[index(entry.key, 1)];
            if (!slot) {
                slot.emplace(std::move(entry));
                return std::nullopt;
            }
            std::swap(entry, *slot);
            table ^= 1;
        }
        return entry;
    }
    void rehash(std::size_t capacity, std::optional<Entry> extra = std::nullopt) {
        std::vector<Entry> entries;
        entries.reserve(size_ + (extra ? 1 : 0));
        for (auto& slot : first_) if (slot) entries.push_back(std::move(*slot));
        for (auto& slot : second_) if (slot) entries.push_back(std::move(*slot));
        if (extra) entries.push_back(std::move(*extra));
        capacity = std::max<std::size_t>(8, std::bit_ceil(capacity));
        constexpr std::size_t max_rehash_attempts = 64;
        for (std::size_t attempt = 0; attempt < max_rehash_attempts; ++attempt) {
            seed_ = mix(seed_ + 0x9e3779b97f4a7c15ULL);
            first_.assign(capacity, std::nullopt);
            second_.assign(capacity, std::nullopt);
            bool success = true;
            for (const Entry& entry : entries) {
                if (place(entry)) {
                    success = false;
                    break;
                }
            }
            if (success) {
                size_ = entries.size();
                return;
            }
        }
        first_.clear();
        second_.clear();
        fallback_ = std::move(entries);
        fallback_mode_ = true;
        size_ = fallback_.size();
    }

public:
    CuckooHashTable() { rehash(8); }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    Value* find(const Key& key) {
        if (fallback_mode_) {
            for (Entry& entry : fallback_) {
                if (equal_(entry.key, key)) return &entry.value;
            }
            return nullptr;
        }
        auto& a = first_[index(key, 0)];
        if (a && equal_(a->key, key)) return &a->value;
        auto& b = second_[index(key, 1)];
        if (b && equal_(b->key, key)) return &b->value;
        return nullptr;
    }
    const Value* find(const Key& key) const {
        return const_cast<CuckooHashTable*>(this)->find(key);
    }
    [[nodiscard]] bool contains(const Key& key) const {
        return find(key) != nullptr;
    }
    bool insert_or_assign(const Key& key, const Value& value) {
        if (Value* current = find(key)) {
            *current = value;
            return false;
        }
        if (fallback_mode_) {
            fallback_.push_back(Entry{key, value});
            ++size_;
            return true;
        }
        if ((size_ + 1) * 2 > first_.size()) {
            rehash(first_.size() * 2, Entry{key, value});
        } else if (auto displaced = place(Entry{key, value})) {
            rehash(first_.size() * 2, std::move(displaced));
        } else ++size_;
        return true;
    }
    bool erase(const Key& key) {
        if (fallback_mode_) {
            for (std::size_t i = 0; i < fallback_.size(); ++i) {
                if (!equal_(fallback_[i].key, key)) continue;
                if (i + 1 != fallback_.size()) {
                    fallback_[i] = std::move(fallback_.back());
                }
                fallback_.pop_back();
                --size_;
                return true;
            }
            return false;
        }
        auto& a = first_[index(key, 0)];
        if (a && equal_(a->key, key)) {
            a.reset();
            --size_;
            return true;
        }
        auto& b = second_[index(key, 1)];
        if (b && equal_(b->key, key)) {
            b.reset();
            --size_;
            return true;
        }
        return false;
    }
    Value& at(const Key& key) {
        Value* result = find(key);
        if (!result) throw std::out_of_range("CuckooHashTable::at");
        return *result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_HASH_CUCKOO_HASH_TABLE_HPP_INCLUDED
