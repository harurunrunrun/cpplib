#ifndef CPPLIB_SRC_STRUCTURE_HASH_ROBIN_HOOD_HASH_TABLE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_HASH_ROBIN_HOOD_HASH_TABLE_HPP_INCLUDED

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
class RobinHoodHashTable {
    struct Entry {
        Key key;
        Value value;
        std::size_t distance;
    };
    std::vector<std::optional<Entry>> table_;
    std::size_t size_ = 0;
    std::size_t total_distance_ = 0;  // 全要素の探索距離の和
    bool mixed_ = false;
    Hash hash_;
    Equal equal_;

    [[nodiscard]] std::size_t mask() const { return table_.size() - 1; }
    // 最初は Hash の値をそのまま mask する（連番の整数キーなどで最も速い）。
    // 挿入で置いた要素の探索距離が kMixDistance を超えるか、探索距離の平均が
    // kMixAverage を超えたら、以後は SplitMix64 で混ぜてから mask するよう切り替えて
    // 同じ大きさで再ハッシュする（下位ビットが揃うキー対策）。切り替えは高々 1 回。
    static constexpr std::size_t kMixDistance = 32;
    static constexpr std::size_t kMixAverage = 4;
    static std::uint64_t mix(std::uint64_t value) {
        value += 0x9e3779b97f4a7c15ULL;
        value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
        value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
        return value ^ (value >> 31);
    }
    std::size_t bucket(const Key& key) {
        const auto hash = static_cast<std::uint64_t>(hash_(key));
        return static_cast<std::size_t>(mixed_ ? mix(hash) : hash) & mask();
    }
    void rehash(std::size_t capacity) {
        capacity = std::max<std::size_t>(8, std::bit_ceil(capacity));
        auto old = std::move(table_);
        table_.assign(capacity, std::nullopt);
        size_ = 0;
        total_distance_ = 0;
        for (auto& slot : old) {
            if (slot) insert_or_assign(
                std::move(slot->key), std::move(slot->value)
            );
        }
    }
    template<class K, class V>
    bool insert_impl(K&& key, V&& value) {
        if (table_.empty() || (size_ + 1) * 10 > table_.size() * 8) {
            rehash(table_.empty() ? 8 : table_.size() * 2);
        }
        std::size_t index = bucket(key);
        Entry incoming{
            std::forward<K>(key), std::forward<V>(value), 0
        };
        // steps: この挿入で増えた探索距離の合計（= 進んだスロット数）
        // longest: 置いた要素の探索距離の最大値が kMixDistance を超えたか
        bool longest = false;
        for (std::size_t steps = 0;; ++steps) {
            auto& slot = table_[index];
            if (!slot) {
                longest |= incoming.distance > kMixDistance;
                slot.emplace(std::move(incoming));
                ++size_;
                total_distance_ += steps;
                if (!mixed_ && (longest ||
                                total_distance_ > kMixAverage * (size_ + 8)))
                    [[unlikely]] {
                    mixed_ = true;
                    rehash(table_.size());
                }
                return true;
            }
            if (equal_(slot->key, incoming.key)) {
                slot->value = std::move(incoming.value);
                return false;
            }
            if (slot->distance < incoming.distance) {
                longest |= incoming.distance > kMixDistance;
                std::swap(*slot, incoming);
            }
            index = (index + 1) & mask();
            ++incoming.distance;
        }
    }

public:
    RobinHoodHashTable() { rehash(8); }
    explicit RobinHoodHashTable(std::size_t capacity) { rehash(capacity); }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    bool insert_or_assign(const Key& key, const Value& value) {
        return insert_impl(key, value);
    }
    bool insert_or_assign(Key&& key, Value&& value) {
        return insert_impl(std::move(key), std::move(value));
    }
    Value* find(const Key& key) {
        if (table_.empty()) return nullptr;
        std::size_t index = bucket(key);
        std::size_t distance = 0;
        while (table_[index] && table_[index]->distance >= distance) {
            if (equal_(table_[index]->key, key)) return &table_[index]->value;
            index = (index + 1) & mask();
            ++distance;
        }
        return nullptr;
    }
    const Value* find(const Key& key) const {
        return const_cast<RobinHoodHashTable*>(this)->find(key);
    }
    [[nodiscard]] bool contains(const Key& key) const {
        return find(key) != nullptr;
    }
    Value& at(const Key& key) {
        Value* result = find(key);
        if (!result) throw std::out_of_range("RobinHoodHashTable::at");
        return *result;
    }
    const Value& at(const Key& key) const {
        const Value* result = find(key);
        if (!result) throw std::out_of_range("RobinHoodHashTable::at");
        return *result;
    }
    bool erase(const Key& key) {
        std::size_t index = bucket(key);
        std::size_t distance = 0;
        while (table_[index] && table_[index]->distance >= distance) {
            if (equal_(table_[index]->key, key)) {
                total_distance_ -= table_[index]->distance;
                std::size_t next = (index + 1) & mask();
                while (table_[next] && table_[next]->distance > 0) {
                    table_[index] = std::move(table_[next]);
                    --table_[index]->distance;
                    --total_distance_;
                    index = next;
                    next = (next + 1) & mask();
                }
                table_[index].reset();
                --size_;
                return true;
            }
            index = (index + 1) & mask();
            ++distance;
        }
        return false;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_HASH_ROBIN_HOOD_HASH_TABLE_HPP_INCLUDED
