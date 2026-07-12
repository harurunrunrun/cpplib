#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<auto MonoidActLen, int MAX_NODE, int MAX_VERSION>
struct PersistentLazyReversibleSplayTree{
    static_assert(MAX_NODE > 0);
    static_assert(MAX_VERSION > 0);
    using S = typename std::remove_cvref_t<decltype(MonoidActLen)>::S;
    using T = typename std::remove_cvref_t<decltype(MonoidActLen)>::T;

private:
    struct Node{
        int left = -1;
        int right = -1;
        int size = 0;
        bool reversed = false;
        bool has_lazy = false;
        S value{};
        S aggregate{};
        S rev_aggregate{};
        T lazy{};
    };

    struct QueryResult{
        S value;
        int length;
    };

    std::array<Node, MAX_NODE> nodes{};
    std::array<int, MAX_VERSION> roots{};
    int used = 0;
    int version_count = 1;

    int size_or_zero(int node) const{
        return node == -1 ? 0 : nodes[static_cast<std::size_t>(node)].size;
    }

    S aggregate_or_e(int node) const{
        return node == -1 ? MonoidActLen.e() : nodes[static_cast<std::size_t>(node)].aggregate;
    }

    S rev_aggregate_or_e(int node) const{
        return node == -1 ? MonoidActLen.e() : nodes[static_cast<std::size_t>(node)].rev_aggregate;
    }

    void pull(int node){
        auto& current = nodes[static_cast<std::size_t>(node)];
        int left_size = size_or_zero(current.left);
        int right_size = size_or_zero(current.right);
        current.size = left_size + 1 + right_size;
        S prefix = MonoidActLen.op(aggregate_or_e(current.left), left_size, current.value, 1);
        current.aggregate = MonoidActLen.op(prefix, left_size + 1, aggregate_or_e(current.right), right_size);
        S rev_prefix = MonoidActLen.op(rev_aggregate_or_e(current.right), right_size, current.value, 1);
        current.rev_aggregate = MonoidActLen.op(rev_prefix, right_size + 1, rev_aggregate_or_e(current.left), left_size);
    }

