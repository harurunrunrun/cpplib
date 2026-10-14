#ifndef CPPLIB_SRC_STRUCTURE_BBST_SEQUENCE_PERSISTENT_REVERSIBLE_SPLAY_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_SEQUENCE_PERSISTENT_REVERSIBLE_SPLAY_TREE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include "../../array/static_or_vector.hpp"
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<auto Monoid, int MAX_NODE = -1, int MAX_VERSION = -1>
struct PersistentReversibleSplayTree{
    static_assert(MAX_NODE == -1 || MAX_NODE > 0);
    static_assert(MAX_VERSION == -1 || MAX_VERSION > 0);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

private:
    struct Node{
        int left = -1;
        int right = -1;
        int size = 0;
        bool reversed = false;
        S value{};
        S aggregate{};
        S rev_aggregate{};
    };

    cpplib_detail::StaticOrVector<Node, (MAX_NODE < 0 ? -1 : static_cast<std::ptrdiff_t>(MAX_NODE))> nodes{};
    cpplib_detail::StaticOrVector<int, (MAX_VERSION < 0 ? -1 : static_cast<std::ptrdiff_t>(MAX_VERSION))> roots{};
    int used = 0;
    int version_count = 1;

    int size_or_zero(int node) const{
        return node == -1 ? 0 : nodes[static_cast<std::size_t>(node)].size;
    }

    S aggregate_or_e(int node) const{
        return node == -1 ? Monoid.e() : nodes[static_cast<std::size_t>(node)].aggregate;
    }

    S rev_aggregate_or_e(int node) const{
        return node == -1 ? Monoid.e() : nodes[static_cast<std::size_t>(node)].rev_aggregate;
    }

    void pull(int node){
        const auto current_node_index_1 = static_cast<std::size_t>(node);
        nodes[current_node_index_1].size = 1 + size_or_zero(nodes[current_node_index_1].left) + size_or_zero(nodes[current_node_index_1].right);
        nodes[current_node_index_1].aggregate = Monoid.op(
            aggregate_or_e(nodes[current_node_index_1].left),
            Monoid.op(nodes[current_node_index_1].value, aggregate_or_e(nodes[current_node_index_1].right))
        );
        nodes[current_node_index_1].rev_aggregate = Monoid.op(
            rev_aggregate_or_e(nodes[current_node_index_1].right),
            Monoid.op(nodes[current_node_index_1].value, rev_aggregate_or_e(nodes[current_node_index_1].left))
        );
    }

