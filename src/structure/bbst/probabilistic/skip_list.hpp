#ifndef CPPLIB_SRC_STRUCTURE_BBST_PROBABILISTIC_SKIP_LIST_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_PROBABILISTIC_SKIP_LIST_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <utility>

template<class Key, class Value, std::size_t MAX_LEVEL = 32,
         class Compare = std::less<Key>>
class SkipList {
    static_assert(MAX_LEVEL >= 1);
    struct Node {
        std::optional<std::pair<Key, Value>> entry;
        std::array<Node*, MAX_LEVEL> next{};
        std::size_t height = MAX_LEVEL;
        Node() = default;
        Node(Key key, Value value, std::size_t height_)
            : entry(std::in_place, std::move(key), std::move(value)),
              height(height_) {}
    };
    Node head_;
    std::size_t size_ = 0;
    std::size_t level_ = 1;
    Compare compare_;
    std::uint64_t random_state_ = 0x9e3779b97f4a7c15ULL;

    [[nodiscard]] bool equal(const Key& a, const Key& b) const {
        return !compare_(a, b) && !compare_(b, a);
    }
    std::size_t random_level() {
        std::size_t result = 1;
        while (result < MAX_LEVEL) {
            random_state_ ^= random_state_ << 7;
            random_state_ ^= random_state_ >> 9;
            if ((random_state_ & 1U) == 0) break;
            ++result;
        }
        return result;
    }

public:
    SkipList() = default;
    explicit SkipList(Compare compare) : compare_(std::move(compare)) {}
    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;
    ~SkipList() {
        Node* node = head_.next[0];
        while (node) {
            Node* next = node->next[0];
            delete node;
            node = next;
        }
    }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    Value* find(const Key& key) {
        Node* node = &head_;
        for (std::size_t current = level_; current-- > 0;) {
            while (node->next[current]
                   && compare_(node->next[current]->entry->first, key)) {
                node = node->next[current];
            }
        }
        node = node->next[0];
        return node && equal(node->entry->first, key)
            ? &node->entry->second : nullptr;
    }
    const Value* find(const Key& key) const {
        return const_cast<SkipList*>(this)->find(key);
    }
    bool insert_or_assign(Key key, Value value) {
        std::array<Node*, MAX_LEVEL> update{};
        Node* node = &head_;
        for (std::size_t current = level_; current-- > 0;) {
            while (node->next[current]
                   && compare_(node->next[current]->entry->first, key)) {
                node = node->next[current];
            }
            update[current] = node;
        }
        node = node->next[0];
        if (node && equal(node->entry->first, key)) {
            node->entry->second = std::move(value);
            return false;
        }
        const std::size_t height = random_level();
        if (height > level_) {
            for (std::size_t i = level_; i < height; ++i) update[i] = &head_;
            level_ = height;
        }
        auto* inserted = new Node(std::move(key), std::move(value), height);
        for (std::size_t i = 0; i < height; ++i) {
            inserted->next[i] = update[i]->next[i];
            update[i]->next[i] = inserted;
        }
        ++size_;
        return true;
    }
    bool erase(const Key& key) {
        std::array<Node*, MAX_LEVEL> update{};
        Node* node = &head_;
        for (std::size_t current = level_; current-- > 0;) {
            while (node->next[current]
                   && compare_(node->next[current]->entry->first, key)) {
                node = node->next[current];
            }
            update[current] = node;
        }
        Node* target = node->next[0];
        if (!target || !equal(target->entry->first, key)) return false;
        for (std::size_t i = 0; i < target->height; ++i) {
            if (update[i]->next[i] == target) {
                update[i]->next[i] = target->next[i];
            }
        }
        delete target;
        while (level_ > 1 && !head_.next[level_ - 1]) --level_;
        --size_;
        return true;
    }
    [[nodiscard]] std::optional<std::pair<Key, Value>> lower_bound(
        const Key& key
    ) const {
        Node* node = const_cast<Node*>(&head_);
        for (std::size_t current = level_; current-- > 0;) {
            while (node->next[current]
                   && compare_(node->next[current]->entry->first, key)) {
                node = node->next[current];
            }
        }
        node = node->next[0];
        return node ? node->entry : std::nullopt;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_BBST_PROBABILISTIC_SKIP_LIST_HPP_INCLUDED
