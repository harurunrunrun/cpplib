#ifndef CPPLIB_SRC_STRUCTURE_TRIE_STRING_CRIT_BIT_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TRIE_STRING_CRIT_BIT_TREE_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class CritBitTree {
    struct Node {
        bool leaf = true;
        std::string key;
        std::size_t byte = 0;
        std::uint8_t mask = 0;
        std::unique_ptr<Node> child[2];
        explicit Node(std::string key_) : key(std::move(key_)) {}
        Node(std::size_t byte_, std::uint8_t mask_)
            : leaf(false), byte(byte_), mask(mask_) {}
    };
    std::unique_ptr<Node> root_;
    std::size_t size_ = 0;

    static unsigned char byte_at(std::string_view key, std::size_t index) {
        return index < key.size()
            ? static_cast<unsigned char>(key[index]) : 0;
    }
    static int direction(
        std::string_view key, std::size_t byte, std::uint8_t mask
    ) {
        return (byte_at(key, byte) & mask) != 0;
    }
    static void collect(const Node* node, std::vector<std::string>& output) {
        if (!node) return;
        if (node->leaf) {
            output.push_back(node->key);
            return;
        }
        collect(node->child[0].get(), output);
        collect(node->child[1].get(), output);
    }

public:
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool contains(std::string_view key) const {
        const Node* node = root_.get();
        if (!node) return false;
        while (!node->leaf) {
            node = node->child[direction(key, node->byte, node->mask)].get();
        }
        return node->key == key;
    }
    bool insert(std::string key) {
        if (!root_) {
            root_ = std::make_unique<Node>(std::move(key));
            ++size_;
            return true;
        }
        Node* leaf = root_.get();
        while (!leaf->leaf) {
            leaf = leaf->child[
                direction(key, leaf->byte, leaf->mask)
            ].get();
        }
        if (leaf->key == key) return false;
        std::size_t byte = 0;
        while (byte_at(key, byte) == byte_at(leaf->key, byte)) ++byte;
        unsigned char difference =
            byte_at(key, byte) ^ byte_at(leaf->key, byte);
        std::uint8_t mask = 0x80U;
        while ((difference & mask) == 0) mask >>= 1;

        std::unique_ptr<Node>* link = &root_;
        while (!(*link)->leaf
               && ((*link)->byte < byte
                   || ((*link)->byte == byte && (*link)->mask > mask))) {
            link = &(*link)->child[
                direction(key, (*link)->byte, (*link)->mask)
            ];
        }
        auto branch = std::make_unique<Node>(byte, mask);
        const int new_direction = direction(key, byte, mask);
        branch->child[new_direction] =
            std::make_unique<Node>(std::move(key));
        branch->child[1 - new_direction] = std::move(*link);
        *link = std::move(branch);
        ++size_;
        return true;
    }
    bool erase(std::string_view key) {
        if (!root_) return false;
        if (root_->leaf) {
            if (root_->key != key) return false;
            root_.reset();
            size_ = 0;
            return true;
        }
        std::unique_ptr<Node>* link = &root_;
        std::unique_ptr<Node>* parent_link = nullptr;
        Node* parent = nullptr;
        int child_direction = 0;
        while (!(*link)->leaf) {
            parent_link = link;
            parent = link->get();
            child_direction = direction(key, parent->byte, parent->mask);
            link = &parent->child[child_direction];
        }
        if ((*link)->key != key) return false;
        auto sibling = std::move(parent->child[1 - child_direction]);
        *parent_link = std::move(sibling);
        --size_;
        return true;
    }
    [[nodiscard]] std::vector<std::string> keys() const {
        std::vector<std::string> result;
        result.reserve(size_);
        collect(root_.get(), result);
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TRIE_STRING_CRIT_BIT_TREE_HPP_INCLUDED
