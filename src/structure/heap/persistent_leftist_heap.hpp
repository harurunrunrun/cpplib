#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>

template<
    class T,
    std::size_t MAX_VERSION,
    std::size_t MAX_NODE,
    class Compare = std::less<T>
>
struct PersistentLeftistHeap{
    static_assert(MAX_VERSION > 0);
    static_assert(MAX_NODE > 0);

private:
    struct Node{
        T value{};
        int left = 0;
        int right = 0;
        int rank = 0;
    };

    static_assert(
        MAX_NODE <= static_cast<std::size_t>(std::numeric_limits<int>::max())
    );

    std::array<Node, MAX_NODE> nodes{};
    std::array<int, MAX_VERSION> roots{};
    std::array<std::size_t, MAX_VERSION> sizes{};
    std::size_t node_count = 1;
    std::size_t version_count = 1;
    [[no_unique_address]] Compare compare{};

    void check_version(std::size_t version) const{
        if(version >= version_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (persistent leftist heap version)."
            );
        }
    }

    void check_new_version() const{
        if(version_count == MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: version capacity exceeded "
                "(persistent leftist heap)."
            );
        }
    }

    int copy_node(int source){
        if(node_count == MAX_NODE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: node capacity exceeded "
                "(persistent leftist heap)."
            );
        }
        nodes[node_count] = nodes[static_cast<std::size_t>(source)];
        return static_cast<int>(node_count++);
    }

    int make_node(const T& value){
        if(node_count == MAX_NODE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: node capacity exceeded "
                "(persistent leftist heap)."
            );
        }
        nodes[node_count] = Node{value, 0, 0, 1};
        return static_cast<int>(node_count++);
    }

    int meld_roots(int left, int right){
        if(left == 0) return right;
        if(right == 0) return left;
        if(compare(
            nodes[static_cast<std::size_t>(right)].value,
            nodes[static_cast<std::size_t>(left)].value
        )){
            std::swap(left, right);
        }

        const int result = copy_node(left);
        Node& node = nodes[static_cast<std::size_t>(result)];
        node.right = meld_roots(node.right, right);
        if(
            nodes[static_cast<std::size_t>(node.left)].rank <
            nodes[static_cast<std::size_t>(node.right)].rank
        ){
            std::swap(node.left, node.right);
        }
        node.rank = nodes[static_cast<std::size_t>(node.right)].rank + 1;
        return result;
    }

    std::size_t add_version(int root, std::size_t size){
        roots[version_count] = root;
        sizes[version_count] = size;
        return version_count++;
    }

public:
    PersistentLeftistHeap() = default;

    explicit PersistentLeftistHeap(Compare compare_)
        : compare(std::move(compare_)){}

    std::size_t versions() const noexcept{ return version_count; }
    std::size_t nodes_used() const noexcept{ return node_count - 1; }

    std::size_t size(std::size_t version) const{
        check_version(version);
        return sizes[version];
    }

    bool empty(std::size_t version) const{
        check_version(version);
        return roots[version] == 0;
    }

    const T& top(std::size_t version) const{
        check_version(version);
        const int root = roots[version];
        if(root == 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty heap (persistent leftist heap top)."
            );
        }
        return nodes[static_cast<std::size_t>(root)].value;
    }

    std::size_t fork(std::size_t version){
        check_version(version);
        check_new_version();
        return add_version(roots[version], sizes[version]);
    }

    std::size_t push(std::size_t version, const T& value){
        check_version(version);
        check_new_version();
        if(sizes[version] == std::numeric_limits<std::size_t>::max())[[unlikely]]{
            throw std::overflow_error(
                "persistent leftist heap logical size overflow."
            );
        }
        const std::size_t snapshot = node_count;
        try{
            const int singleton = make_node(value);
            const int root = meld_roots(roots[version], singleton);
            return add_version(root, sizes[version] + 1);
        }catch(...){
            node_count = snapshot;
            throw;
        }
    }

    std::size_t pop(std::size_t version){
        check_version(version);
        check_new_version();
        const int root = roots[version];
        if(root == 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty heap (persistent leftist heap pop)."
            );
        }
        const std::size_t snapshot = node_count;
        try{
            const Node& node = nodes[static_cast<std::size_t>(root)];
            const int new_root = meld_roots(node.left, node.right);
            return add_version(new_root, sizes[version] - 1);
        }catch(...){
            node_count = snapshot;
            throw;
        }
    }

    std::size_t meld(std::size_t left_version, std::size_t right_version){
        check_version(left_version);
        check_version(right_version);
        check_new_version();
        if(
            sizes[left_version] >
            std::numeric_limits<std::size_t>::max() - sizes[right_version]
        )[[unlikely]]{
            throw std::overflow_error(
                "persistent leftist heap logical size overflow."
            );
        }
        const std::size_t snapshot = node_count;
        try{
            const int root = meld_roots(
                roots[left_version], roots[right_version]
            );
            return add_version(
                root, sizes[left_version] + sizes[right_version]
            );
        }catch(...){
            node_count = snapshot;
            throw;
        }
    }
};
