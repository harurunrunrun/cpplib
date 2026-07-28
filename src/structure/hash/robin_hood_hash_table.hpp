#ifndef CPPLIB_SRC_STRUCTURE_HASH_ROBIN_HOOD_HASH_TABLE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_HASH_ROBIN_HOOD_HASH_TABLE_HPP_INCLUDED

#include <bit>
#include <cstddef>
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
    Hash hash_;
    Equal equal_;

    [[nodiscard]] std::size_t mask() const { return table_.size() - 1; }
    void rehash(std::size_t capacity) {
        capacity = std::max<std::size_t>(8, std::bit_ceil(capacity));
        auto old = std::move(table_);
        table_.assign(capacity, std::nullopt);
        size_ = 0;
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
        std::size_t index = hash_(key) & mask();
        Entry incoming{
            std::forward<K>(key), std::forward<V>(value), 0
        };
        while (true) {
            auto& slot = table_[index];
            if (!slot) {
                slot.emplace(std::move(incoming));
                ++size_;
                return true;
            }
            if (equal_(slot->key, incoming.key)) {
                slot->value = std::move(incoming.value);
                return false;
            }
            if (slot->distance < incoming.distance) {
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
        std::size_t index = hash_(key) & mask();
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
        std::size_t index = hash_(key) & mask();
        std::size_t distance = 0;
        while (table_[index] && table_[index]->distance >= distance) {
            if (equal_(table_[index]->key, key)) {
                std::size_t next = (index + 1) & mask();
                while (table_[next] && table_[next]->distance > 0) {
                    table_[index] = std::move(table_[next]);
                    --table_[index]->distance;
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
