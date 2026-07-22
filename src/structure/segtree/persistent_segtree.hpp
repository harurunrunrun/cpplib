#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_SEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_SEGTREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include "../types/monoid.hpp"

template<auto Monoid, int MAX_SIZE, int MAX_VERSION>
struct PersistentSegtree{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_VERSION >= 0);
    static_assert(MAX_VERSION < std::numeric_limits<int>::max());

    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

    static constexpr int sz = []{
        int res = 1;
        while(res < MAX_SIZE){
            res <<= 1;
        }
        return res;
    }();

    static constexpr int height = []{
        int res = 1;
        int h = 0;
        while(res < MAX_SIZE){
            res <<= 1;
            h++;
        }
        return h;
    }();

private:
    static constexpr std::size_t nodes_per_update =
        static_cast<std::size_t>(height) + 1;

    static_assert(
        static_cast<std::size_t>(MAX_VERSION) <=
        std::numeric_limits<std::size_t>::max() / nodes_per_update
    );

    static constexpr std::size_t node_capacity =
        static_cast<std::size_t>(MAX_VERSION) * nodes_per_update;

    static_assert(
        node_capacity <= static_cast<std::size_t>(std::numeric_limits<int>::max())
    );

    struct Node{
        int left;
        int right;
    };

    int _n = 0;
    int version_count = 1;
    int node_count = 0;

    std::array<std::optional<S>, 2 * sz> initial;
    std::array<int, static_cast<std::size_t>(MAX_VERSION) + 1> root;
    std::array<Node, node_capacity> nodes;
    std::array<std::optional<S>, node_capacity> values;

    void initialize(int n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }

        _n = n;
        root.fill(-1);

        for(int k = 0; k < 2 * sz; k++){
            initial[k].emplace(Monoid.e());
        }
    }

    void build(){
        for(int k = sz - 1; k > 0; k--){
            S value = Monoid.op(*initial[2 * k], *initial[2 * k + 1]);
            initial[k].reset();
            initial[k].emplace(value);
        }
    }

    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    const S& value_at(int node, int tree_index) const{
        if(node == -1){
            return *initial[tree_index];
        }
        return *values[node];
    }

    int left_child(int node) const{
        return node == -1 ? -1 : nodes[node].left;
    }

    int right_child(int node) const{
        return node == -1 ? -1 : nodes[node].right;
    }

    int new_node(int source, int tree_index){
        if(static_cast<std::size_t>(node_count) >= node_capacity)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: node capacity exceeded (set)."
            );
        }

        int res = node_count++;
        if(source == -1){
            nodes[res] = {-1, -1};
        }else{
            nodes[res] = nodes[source];
        }
        values[res].emplace(value_at(source, tree_index));
        return res;
    }

    int set_impl(
        int source,
        int tree_index,
        int l,
        int r,
        int k,
        const S& x
    ){
        int current = new_node(source, tree_index);

        if(r - l == 1){
            *values[current] = x;
            return current;
        }

        int mid = l + (r - l) / 2;
        if(k < mid){
            nodes[current].left = set_impl(
                left_child(source), 2 * tree_index, l, mid, k, x
            );
        }else{
            nodes[current].right = set_impl(
                right_child(source), 2 * tree_index + 1, mid, r, k, x
            );
        }

        S merged = Monoid.op(
            value_at(nodes[current].left, 2 * tree_index),
            value_at(nodes[current].right, 2 * tree_index + 1)
        );
        *values[current] = merged;
        return current;
    }

    S prod_impl(
        int node,
        int tree_index,
        int l,
        int r,
        int ql,
        int qr
    ) const{
        if(r <= ql || qr <= l){
            return Monoid.e();
        }
        if(ql <= l && r <= qr){
            return value_at(node, tree_index);
        }

        int mid = l + (r - l) / 2;
        return Monoid.op(
            prod_impl(left_child(node), 2 * tree_index, l, mid, ql, qr),
            prod_impl(right_child(node), 2 * tree_index + 1, mid, r, ql, qr)
        );
    }

    template<class F, class... Args>
    int max_right_impl(
        int node,
        int tree_index,
        int l,
        int r,
        int ql,
        S& sm,
        F& f,
        Args&... args
    ) const{
        if(r <= ql || _n <= l){
            return -1;
        }

        if(ql <= l && r <= _n){
            S next = Monoid.op(sm, value_at(node, tree_index));
            if(std::invoke(f, next, args...)){
                sm = next;
                return -1;
            }
            if(r - l == 1){
                return l;
            }
        }

        int mid = l + (r - l) / 2;
        int res = max_right_impl(
            left_child(node), 2 * tree_index, l, mid, ql, sm, f, args...
        );
        if(res != -1){
            return res;
        }
        return max_right_impl(
            right_child(node), 2 * tree_index + 1, mid, r, ql, sm, f, args...
        );
    }

    template<class F, class... Args>
    int min_left_impl(
        int node,
        int tree_index,
        int l,
        int r,
        int qr,
        S& sm,
        F& f,
        Args&... args
    ) const{
        if(qr <= l || _n <= l){
            return -1;
        }

        if(r <= qr && r <= _n){
            S next = Monoid.op(value_at(node, tree_index), sm);
            if(std::invoke(f, next, args...)){
                sm = next;
                return -1;
            }
            if(r - l == 1){
                return r;
            }
        }

        int mid = l + (r - l) / 2;
        int res = min_left_impl(
            right_child(node), 2 * tree_index + 1, mid, r, qr, sm, f, args...
        );
        if(res != -1){
            return res;
        }
        return min_left_impl(
            left_child(node), 2 * tree_index, l, mid, qr, sm, f, args...
        );
    }

    void reset_nodes(int snapshot){
        while(snapshot < node_count){
            --node_count;
            values[node_count].reset();
        }
    }

