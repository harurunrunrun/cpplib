#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_LAZYSEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_LAZYSEGTREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include "../types/monoid_act.hpp"

template<auto Monoid_act, int MAX_SIZE, int MAX_VERSION>
struct PersistentLazySegtree{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_VERSION >= 0);
    static_assert(MAX_VERSION < std::numeric_limits<int>::max());

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

    std::array<std::optional<S>, 2 * sz> initial;
    std::array<int, static_cast<std::size_t>(MAX_VERSION) + 1> root;
    std::array<Node, node_capacity> nodes;
    std::array<std::optional<S>, node_capacity> values;
    std::array<std::optional<T>, node_capacity> lazy;

    void initialize(int n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }

        _n = n;
        root.fill(-1);
        for(int k = 0; k < 2 * sz; k++){
            initial[k].emplace(Monoid_act.e());
        }
    }

    void build(){
        for(int k = sz - 1; k > 0; k--){
            S value = Monoid_act.op(*initial[2 * k], *initial[2 * k + 1]);
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

    T lazy_at(int node) const{
        if(node == -1){
            return Monoid_act.id();
        }
        return *lazy[node];
    }

    int left_child(int node) const{
        return node == -1 ? -1 : nodes[node].left;
    }

    int right_child(int node) const{
        return node == -1 ? -1 : nodes[node].right;
    }

    int clone_node(int source, int tree_index){
        if(static_cast<std::size_t>(node_count) >= node_capacity)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: node capacity exceeded (update)."
            );
        }

        int res = node_count++;
        if(source == -1){
            nodes[res] = {-1, -1};
            lazy[res].emplace(Monoid_act.id());
        }else{
            nodes[res] = nodes[source];
            lazy[res].emplace(*lazy[source]);
        }
        values[res].emplace(value_at(source, tree_index));
        return res;
    }

    void all_apply(int node, const T& f){
        S value = Monoid_act.mapping(f, *values[node]);
        T action = Monoid_act.composition(f, *lazy[node]);
        values[node].reset();
        values[node].emplace(value);
        lazy[node].reset();
        lazy[node].emplace(action);
    }

    void push(int node, int tree_index){
        int left = clone_node(nodes[node].left, 2 * tree_index);
        int right = clone_node(nodes[node].right, 2 * tree_index + 1);
        all_apply(left, *lazy[node]);
        all_apply(right, *lazy[node]);
        nodes[node] = {left, right};
        lazy[node].reset();
        lazy[node].emplace(Monoid_act.id());
    }

    void update(int node){
        S value = Monoid_act.op(
            *values[nodes[node].left],
            *values[nodes[node].right]
        );
        values[node].reset();
        values[node].emplace(value);
    }

    void set_impl(
        int node,
        int tree_index,
        int l,
        int r,
        int k,
        const S& x
    ){
        if(r - l == 1){
            values[node].reset();
            values[node].emplace(x);
            lazy[node].reset();
            lazy[node].emplace(Monoid_act.id());
            return;
        }

        push(node, tree_index);
        int mid = l + (r - l) / 2;
        if(k < mid){
            set_impl(nodes[node].left, 2 * tree_index, l, mid, k, x);
        }else{
            set_impl(
                nodes[node].right, 2 * tree_index + 1, mid, r, k, x
            );
        }
        update(node);
    }

    void apply_impl(
        int node,
        int tree_index,
        int l,
        int r,
        int ql,
        int qr,
        const T& f
    ){
        if(ql <= l && r <= qr){
            all_apply(node, f);
            return;
        }

        push(node, tree_index);
        int mid = l + (r - l) / 2;
        if(ql < mid){
            apply_impl(
                nodes[node].left, 2 * tree_index, l, mid, ql, qr, f
            );
        }
        if(mid < qr){
            apply_impl(
                nodes[node].right, 2 * tree_index + 1, mid, r, ql, qr, f
            );
        }
        update(node);
    }

    S prod_impl(
        int node,
        int tree_index,
        int l,
        int r,
        int ql,
        int qr,
        const T& carry
    ) const{
        if(r <= ql || qr <= l){
            return Monoid_act.e();
        }
        if(ql <= l && r <= qr){
            return Monoid_act.mapping(carry, value_at(node, tree_index));
        }

        T next = Monoid_act.composition(carry, lazy_at(node));
        int mid = l + (r - l) / 2;
        return Monoid_act.op(
            prod_impl(
                left_child(node), 2 * tree_index,
                l, mid, ql, qr, next
            ),
            prod_impl(
                right_child(node), 2 * tree_index + 1,
                mid, r, ql, qr, next
            )
        );
    }

    template<class F, class... Args>
    int max_right_impl(
        int node,
        int tree_index,
        int l,
        int r,
        int ql,
        const T& carry,
        S& sm,
        F& f,
        Args&... args
    ) const{
        if(r <= ql || _n <= l){
            return -1;
        }

        if(ql <= l && r <= _n){
            S value = Monoid_act.mapping(carry, value_at(node, tree_index));
            S next_value = Monoid_act.op(sm, value);
            if(std::invoke(f, next_value, args...)){
                sm = next_value;
                return -1;
            }
            if(r - l == 1){
                return l;
            }
        }

        T next = Monoid_act.composition(carry, lazy_at(node));
        int mid = l + (r - l) / 2;
        int res = max_right_impl(
            left_child(node), 2 * tree_index, l, mid,
            ql, next, sm, f, args...
        );
        if(res != -1){
            return res;
        }
        return max_right_impl(
            right_child(node), 2 * tree_index + 1, mid, r,
            ql, next, sm, f, args...
        );
    }

    template<class F, class... Args>
    int min_left_impl(
        int node,
        int tree_index,
        int l,
        int r,
        int qr,
        const T& carry,
        S& sm,
        F& f,
        Args&... args
    ) const{
        if(qr <= l || _n <= l){
            return -1;
        }

        if(r <= qr && r <= _n){
            S value = Monoid_act.mapping(carry, value_at(node, tree_index));
            S next_value = Monoid_act.op(value, sm);
            if(std::invoke(f, next_value, args...)){
                sm = next_value;
                return -1;
            }
            if(r - l == 1){
                return r;
            }
        }

        T next = Monoid_act.composition(carry, lazy_at(node));
        int mid = l + (r - l) / 2;
        int res = min_left_impl(
            right_child(node), 2 * tree_index + 1, mid, r,
            qr, next, sm, f, args...
        );
        if(res != -1){
            return res;
        }
        return min_left_impl(
            left_child(node), 2 * tree_index, l, mid,
            qr, next, sm, f, args...
        );
    }

    void reset_nodes(int snapshot){
        while(snapshot < node_count){
            --node_count;
            values[node_count].reset();
            lazy[node_count].reset();
        }
    }

    int reserve_version(){
        if(version_count > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: version capacity exceeded (update)."
            );
        }
        return version_count;
    }

