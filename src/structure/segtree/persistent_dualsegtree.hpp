#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_DUALSEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_DUALSEGTREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include "../types/monoid_act.hpp"

template<auto Monoid_act, int MAX_SIZE, int MAX_VERSION>
struct PersistentDualSegtree{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_VERSION >= 0);

    using S = typename std::remove_cvref_t<decltype(Monoid_act)>::S;
    using T = typename std::remove_cvref_t<decltype(Monoid_act)>::T;

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
        static_cast<std::size_t>(4 * height) + 1;
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
    std::array<std::optional<S>, sz> initial;
    std::array<int, static_cast<std::size_t>(MAX_VERSION) + 1> root;
    std::array<Node, node_capacity> nodes;
    std::array<std::optional<T>, node_capacity> lazy;
    std::array<std::optional<S>, node_capacity> values;

    void initialize(int n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        _n = n;
        root.fill(-1);
        for(int k = 0; k < sz; k++){
            initial[k].emplace(Monoid_act.e());
        }
    }

    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    int left_child(int node) const{
        return node == -1 ? -1 : nodes[node].left;
    }

    int right_child(int node) const{
        return node == -1 ? -1 : nodes[node].right;
    }

    T action_at(int node) const{
        return node == -1 ? Monoid_act.id() : *lazy[node];
    }

    const S& value_at(int node, int index) const{
        return node == -1 ? *initial[index] : *values[node];
    }

    int clone_node(int source, bool leaf, int index){
        if(static_cast<std::size_t>(node_count) >= node_capacity)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: node capacity exceeded (update)."
            );
        }
        int res = node_count++;
        nodes[res] = source == -1 ? Node{-1, -1} : nodes[source];
        lazy[res].emplace(action_at(source));
        if(leaf){
            values[res].emplace(value_at(source, index));
        }
        return res;
    }

    void compose(int node, const T& f){
        T action = Monoid_act.composition(f, *lazy[node]);
        lazy[node].reset();
        lazy[node].emplace(action);
    }

    void push(int node, int l, int r){
        int mid = l + (r - l) / 2;
        int left = clone_node(nodes[node].left, mid - l == 1, l);
        int right = clone_node(nodes[node].right, r - mid == 1, mid);
        compose(left, *lazy[node]);
        compose(right, *lazy[node]);
        nodes[node] = {left, right};
        lazy[node].reset();
        lazy[node].emplace(Monoid_act.id());
    }

    void apply_impl(
        int node,
        int l,
        int r,
        int ql,
        int qr,
        const T& f
    ){
        if(ql <= l && r <= qr){
            compose(node, f);
            return;
        }
        push(node, l, r);
        int mid = l + (r - l) / 2;
        if(ql < mid){
            apply_impl(nodes[node].left, l, mid, ql, qr, f);
        }
        if(mid < qr){
            apply_impl(nodes[node].right, mid, r, ql, qr, f);
        }
    }

    void set_impl(int node, int l, int r, int k, const S& x){
        if(r - l == 1){
            values[node].reset();
            values[node].emplace(x);
            lazy[node].reset();
            lazy[node].emplace(Monoid_act.id());
            return;
        }
        push(node, l, r);
        int mid = l + (r - l) / 2;
        if(k < mid){
            set_impl(nodes[node].left, l, mid, k, x);
        }else{
            set_impl(nodes[node].right, mid, r, k, x);
        }
    }

    S get_impl(int node, int l, int r, int k) const{
        if(r - l == 1){
            return Monoid_act.mapping(
                action_at(node),
                value_at(node, l)
            );
        }
        int mid = l + (r - l) / 2;
        S value = k < mid
            ? get_impl(left_child(node), l, mid, k)
            : get_impl(right_child(node), mid, r, k);
        return Monoid_act.mapping(action_at(node), value);
    }

    void reset_nodes(int snapshot){
        while(snapshot < node_count){
            --node_count;
            lazy[node_count].reset();
            values[node_count].reset();
        }
    }

    int reserve_version() const{
        if(version_count > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: version capacity exceeded (update)."
            );
        }
        return version_count;
    }

public:
    PersistentDualSegtree(const PersistentDualSegtree&) = delete;
    PersistentDualSegtree& operator=(const PersistentDualSegtree&) = delete;
    PersistentDualSegtree(PersistentDualSegtree&&) = delete;
    PersistentDualSegtree& operator=(PersistentDualSegtree&&) = delete;

    explicit PersistentDualSegtree(int n = MAX_SIZE){
        initialize(n);
    }

    explicit PersistentDualSegtree(const std::vector<S>& v){
        if(v.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        initialize(static_cast<int>(v.size()));
        for(int k = 0; k < _n; k++){
            initial[k].reset();
            initial[k].emplace(v[static_cast<std::size_t>(k)]);
        }
    }

    template<std::size_t N>
    explicit PersistentDualSegtree(const std::array<S, N>& v){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
        initialize(static_cast<int>(N));
        for(int k = 0; k < _n; k++){
            initial[k].reset();
            initial[k].emplace(v[static_cast<std::size_t>(k)]);
        }
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

    int fork(int version){
        check_version(
            version,
            "library assertion fault: range violation (fork)."
        );
        int new_version = reserve_version();
        root[new_version] = root[version];
        version_count++;
        return new_version;
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
        int new_version = reserve_version();
        int snapshot = node_count;
        try{
            int new_root = clone_node(root[version], sz == 1, 0);
            set_impl(new_root, 0, sz, k, x);
            root[new_version] = new_root;
        }catch(...){
            reset_nodes(snapshot);
            throw;
        }
        version_count++;
        return new_version;
    }

    int apply(int version, int l, int r, const T& f){
        check_version(
            version,
            "library assertion fault: range violation (apply)."
        );
        if(l < 0 || r < l || _n < r)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (apply)."
            );
        }
        if(l == r){
            return fork(version);
        }
        int new_version = reserve_version();
        int snapshot = node_count;
        try{
            int new_root = clone_node(root[version], sz == 1, 0);
            apply_impl(new_root, 0, sz, l, r, f);
            root[new_version] = new_root;
        }catch(...){
            reset_nodes(snapshot);
            throw;
        }
        version_count++;
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
        return get_impl(root[version], 0, sz, k);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_DUALSEGTREE_HPP_INCLUDED
