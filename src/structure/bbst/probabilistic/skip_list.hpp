#ifndef CPPLIB_SRC_STRUCTURE_BBST_PROBABILISTIC_SKIP_LIST_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_PROBABILISTIC_SKIP_LIST_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <new>
#include <optional>
#include <utility>

template<class Key, class Value, std::size_t MAX_LEVEL = 32,
         class Compare = std::less<Key>>
class SkipList {
    static_assert(MAX_LEVEL >= 1);
    struct Node {
        // 高さ分だけのリンク配列（通常ノードはノード直後に確保、番兵は head_links_）
        Node** next = nullptr;
        std::optional<std::pair<Key, Value>> entry;
        std::size_t height = MAX_LEVEL;
        Node() = default;
        Node(Key key, Value value, std::size_t height_)
            : entry(std::in_place, std::move(key), std::move(value)),
              height(height_) {}
    };
    static constexpr std::align_val_t node_align_{alignof(Node)};
    static Node* new_node(Key&& key, Value&& value, std::size_t height) {
        void* memory = ::operator new(
            sizeof(Node) + height * sizeof(Node*), node_align_);
        Node* node;
        try {
            node = ::new (memory) Node(std::move(key), std::move(value), height);
        } catch (...) {
            ::operator delete(memory, node_align_);
            throw;
        }
        Node** links = reinterpret_cast<Node**>(
            static_cast<unsigned char*>(memory) + sizeof(Node));
        for (std::size_t i = 0; i < height; ++i) ::new (links + i) Node*(nullptr);
        node->next = links;
        return node;
    }
    static void delete_node(Node* node) noexcept {
        node->~Node();
        ::operator delete(static_cast<void*>(node), node_align_);
    }
    std::array<Node*, MAX_LEVEL> head_links_{};
    Node head_ = make_head();
    Node make_head() {
        Node head;
        head.next = head_links_.data();
        return head;
    }
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
            delete_node(node);
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
        Node* inserted = new_node(std::move(key), std::move(value), height);
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
        delete_node(target);
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