public:
    PersistentLazySegtree(const PersistentLazySegtree&) = delete;
    PersistentLazySegtree& operator=(const PersistentLazySegtree&) = delete;
    PersistentLazySegtree(PersistentLazySegtree&&) = delete;
    PersistentLazySegtree& operator=(PersistentLazySegtree&&) = delete;

    explicit PersistentLazySegtree(int n = MAX_SIZE){
        initialize(n);
        build();
    }

    explicit PersistentLazySegtree(const std::vector<S>& v){
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
    explicit PersistentLazySegtree(const std::array<S, N>& v){
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

    int fork(int version){
        check_version(
            version,
            "library assertion fault: range violation (fork)."
        );
        int new_version = reserve_version();
        version_count++;
        root[new_version] = root[version];
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
            int new_root = clone_node(root[version], 1);
            set_impl(new_root, 1, 0, sz, k, x);
            root[new_version] = new_root;
        }catch(...){
            reset_nodes(snapshot);
            throw;
        }
        version_count++;
        return new_version;
    }

    int apply(int version, int k, const T& f){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (apply)."
            );
        }
        return apply(version, k, k + 1, f);
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
            int new_root = clone_node(root[version], 1);
            apply_impl(new_root, 1, 0, sz, l, r, f);
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
        return prod_impl(
            root[version], 1, 0, sz, k, k + 1, Monoid_act.id()
        );
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
        return prod_impl(
            root[version], 1, 0, sz, l, r, Monoid_act.id()
        );
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
        if(!std::invoke(f, Monoid_act.e(), args...))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: f(e) must be true (max_right)."
            );
        }
        if(l == _n){
            return _n;
        }

        S sm = Monoid_act.e();
        int res = max_right_impl(
            root[version], 1, 0, sz, l, Monoid_act.id(),
            sm, f, args...
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
        if(!std::invoke(f, Monoid_act.e(), args...))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: f(e) must be true (min_left)."
            );
        }
        if(r == 0){
            return 0;
        }

        S sm = Monoid_act.e();
        int res = min_left_impl(
            root[version], 1, 0, sz, r, Monoid_act.id(),
            sm, f, args...
        );
        return res == -1 ? 0 : res;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_LAZYSEGTREE_HPP_INCLUDED
