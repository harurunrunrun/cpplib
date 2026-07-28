#ifndef CPPLIB_SRC_STRUCTURE_TRIE_STRING_PATRICIA_TRIE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TRIE_STRING_PATRICIA_TRIE_HPP_INCLUDED

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class PatriciaTrie {
    struct Node;
    struct Edge {
        std::string label;
        std::unique_ptr<Node> child;
    };
    struct Node {
        bool terminal = false;
        std::map<unsigned char, Edge> edges;
    };
    Node root_;
    std::size_t size_ = 0;

    static std::size_t common_prefix(
        std::string_view first, std::string_view second
    ) {
        std::size_t length = 0;
        while (length < first.size() && length < second.size()
               && first[length] == second[length]) ++length;
        return length;
    }

    bool erase_impl(Node& node, std::string_view key, bool is_root) {
        if (key.empty()) {
            if (!node.terminal) return false;
            node.terminal = false;
            --size_;
            return true;
        }
        auto iterator = node.edges.find(static_cast<unsigned char>(key[0]));
        if (iterator == node.edges.end()) return false;
        Edge& edge = iterator->second;
        if (!key.starts_with(edge.label)) return false;
        if (!erase_impl(*edge.child, key.substr(edge.label.size()), false)) {
            return false;
        }
        Node* child = edge.child.get();
        if (!child->terminal && child->edges.empty()) {
            node.edges.erase(iterator);
        } else if (!child->terminal && child->edges.size() == 1) {
            auto only = child->edges.begin();
            edge.label += only->second.label;
            edge.child = std::move(only->second.child);
        }
        (void)is_root;
        return true;
    }

    static void collect(
        const Node& node, std::string& current, std::vector<std::string>& output
    ) {
        if (node.terminal) output.push_back(current);
        for (const auto& [unused, edge] : node.edges) {
            (void)unused;
            const std::size_t old_size = current.size();
            current += edge.label;
            collect(*edge.child, current, output);
            current.resize(old_size);
        }
    }

public:
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool contains(std::string_view key) const {
        const Node* node = &root_;
        while (!key.empty()) {
            const auto iterator =
                node->edges.find(static_cast<unsigned char>(key[0]));
            if (iterator == node->edges.end()) return false;
            const Edge& edge = iterator->second;
            if (!key.starts_with(edge.label)) return false;
            key.remove_prefix(edge.label.size());
            node = edge.child.get();
        }
        return node->terminal;
    }
    bool insert(std::string_view key) {
        Node* node = &root_;
        while (!key.empty()) {
            const unsigned char first = static_cast<unsigned char>(key[0]);
            auto iterator = node->edges.find(first);
            if (iterator == node->edges.end()) {
                auto leaf = std::make_unique<Node>();
                leaf->terminal = true;
                node->edges.emplace(
                    first, Edge{std::string(key), std::move(leaf)}
                );
                ++size_;
                return true;
            }
            Edge& edge = iterator->second;
            const std::size_t shared = common_prefix(key, edge.label);
            if (shared == edge.label.size()) {
                key.remove_prefix(shared);
                node = edge.child.get();
                continue;
            }
            auto middle = std::make_unique<Node>();
            const std::string old_suffix = edge.label.substr(shared);
            middle->edges.emplace(
                static_cast<unsigned char>(old_suffix[0]),
                Edge{old_suffix, std::move(edge.child)}
            );
            edge.label.resize(shared);
            edge.child = std::move(middle);
            key.remove_prefix(shared);
            node = edge.child.get();
            if (key.empty()) {
                node->terminal = true;
            } else {
                auto leaf = std::make_unique<Node>();
                leaf->terminal = true;
                node->edges.emplace(
                    static_cast<unsigned char>(key[0]),
                    Edge{std::string(key), std::move(leaf)}
                );
            }
            ++size_;
            return true;
        }
        if (node->terminal) return false;
        node->terminal = true;
        ++size_;
        return true;
    }
    bool erase(std::string_view key) {
        return erase_impl(root_, key, true);
    }
    [[nodiscard]] std::vector<std::string> keys() const {
        std::vector<std::string> result;
        result.reserve(size_);
        std::string current;
        collect(root_, current, result);
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TRIE_STRING_PATRICIA_TRIE_HPP_INCLUDED