    int copy_node(int node){
        if constexpr(MAX_NODE < 0) nodes.ensure(cpplib_detail::checked_int_size(static_cast<std::size_t>(used) + 1));
        if(node == -1) return -1;
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentReversibleSplayTree).");
        }
        nodes[static_cast<std::size_t>(used)] = nodes[static_cast<std::size_t>(node)];
        return used++;
    }

    int new_node(const S& value){
        if constexpr(MAX_NODE < 0) nodes.ensure(cpplib_detail::checked_int_size(static_cast<std::size_t>(used) + 1));
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentReversibleSplayTree).");
        }
        int node = used++;
        nodes[static_cast<std::size_t>(node)] = {-1, -1, 1, false, value, value, value};
        return node;
    }

    void toggle(int node){
        if(node == -1) return;
        const auto current_node_index_2 = static_cast<std::size_t>(node);
        std::swap(nodes[current_node_index_2].left, nodes[current_node_index_2].right);
        std::swap(nodes[current_node_index_2].aggregate, nodes[current_node_index_2].rev_aggregate);
        nodes[current_node_index_2].reversed = !nodes[current_node_index_2].reversed;
    }

    // Returns a writable copy whose reversal flag has been propagated.
    int expose(int root){
        int node = copy_node(root);
        const auto current_node_index_3 = static_cast<std::size_t>(node);
        if(!nodes[current_node_index_3].reversed) return node;
        if(nodes[current_node_index_3].left != -1){
            nodes[current_node_index_3].left = copy_node(nodes[current_node_index_3].left);
            toggle(nodes[current_node_index_3].left);
        }
        if(nodes[current_node_index_3].right != -1){
            nodes[current_node_index_3].right = copy_node(nodes[current_node_index_3].right);
            toggle(nodes[current_node_index_3].right);
        }
        nodes[current_node_index_3].reversed = false;
        return node;
    }

    int rotate_right(int root){
        int child = nodes[static_cast<std::size_t>(root)].left;
        nodes[static_cast<std::size_t>(root)].left = nodes[static_cast<std::size_t>(child)].right;
        nodes[static_cast<std::size_t>(child)].right = root;
        pull(root);
        pull(child);
        return child;
    }

    int rotate_left(int root){
        int child = nodes[static_cast<std::size_t>(root)].right;
        nodes[static_cast<std::size_t>(root)].right = nodes[static_cast<std::size_t>(child)].left;
        nodes[static_cast<std::size_t>(child)].left = root;
        pull(root);
        pull(child);
        return child;
    }

    // Functional bottom-up splay by implicit rank. Every rotated node is a copy.
    int splay(int root, int k){
        int node = expose(root);
        int left_size = size_or_zero(nodes[static_cast<std::size_t>(node)].left);
        if(k < left_size){
            int left = expose(nodes[static_cast<std::size_t>(node)].left);
            nodes[static_cast<std::size_t>(node)].left = left;
            int left_left_size = size_or_zero(nodes[static_cast<std::size_t>(left)].left);
            if(k < left_left_size){
                nodes[static_cast<std::size_t>(left)].left =
                    splay(nodes[static_cast<std::size_t>(left)].left, k);
                node = rotate_right(node);
            }else if(k > left_left_size){
                nodes[static_cast<std::size_t>(left)].right =
                    splay(nodes[static_cast<std::size_t>(left)].right, k - left_left_size - 1);
                nodes[static_cast<std::size_t>(node)].left = rotate_left(left);
            }
            return nodes[static_cast<std::size_t>(node)].left == -1 ? node : rotate_right(node);
        }
        if(k > left_size){
            int right = expose(nodes[static_cast<std::size_t>(node)].right);
            nodes[static_cast<std::size_t>(node)].right = right;
            int rank = k - left_size - 1;
            int right_left_size = size_or_zero(nodes[static_cast<std::size_t>(right)].left);
            if(rank < right_left_size){
                nodes[static_cast<std::size_t>(right)].left =
                    splay(nodes[static_cast<std::size_t>(right)].left, rank);
                nodes[static_cast<std::size_t>(node)].right = rotate_right(right);
            }else if(rank > right_left_size){
                nodes[static_cast<std::size_t>(right)].right =
                    splay(nodes[static_cast<std::size_t>(right)].right, rank - right_left_size - 1);
                node = rotate_left(node);
            }
            return nodes[static_cast<std::size_t>(node)].right == -1 ? node : rotate_left(node);
        }
        return node;
    }

    std::pair<int, int> split(int root, int k){
        if(root == -1) return {-1, -1};
        if(k == 0) return {-1, root};
        if(k == size_or_zero(root)) return {root, -1};
        int right = splay(root, k);
        int left = nodes[static_cast<std::size_t>(right)].left;
        nodes[static_cast<std::size_t>(right)].left = -1;
        pull(right);
        return {left, right};
    }

    int merge(int left, int right){
        if(left == -1) return right;
        if(right == -1) return left;
        left = splay(left, size_or_zero(left) - 1);
        nodes[static_cast<std::size_t>(left)].right = right;
        pull(left);
        return left;
    }

    int build(const std::vector<S>& values, int l, int r){
        if(l == r) return -1;
        int m = (l + r) >> 1;
        int node = new_node(values[static_cast<std::size_t>(m)]);
        nodes[static_cast<std::size_t>(node)].left = build(values, l, m);
        nodes[static_cast<std::size_t>(node)].right = build(values, m + 1, r);
        pull(node);
        return node;
    }

    void check_version(int version) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PersistentReversibleSplayTree).");
        }
    }

    void check_insert_position(int version, int p) const{
        check_version(version);
        if(p < 0 || size_or_zero(roots[static_cast<std::size_t>(version)]) < p)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PersistentReversibleSplayTree).");
        }
    }

    void check_position(int version, int p) const{
        check_version(version);
        if(p < 0 || size_or_zero(roots[static_cast<std::size_t>(version)]) <= p)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PersistentReversibleSplayTree).");
        }
    }

    void check_range(int version, int l, int r) const{
        check_version(version);
        if(l < 0 || r < l || size_or_zero(roots[static_cast<std::size_t>(version)]) < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PersistentReversibleSplayTree).");
        }
    }

    template<class F>
    int make_version(F&& operation){
        if(version_count == MAX_VERSION)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (PersistentReversibleSplayTree).");
        }
        int checkpoint = used;
        try{
            int root = operation();
            if constexpr(MAX_VERSION < 0) roots.ensure(static_cast<std::size_t>(version_count) + 1);
            roots[static_cast<std::size_t>(version_count)] = root;
            return version_count++;
        }catch(...){
            used = checkpoint;
            throw;
        }
    }

    S get_rec(int node, int k, bool flip) const{
        const auto current_node_index_4 = static_cast<std::size_t>(node);
        int left = flip ? nodes[current_node_index_4].right : nodes[current_node_index_4].left;
        int right = flip ? nodes[current_node_index_4].left : nodes[current_node_index_4].right;
        bool child_flip = flip ^ nodes[current_node_index_4].reversed;
        int left_size = size_or_zero(left);
        if(k < left_size) return get_rec(left, k, child_flip);
        if(k == left_size) return nodes[current_node_index_4].value;
        return get_rec(right, k - left_size - 1, child_flip);
    }

    S prod_rec(int node, int l, int r, bool flip) const{
        if(l == r) return Monoid.e();
        int node_size = size_or_zero(node);
        if(l == 0 && r == node_size){
            return flip ? rev_aggregate_or_e(node) : aggregate_or_e(node);
        }
        const auto current_node_index_5 = static_cast<std::size_t>(node);
        int left = flip ? nodes[current_node_index_5].right : nodes[current_node_index_5].left;
        int right = flip ? nodes[current_node_index_5].left : nodes[current_node_index_5].right;
        bool child_flip = flip ^ nodes[current_node_index_5].reversed;
        int left_size = size_or_zero(left);
        S result = Monoid.e();
        if(l < left_size){
            result = prod_rec(left, l, std::min(r, left_size), child_flip);
        }
        if(l <= left_size && left_size < r){
            result = Monoid.op(result, nodes[current_node_index_5].value);
        }
        if(left_size + 1 < r){
            result = Monoid.op(
                result,
                prod_rec(right, std::max(0, l - left_size - 1), r - left_size - 1, child_flip)
            );
        }
        return result;
    }

    void collect(int node, bool flip, std::vector<S>& result) const{
        if(node == -1) return;
        const auto current_node_index_6 = static_cast<std::size_t>(node);
        int left = flip ? nodes[current_node_index_6].right : nodes[current_node_index_6].left;
        int right = flip ? nodes[current_node_index_6].left : nodes[current_node_index_6].right;
        bool child_flip = flip ^ nodes[current_node_index_6].reversed;
        collect(left, child_flip, result);
        result.push_back(nodes[current_node_index_6].value);
        collect(right, child_flip, result);
    }

