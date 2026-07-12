#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>

namespace wavelet_matrix_detail{

template<int MAX_BLOCK, int MAX_UPDATE>
struct PersistentBlockReference{
    static_assert(MAX_BLOCK >= 0);
    static_assert(MAX_UPDATE >= 0);

private:
    static constexpr int height = []{
        int value = MAX_BLOCK;
        int result = 0;
        while(value > 1){
            value = (value + 1) >> 1;
            result++;
        }
        return result;
    }();
    static constexpr int initial_node_capacity =
        MAX_BLOCK == 0 ? 0 : 2 * MAX_BLOCK - 1;
    static constexpr int node_capacity =
        initial_node_capacity + MAX_UPDATE * (height + 1) + 1;

    struct Node{
        int left = -1;
        int right = -1;
        int value = -1;
    };

    int _size = 0;
    int used = 0;
    std::array<Node, node_capacity> nodes{};

    int new_node(){
        if(used == node_capacity)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation (block reference)."
            );
        }
        return used++;
    }

    int clone_node(int source){
        int node = new_node();
        nodes[static_cast<std::size_t>(node)] =
            nodes[static_cast<std::size_t>(source)];
        return node;
    }

    int build_rec(const std::array<int, MAX_BLOCK>& values, int l, int r){
        int node = new_node();
        if(r - l == 1){
            nodes[static_cast<std::size_t>(node)].value =
                values[static_cast<std::size_t>(l)];
            return node;
        }
        int m = (l + r) >> 1;
        nodes[static_cast<std::size_t>(node)].left = build_rec(values, l, m);
        nodes[static_cast<std::size_t>(node)].right = build_rec(values, m, r);
        return node;
    }

    int set_rec(int source, int l, int r, int position, int value){
        int node = clone_node(source);
        if(r - l == 1){
            nodes[static_cast<std::size_t>(node)].value = value;
            return node;
        }
        int m = (l + r) >> 1;
        if(position < m){
            nodes[static_cast<std::size_t>(node)].left = set_rec(
                nodes[static_cast<std::size_t>(source)].left,
                l, m, position, value
            );
        }else{
            nodes[static_cast<std::size_t>(node)].right = set_rec(
                nodes[static_cast<std::size_t>(source)].right,
                m, r, position, value
            );
        }
        return node;
    }

public:
    int build(const std::array<int, MAX_BLOCK>& values, int size){
        if(size < 0 || MAX_BLOCK < size)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (block reference build)."
            );
        }
        _size = size;
        return size == 0 ? -1 : build_rec(values, 0, size);
    }

    int get(int root, int position) const{
        int l = 0;
        int r = _size;
        while(r - l > 1){
            int m = (l + r) >> 1;
            if(position < m){
                root = nodes[static_cast<std::size_t>(root)].left;
                r = m;
            }else{
                root = nodes[static_cast<std::size_t>(root)].right;
                l = m;
            }
        }
        return nodes[static_cast<std::size_t>(root)].value;
    }

    int set(int root, int position, int value){
        return set_rec(root, 0, _size, position, value);
    }

    int nodes_used() const{ return used; }

    void rollback(int snapshot){
        if(snapshot < 0 || used < snapshot)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (block reference rollback)."
            );
        }
        used = snapshot;
    }
};

} // namespace wavelet_matrix_detail
