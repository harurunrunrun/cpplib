#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

template<int MAX_SIZE>
struct BinaryTreeTraversalResult{
    static_assert(MAX_SIZE >= 0);

    bool valid = false;
    int node_count = 0;
    int root = -1;
    std::array<int, static_cast<std::size_t>(MAX_SIZE)> left_child;
    std::array<int, static_cast<std::size_t>(MAX_SIZE)> right_child;

    BinaryTreeTraversalResult() noexcept{}

    BinaryTreeTraversalResult(const BinaryTreeTraversalResult& other) noexcept:
        valid(other.valid),
        node_count(other.node_count),
        root(other.root){
        std::copy_n(other.left_child.begin(), node_count, left_child.begin());
        std::copy_n(other.right_child.begin(), node_count, right_child.begin());
    }

    BinaryTreeTraversalResult(BinaryTreeTraversalResult&& other) noexcept:
        BinaryTreeTraversalResult(
            static_cast<const BinaryTreeTraversalResult&>(other)
        ){}

    BinaryTreeTraversalResult& operator=(const BinaryTreeTraversalResult& other) noexcept{
        if(this == &other) return *this;
        valid = other.valid;
        node_count = other.node_count;
        root = other.root;
        std::copy_n(other.left_child.begin(), node_count, left_child.begin());
        std::copy_n(other.right_child.begin(), node_count, right_child.begin());
        return *this;
    }

    BinaryTreeTraversalResult& operator=(BinaryTreeTraversalResult&& other) noexcept{
        return *this = static_cast<const BinaryTreeTraversalResult&>(other);
    }

    int size() const noexcept{
        return node_count;
    }

    bool empty() const noexcept{
        return node_count == 0;
    }

    explicit operator bool() const noexcept{
        return valid;
    }
};

namespace binary_tree_traversal_reconstruction_detail{

struct Segment{
    int preorder_begin;
    int inorder_begin;
    int length;
    int parent;
    bool is_right_child;
};

}

template<
    int MAX_SIZE,
    class Key,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>
>
BinaryTreeTraversalResult<MAX_SIZE> reconstruct_binary_tree_traversals(
    const std::vector<Key>& preorder,
    const std::vector<Key>& inorder,
    const std::vector<Key>& postorder,
    Hash hash = Hash(),
    KeyEqual equal = KeyEqual()
){
    if(
        preorder.size() > static_cast<std::size_t>(MAX_SIZE)
        || inorder.size() > static_cast<std::size_t>(MAX_SIZE)
        || postorder.size() > static_cast<std::size_t>(MAX_SIZE)
    )[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: capacity exceeded "
            "(reconstruct_binary_tree_traversals)."
        );
    }

    BinaryTreeTraversalResult<MAX_SIZE> result;
    if(preorder.size() != inorder.size() || preorder.size() != postorder.size()){
        return result;
    }

    const int n = static_cast<int>(preorder.size());
    result.node_count = n;
    std::fill_n(result.left_child.begin(), n, -1);
    std::fill_n(result.right_child.begin(), n, -1);
    if(n == 0){
        result.valid = true;
        return result;
    }

    std::unordered_map<Key, int, Hash, KeyEqual> inorder_position(0, hash, equal);
    inorder_position.reserve(preorder.size());
    for(int index = 0; index < n; ++index){
        const auto [iterator, inserted] = inorder_position.emplace(
            inorder[static_cast<std::size_t>(index)],
            index
        );
        (void)iterator;
        if(!inserted) return result;
    }

    std::vector<unsigned char> seen_preorder(preorder.size(), 0);
    std::vector<unsigned char> seen_postorder(postorder.size(), 0);
    for(int index = 0; index < n; ++index){
        const auto preorder_iterator = inorder_position.find(
            preorder[static_cast<std::size_t>(index)]
        );
        const auto postorder_iterator = inorder_position.find(
            postorder[static_cast<std::size_t>(index)]
        );
        if(
            preorder_iterator == inorder_position.end()
            || postorder_iterator == inorder_position.end()
        ) return result;

        const std::size_t preorder_position = static_cast<std::size_t>(
            preorder_iterator->second
        );
        const std::size_t postorder_position = static_cast<std::size_t>(
            postorder_iterator->second
        );
        if(seen_preorder[preorder_position] != 0 || seen_postorder[postorder_position] != 0){
            return result;
        }
        seen_preorder[preorder_position] = 1;
        seen_postorder[postorder_position] = 1;
    }

    using binary_tree_traversal_reconstruction_detail::Segment;
    std::vector<Segment> stack;
    stack.reserve(preorder.size());
    stack.push_back({0, 0, n, -1, false});
    while(!stack.empty()){
        const Segment segment = stack.back();
        stack.pop_back();
        if(segment.length == 0) continue;

        const auto iterator = inorder_position.find(
            preorder[static_cast<std::size_t>(segment.preorder_begin)]
        );
        const int vertex = iterator->second;
        if(
            vertex < segment.inorder_begin
            || segment.inorder_begin + segment.length <= vertex
        ) return result;

        if(segment.parent == -1){
            result.root = vertex;
        }else if(segment.is_right_child){
            result.right_child[static_cast<std::size_t>(segment.parent)] = vertex;
        }else{
            result.left_child[static_cast<std::size_t>(segment.parent)] = vertex;
        }

        const int left_length = vertex - segment.inorder_begin;
        const int right_length = segment.length - left_length - 1;
        if(right_length != 0){
            stack.push_back({
                segment.preorder_begin + left_length + 1,
                vertex + 1,
                right_length,
                vertex,
                true
            });
        }
        if(left_length != 0){
            stack.push_back({
                segment.preorder_begin + 1,
                segment.inorder_begin,
                left_length,
                vertex,
                false
            });
        }
    }

    std::vector<std::pair<int, bool>> traversal_stack;
    traversal_stack.reserve(preorder.size());
    traversal_stack.emplace_back(result.root, false);
    int postorder_index = 0;
    while(!traversal_stack.empty()){
        const auto [vertex, expanded] = traversal_stack.back();
        traversal_stack.pop_back();
        if(expanded){
            if(
                postorder_index == n
                || !equal(
                    inorder[static_cast<std::size_t>(vertex)],
                    postorder[static_cast<std::size_t>(postorder_index)]
                )
            ) return result;
            ++postorder_index;
            continue;
        }

        traversal_stack.emplace_back(vertex, true);
        const int right = result.right_child[static_cast<std::size_t>(vertex)];
        const int left = result.left_child[static_cast<std::size_t>(vertex)];
        if(right != -1) traversal_stack.emplace_back(right, false);
        if(left != -1) traversal_stack.emplace_back(left, false);
    }

    if(postorder_index != n) return result;
    result.valid = true;
    return result;
}

template<
    int MAX_SIZE,
    class Key,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>
>
bool binary_tree_traversals_compatible(
    const std::vector<Key>& preorder,
    const std::vector<Key>& inorder,
    const std::vector<Key>& postorder,
    Hash hash = Hash(),
    KeyEqual equal = KeyEqual()
){
    return static_cast<bool>(reconstruct_binary_tree_traversals<MAX_SIZE>(
        preorder,
        inorder,
        postorder,
        std::move(hash),
        std::move(equal)
    ));
}