public:
    PersistentReversibleSplayTree(){
        roots.ensure(1); roots.fill(-1);
    }

    explicit PersistentReversibleSplayTree(const std::vector<S>& values) : PersistentReversibleSplayTree(){
        if(values.size() > static_cast<std::size_t>(MAX_NODE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentReversibleSplayTree).");
        }
        roots[0] = build(values, 0, cpplib_detail::checked_int_size(values.size()));
    }

    int versions() const{
        return version_count;
    }

    int size(int version = 0) const{
        check_version(version);
        return size_or_zero(roots[static_cast<std::size_t>(version)]);
    }

    bool empty(int version = 0) const{
        return size(version) == 0;
    }

    int insert(int p, const S& value, int version = 0){
        check_insert_position(version, p);
        return make_version([&]{
            auto [left, right] = split(roots[static_cast<std::size_t>(version)], p);
            int middle = new_node(value);
            return merge(merge(left, middle), right);
        });
    }

    int push_front(const S& value, int version = 0){
        return insert(0, value, version);
    }

    int push_back(const S& value, int version = 0){
        check_version(version);
        return insert(size_or_zero(roots[static_cast<std::size_t>(version)]), value, version);
    }

    int erase(int p, int version = 0){
        check_position(version, p);
        return make_version([&]{
            auto [left, right] = split(roots[static_cast<std::size_t>(version)], p);
            auto [middle, tail] = split(right, 1);
            (void)middle;
            return merge(left, tail);
        });
    }

    int set(int p, const S& value, int version = 0){
        check_position(version, p);
        return make_version([&]{
            auto [left, right] = split(roots[static_cast<std::size_t>(version)], p);
            auto [middle, tail] = split(right, 1);
            middle = copy_node(middle);
            const auto current_node_index_7 = static_cast<std::size_t>(middle);
            nodes[current_node_index_7].value = value;
            nodes[current_node_index_7].aggregate = value;
            nodes[current_node_index_7].rev_aggregate = value;
            nodes[current_node_index_7].reversed = false;
            return merge(merge(left, middle), tail);
        });
    }

    S get(int p, int version = 0) const{
        check_position(version, p);
        return get_rec(roots[static_cast<std::size_t>(version)], p, false);
    }

    int reverse(int l, int r, int version = 0){
        check_range(version, l, r);
        return make_version([&]{
            if(l == r) return roots[static_cast<std::size_t>(version)];
            auto [left, right] = split(roots[static_cast<std::size_t>(version)], l);
            auto [middle, tail] = split(right, r - l);
            middle = copy_node(middle);
            toggle(middle);
            return merge(merge(left, middle), tail);
        });
    }

    S prod(int l, int r, int version = 0) const{
        check_range(version, l, r);
        return prod_rec(roots[static_cast<std::size_t>(version)], l, r, false);
    }

    S all_prod(int version = 0) const{
        check_version(version);
        return aggregate_or_e(roots[static_cast<std::size_t>(version)]);
    }

    std::vector<S> to_vector(int version = 0) const{
        check_version(version);
        std::vector<S> result;
        result.reserve(static_cast<std::size_t>(size_or_zero(roots[static_cast<std::size_t>(version)])));
        collect(roots[static_cast<std::size_t>(version)], false, result);
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_BBST_SEQUENCE_PERSISTENT_REVERSIBLE_SPLAY_TREE_HPP_INCLUDED