    int copy_node(int node){
        if(node == -1) return -1;
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentLazyReversibleSplayTree).");
        }
        nodes[static_cast<std::size_t>(used)] = nodes[static_cast<std::size_t>(node)];
        return used++;
    }

    int new_node(const S& value){
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentLazyReversibleSplayTree).");
        }
        int node = used++;
        nodes[static_cast<std::size_t>(node)] = {
            -1, -1, 1, false, false, value, value, value, MonoidActLen.id()
        };
        return node;
    }

    void all_apply(int node, const T& f){
        if(node == -1) return;
        auto& current = nodes[static_cast<std::size_t>(node)];
        current.value = MonoidActLen.mapping(f, current.value, 1);
        current.aggregate = MonoidActLen.mapping(f, current.aggregate, current.size);
        current.rev_aggregate = MonoidActLen.mapping(f, current.rev_aggregate, current.size);
        if(current.has_lazy){
            current.lazy = MonoidActLen.composition(f, current.lazy);
        }else{
            current.lazy = f;
            current.has_lazy = true;
        }
    }

    void toggle(int node){
        if(node == -1) return;
        auto& current = nodes[static_cast<std::size_t>(node)];
        std::swap(current.left, current.right);
        std::swap(current.aggregate, current.rev_aggregate);
        current.reversed = !current.reversed;
    }

    // Returns a writable copy whose reversal and action have been propagated.
    int expose(int root){
        int node = copy_node(root);
        auto& current = nodes[static_cast<std::size_t>(node)];
        if(!current.reversed && !current.has_lazy) return node;
        if(current.left != -1){
            current.left = copy_node(current.left);
            if(current.reversed) toggle(current.left);
            if(current.has_lazy) all_apply(current.left, current.lazy);
        }
        if(current.right != -1){
            current.right = copy_node(current.right);
            if(current.reversed) toggle(current.right);
            if(current.has_lazy) all_apply(current.right, current.lazy);
        }
        current.reversed = false;
        current.has_lazy = false;
        current.lazy = MonoidActLen.id();
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
            throw std::runtime_error("library assertion fault: range violation (PersistentLazyReversibleSplayTree).");
        }
    }

    void check_insert_position(int version, int p) const{
        check_version(version);
        if(p < 0 || size_or_zero(roots[static_cast<std::size_t>(version)]) < p)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PersistentLazyReversibleSplayTree).");
        }
    }

    void check_position(int version, int p) const{
        check_version(version);
        if(p < 0 || size_or_zero(roots[static_cast<std::size_t>(version)]) <= p)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PersistentLazyReversibleSplayTree).");
        }
    }

    void check_range(int version, int l, int r) const{
        check_version(version);
        if(l < 0 || r < l || size_or_zero(roots[static_cast<std::size_t>(version)]) < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PersistentLazyReversibleSplayTree).");
        }
    }

    template<class F>
    int make_version(F&& operation){
        if(version_count == MAX_VERSION)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (PersistentLazyReversibleSplayTree).");
        }
        int checkpoint = used;
        try{
            int root = operation();
            roots[static_cast<std::size_t>(version_count)] = root;
            return version_count++;
        }catch(...){
            used = checkpoint;
            throw;
        }
    }

    QueryResult combine(const QueryResult& left, const QueryResult& right) const{
        if(left.length == 0) return right;
        if(right.length == 0) return left;
        return {
            MonoidActLen.op(left.value, left.length, right.value, right.length),
            left.length + right.length
        };
    }

    T child_action(const T& action, const Node& current) const{
        return current.has_lazy ? MonoidActLen.composition(action, current.lazy) : action;
    }

    S get_rec(int node, int k, bool flip, const T& action) const{
        const auto& current = nodes[static_cast<std::size_t>(node)];
        int left = flip ? current.right : current.left;
        int right = flip ? current.left : current.right;
        bool child_flip = flip ^ current.reversed;
        T next_action = child_action(action, current);
        int left_size = size_or_zero(left);
        if(k < left_size) return get_rec(left, k, child_flip, next_action);
        if(k == left_size) return MonoidActLen.mapping(action, current.value, 1);
        return get_rec(right, k - left_size - 1, child_flip, next_action);
    }

    QueryResult prod_rec(int node, int l, int r, bool flip, const T& action) const{
        if(l == r) return {MonoidActLen.e(), 0};
        int node_size = size_or_zero(node);
        if(l == 0 && r == node_size){
            S value = flip ? rev_aggregate_or_e(node) : aggregate_or_e(node);
            return {MonoidActLen.mapping(action, value, node_size), node_size};
        }
        const auto& current = nodes[static_cast<std::size_t>(node)];
        int left = flip ? current.right : current.left;
        int right = flip ? current.left : current.right;
        bool child_flip = flip ^ current.reversed;
        T next_action = child_action(action, current);
        int left_size = size_or_zero(left);
        QueryResult result{MonoidActLen.e(), 0};
        if(l < left_size){
            result = prod_rec(left, l, std::min(r, left_size), child_flip, next_action);
        }
        if(l <= left_size && left_size < r){
            result = combine(result, {MonoidActLen.mapping(action, current.value, 1), 1});
        }
        if(left_size + 1 < r){
            result = combine(
                result,
                prod_rec(
                    right,
                    std::max(0, l - left_size - 1),
                    r - left_size - 1,
                    child_flip,
                    next_action
                )
            );
        }
        return result;
    }

    void collect(int node, bool flip, const T& action, std::vector<S>& result) const{
        if(node == -1) return;
        const auto& current = nodes[static_cast<std::size_t>(node)];
        int left = flip ? current.right : current.left;
        int right = flip ? current.left : current.right;
        bool child_flip = flip ^ current.reversed;
        T next_action = child_action(action, current);
        collect(left, child_flip, next_action, result);
        result.push_back(MonoidActLen.mapping(action, current.value, 1));
        collect(right, child_flip, next_action, result);
    }

public:
    PersistentLazyReversibleSplayTree(){
        roots.fill(-1);
    }

    explicit PersistentLazyReversibleSplayTree(const std::vector<S>& values) : PersistentLazyReversibleSplayTree(){
        if(values.size() > static_cast<std::size_t>(MAX_NODE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentLazyReversibleSplayTree).");
        }
        roots[0] = build(values, 0, static_cast<int>(values.size()));
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
            auto& current = nodes[static_cast<std::size_t>(middle)];
            current.value = value;
            current.aggregate = value;
            current.rev_aggregate = value;
            current.reversed = false;
            current.has_lazy = false;
            current.lazy = MonoidActLen.id();
            return merge(merge(left, middle), tail);
        });
    }

    int apply(int l, int r, const T& f, int version = 0){
        check_range(version, l, r);
        return make_version([&]{
            if(l == r) return roots[static_cast<std::size_t>(version)];
            auto [left, right] = split(roots[static_cast<std::size_t>(version)], l);
            auto [middle, tail] = split(right, r - l);
            middle = copy_node(middle);
            all_apply(middle, f);
            return merge(merge(left, middle), tail);
        });
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

    S get(int p, int version = 0) const{
        check_position(version, p);
        return get_rec(roots[static_cast<std::size_t>(version)], p, false, MonoidActLen.id());
    }

    S prod(int l, int r, int version = 0) const{
        check_range(version, l, r);
        return prod_rec(
            roots[static_cast<std::size_t>(version)], l, r, false, MonoidActLen.id()
        ).value;
    }

    S all_prod(int version = 0) const{
        check_version(version);
        return aggregate_or_e(roots[static_cast<std::size_t>(version)]);
    }

    std::vector<S> to_vector(int version = 0) const{
        check_version(version);
        std::vector<S> result;
        result.reserve(static_cast<std::size_t>(size_or_zero(roots[static_cast<std::size_t>(version)])));
        collect(roots[static_cast<std::size_t>(version)], false, MonoidActLen.id(), result);
        return result;
    }
};