public:
    PersistentSegtree(const PersistentSegtree&) = delete;
    PersistentSegtree& operator=(const PersistentSegtree&) = delete;
    PersistentSegtree(PersistentSegtree&&) = delete;
    PersistentSegtree& operator=(PersistentSegtree&&) = delete;

    explicit PersistentSegtree(int n = MAX_SIZE){
        initialize(n);
        build();
    }

    explicit PersistentSegtree(const std::vector<S>& v){
        if(v.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }

        initialize(static_cast<int>(v.size()));
        for(int k = 0; k < _n; k++){
            initial[k + sz].reset();
            initial[k + sz].emplace(v[static_cast<std::size_t>(k)]);
        }
        build();
    }

    template<std::size_t N>
    explicit PersistentSegtree(const std::array<S, N>& v){
        if(N > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }

        initialize(static_cast<int>(N));
        for(int k = 0; k < _n; k++){
            initial[k + sz].reset();
            initial[k + sz].emplace(v[static_cast<std::size_t>(k)]);
        }
        build();
    }

    int size() const{
        return _n;
    }

    int versions() const{
        return version_count;
    }

    int latest_version() const{
        return version_count - 1;
    }

    int set(int version, int k, const S& x){
        check_version(
            version,
            "library assertion fault: range violation (set)."
        );
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (set)."
            );
        }
        if(version_count > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: version capacity exceeded (set)."
            );
        }

        int snapshot = node_count;
        int new_root;
        try{
            new_root = set_impl(root[version], 1, 0, sz, k, x);
        }catch(...){
            reset_nodes(snapshot);
            throw;
        }

        int new_version = version_count++;
        root[new_version] = new_root;
        return new_version;
    }

    int fork(int version){
        check_version(
            version,
            "library assertion fault: range violation (fork)."
        );
        if(version_count > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: version capacity exceeded (fork)."
            );
        }

        int new_version = version_count++;
        root[new_version] = root[version];
        return new_version;
    }

    S get(int version, int k) const{
        check_version(
            version,
            "library assertion fault: range violation (get)."
        );
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (get)."
            );
        }

        int node = root[version];
        int tree_index = 1;
        int l = 0;
        int r = sz;
        while(r - l > 1){
            int mid = l + (r - l) / 2;
            if(k < mid){
                node = left_child(node);
                tree_index *= 2;
                r = mid;
            }else{
                node = right_child(node);
                tree_index = 2 * tree_index + 1;
                l = mid;
            }
        }
        return value_at(node, tree_index);
    }

    S prod(int version, int l, int r) const{
        check_version(
            version,
            "library assertion fault: range violation (prod)."
        );
        if(l < 0 || r < l || _n < r)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (prod)."
            );
        }
        return prod_impl(root[version], 1, 0, sz, l, r);
    }

    S all_prod(int version) const{
        check_version(
            version,
            "library assertion fault: range violation (all_prod)."
        );
        return value_at(root[version], 1);
    }

    int max_right(int version, int l, auto f, auto&&... args) const{
        check_version(
            version,
            "library assertion fault: range violation (max_right)."
        );
        if(l < 0 || _n < l)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (max_right)."
            );
        }
        if(!std::invoke(f, Monoid.e(), args...))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: f(e) must be true (max_right)."
            );
        }
        if(l == _n){
            return _n;
        }

        S sm = Monoid.e();
        int res = max_right_impl(
            root[version], 1, 0, sz, l, sm, f, args...
        );
        return res == -1 ? _n : res;
    }

    int min_left(int version, int r, auto f, auto&&... args) const{
        check_version(
            version,
            "library assertion fault: range violation (min_left)."
        );
        if(r < 0 || _n < r)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (min_left)."
            );
        }
        if(!std::invoke(f, Monoid.e(), args...))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: f(e) must be true (min_left)."
            );
        }
        if(r == 0){
            return 0;
        }

        S sm = Monoid.e();
        int res = min_left_impl(
            root[version], 1, 0, sz, r, sm, f, args...
        );
        return res == -1 ? 0 : res;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_SEGTREE_HPP_INCLUDED
