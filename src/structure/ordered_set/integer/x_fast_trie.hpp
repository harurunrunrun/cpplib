#ifndef CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_X_FAST_TRIE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_X_FAST_TRIE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>

template<class UInt = std::uint64_t,
         int BITS = std::numeric_limits<UInt>::digits>
class XFastTrie {
    static_assert(std::is_unsigned_v<UInt>);
    static_assert(1 <= BITS && BITS <= std::numeric_limits<UInt>::digits);
    struct Node {
        std::unique_ptr<Node> child[2];
        Node* parent = nullptr;
        Node* minimum_leaf = nullptr;
        Node* maximum_leaf = nullptr;
        Node* previous = nullptr;
        Node* next = nullptr;
        UInt key = 0;
    };
    std::unique_ptr<Node> root_ = std::make_unique<Node>();
    std::array<std::unordered_map<UInt, Node*>, BITS + 1> levels_;
    std::size_t size_ = 0;

    static UInt prefix(UInt key, int depth) {
        return depth == 0 ? UInt{0} : key >> (BITS - depth);
    }
    static void check_key(UInt key) {
        if constexpr (BITS < std::numeric_limits<UInt>::digits) {
            if ((key >> BITS) != 0) {
                throw std::out_of_range("XFastTrie key outside universe");
            }
        }
    }
    static void refresh(Node* node) {
        if (!node->child[0] && !node->child[1]) {
            node->minimum_leaf = node;
            node->maximum_leaf = node;
            return;
        }
        node->minimum_leaf = node->child[0]
            ? node->child[0]->minimum_leaf : node->child[1]->minimum_leaf;
        node->maximum_leaf = node->child[1]
            ? node->child[1]->maximum_leaf : node->child[0]->maximum_leaf;
    }

public:
    XFastTrie() { levels_[0][0] = root_.get(); }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool contains(UInt key) const {
        check_key(key);
        return levels_[BITS].contains(key);
    }
    bool insert(UInt key) {
        check_key(key);
        if (contains(key)) return false;
        Node* predecessor_node = nullptr;
        Node* successor_node = nullptr;
        if (!empty()) {
            const auto predecessor_key = predecessor(key);
            const auto successor_key = lower_bound(key);
            if (predecessor_key) {
                predecessor_node = levels_[BITS].at(*predecessor_key);
            }
            if (successor_key) {
                successor_node = levels_[BITS].at(*successor_key);
            }
        }
        Node* node = root_.get();
        std::array<Node*, BITS + 1> path{};
        path[0] = node;
        for (int depth = 1; depth <= BITS; ++depth) {
            const int bit = static_cast<int>((key >> (BITS - depth)) & 1U);
            if (!node->child[bit]) {
                node->child[bit] = std::make_unique<Node>();
                node->child[bit]->parent = node;
                levels_[depth][prefix(key, depth)] =
                    node->child[bit].get();
            }
            node = node->child[bit].get();
            path[depth] = node;
        }
        node->key = key;
        node->previous = predecessor_node;
        node->next = successor_node;
        if (predecessor_node) predecessor_node->next = node;
        if (successor_node) successor_node->previous = node;
        for (int depth = BITS; depth >= 0; --depth) refresh(path[depth]);
        ++size_;
        return true;
    }
    bool erase(UInt key) {
        check_key(key);
        auto iterator = levels_[BITS].find(key);
        if (iterator == levels_[BITS].end()) return false;
        Node* leaf = iterator->second;
        if (leaf->previous) leaf->previous->next = leaf->next;
        if (leaf->next) leaf->next->previous = leaf->previous;
        Node* node = leaf;
        for (int depth = BITS; depth >= 1; --depth) {
            Node* parent = node->parent;
            const int bit = parent->child[1].get() == node;
            levels_[depth].erase(prefix(key, depth));
            parent->child[bit].reset();
            refresh(parent);
            node = parent;
            if (parent->child[0] || parent->child[1]) break;
        }
        for (Node* current = node; current; current = current->parent) {
            refresh(current);
        }
        --size_;
        return true;
    }
    [[nodiscard]] std::optional<UInt> lower_bound(UInt key) const {
        check_key(key);
        if (empty()) return std::nullopt;
        if (const auto found = levels_[BITS].find(key);
            found != levels_[BITS].end()) return key;
        int low = 0;
        int high = BITS;
        while (high - low > 1) {
            const int middle = (low + high) / 2;
            if (levels_[middle].contains(prefix(key, middle))) low = middle;
            else high = middle;
        }
        Node* node = levels_[low].at(prefix(key, low));
        const int missing_bit =
            static_cast<int>((key >> (BITS - low - 1)) & 1U);
        Node* candidate = missing_bit == 0
            ? node->minimum_leaf : node->maximum_leaf->next;
        if (!candidate) return std::nullopt;
        return candidate->key;
    }
    [[nodiscard]] std::optional<UInt> predecessor(UInt key) const {
        check_key(key);
        if (empty()) return std::nullopt;
        const auto lower = lower_bound(key);
        if (!lower) return root_->maximum_leaf->key;
        Node* leaf = levels_[BITS].at(*lower);
        if (*lower < key) return *lower;
        if (!leaf->previous) return std::nullopt;
        return leaf->previous->key;
    }
    [[nodiscard]] std::optional<UInt> successor(UInt key) const {
        check_key(key);
        if constexpr (BITS == std::numeric_limits<UInt>::digits) {
            if (key == std::numeric_limits<UInt>::max()) return std::nullopt;
        } else {
            if (key == (UInt{1} << BITS) - 1) {
                return std::nullopt;
            }
        }
        return lower_bound(static_cast<UInt>(key + 1));
    }
    [[nodiscard]] UInt minimum() const {
        if (empty()) throw std::out_of_range("XFastTrie::minimum");
        return root_->minimum_leaf->key;
    }
    [[nodiscard]] UInt maximum() const {
        if (empty()) throw std::out_of_range("XFastTrie::maximum");
        return root_->maximum_leaf->key;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_X_FAST_TRIE_HPP_INCLUDED
