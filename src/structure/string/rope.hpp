#ifndef CPPLIB_SRC_STRUCTURE_STRING_ROPE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_STRING_ROPE_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

class Rope {
    struct Node {
        char value;
        std::uint64_t priority;
        std::size_t size = 1;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        Node(char value_, std::uint64_t priority_)
            : value(value_), priority(priority_) {}
    };
    std::unique_ptr<Node> root_;
    inline static std::uint64_t random_state_ = 0x243f6a8885a308d3ULL;
    static std::uint64_t random_priority() {
        random_state_ ^= random_state_ << 7;
        random_state_ ^= random_state_ >> 9;
        return random_state_;
    }
    static std::size_t size_of(const std::unique_ptr<Node>& node) {
        return node ? node->size : 0;
    }
    static void update(Node* node) {
        node->size = 1 + size_of(node->left) + size_of(node->right);
    }
    static std::unique_ptr<Node> merge(
        std::unique_ptr<Node> left, std::unique_ptr<Node> right
    ) {
        if (!left) return right;
        if (!right) return left;
        if (left->priority > right->priority) {
            left->right = merge(std::move(left->right), std::move(right));
            update(left.get());
            return left;
        }
        right->left = merge(std::move(left), std::move(right->left));
        update(right.get());
        return right;
    }
    static std::pair<std::unique_ptr<Node>, std::unique_ptr<Node>> split(
        std::unique_ptr<Node> node, std::size_t count
    ) {
        if (!node) return {};
        const std::size_t left_size = size_of(node->left);
        if (count <= left_size) {
            auto [left, middle] = split(std::move(node->left), count);
            node->left = std::move(middle);
            update(node.get());
            return {std::move(left), std::move(node)};
        }
        auto [middle, right] = split(
            std::move(node->right), count - left_size - 1
        );
        node->right = std::move(middle);
        update(node.get());
        return {std::move(node), std::move(right)};
    }
    static std::unique_ptr<Node> build(std::string_view text) {
        std::unique_ptr<Node> result;
        for (char character : text) {
            result = merge(
                std::move(result),
                std::make_unique<Node>(character, random_priority())
            );
        }
        return result;
    }
    static void append(const Node* node, std::string& output) {
        if (!node) return;
        append(node->left.get(), output);
        output.push_back(node->value);
        append(node->right.get(), output);
    }

public:
    Rope() = default;
    explicit Rope(std::string_view text) : root_(build(text)) {}
    Rope(Rope&&) noexcept = default;
    Rope& operator=(Rope&&) noexcept = default;
    Rope(const Rope&) = delete;
    Rope& operator=(const Rope&) = delete;
    [[nodiscard]] bool empty() const noexcept { return !root_; }
    [[nodiscard]] std::size_t size() const noexcept { return size_of(root_); }
    [[nodiscard]] char at(std::size_t index) const {
        if (index >= size()) throw std::out_of_range("Rope::at");
        const Node* node = root_.get();
        while (node) {
            const std::size_t left_size =
                node->left ? node->left->size : 0;
            if (index < left_size) node = node->left.get();
            else if (index == left_size) return node->value;
            else {
                index -= left_size + 1;
                node = node->right.get();
            }
        }
        throw std::out_of_range("Rope::at");
    }
    void insert(std::size_t position, std::string_view text) {
        if (position > size()) throw std::out_of_range("Rope::insert");
        auto [left, right] = split(std::move(root_), position);
        root_ = merge(merge(std::move(left), build(text)), std::move(right));
    }
    void erase(std::size_t position, std::size_t length) {
        if (position > size() || length > size() - position) {
            throw std::out_of_range("Rope::erase");
        }
        auto [left, suffix] = split(std::move(root_), position);
        auto [removed, right] = split(std::move(suffix), length);
        (void)removed;
        root_ = merge(std::move(left), std::move(right));
    }
    void set(std::size_t position, char value) {
        if (position >= size()) throw std::out_of_range("Rope::set");
        Node* node = root_.get();
        std::size_t index = position;
        while (true) {
            const std::size_t left_size =
                node->left ? node->left->size : 0;
            if (index < left_size) node = node->left.get();
            else if (index == left_size) {
                node->value = value;
                return;
            } else {
                index -= left_size + 1;
                node = node->right.get();
            }
        }
    }
    [[nodiscard]] std::string substr(
        std::size_t position, std::size_t length
    ) {
        if (position > size() || length > size() - position) {
            throw std::out_of_range("Rope::substr");
        }
        auto [left, suffix] = split(std::move(root_), position);
        auto [middle, right] = split(std::move(suffix), length);
        std::string result;
        result.reserve(length);
        append(middle.get(), result);
        root_ = merge(
            std::move(left), merge(std::move(middle), std::move(right))
        );
        return result;
    }
    [[nodiscard]] std::string str() const {
        std::string result;
        result.reserve(size());
        append(root_.get(), result);
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_STRING_ROPE_HPP_INCLUDED
