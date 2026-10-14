#ifndef CPPLIB_SRC_STRUCTURE_BBST_SEQUENCE_PERSISTENT_LAZY_WEIGHTED_BALANCED_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_SEQUENCE_PERSISTENT_LAZY_WEIGHTED_BALANCED_TREE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include "../../array/static_or_vector.hpp"
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<auto MonoidActLen, int MAX_NODE = -1, int MAX_VERSION = -1>
struct PersistentLazyWeightedBalancedTree{
    static_assert(MAX_NODE == -1 || MAX_NODE > 0);
    static_assert(MAX_VERSION == -1 || MAX_VERSION > 0);
    using S = typename std::remove_cvref_t<decltype(MonoidActLen)>::S;
    using T = typename std::remove_cvref_t<decltype(MonoidActLen)>::T;

private:
    static constexpr int DELTA = 3;
    static constexpr int RATIO = 2;

    struct Node{
        int left = -1, right = -1, size = 0;
        bool reversed = false, has_lazy = false;
        S value{}, aggregate{}, rev_aggregate{};
        T lazy{};
    };
    struct QueryResult{ S value; int length; };

    cpplib_detail::StaticOrVector<Node, (MAX_NODE < 0 ? -1 : static_cast<std::ptrdiff_t>(MAX_NODE))> nodes{};
    cpplib_detail::StaticOrVector<int, (MAX_VERSION < 0 ? -1 : static_cast<std::ptrdiff_t>(MAX_VERSION))> roots{};
    int used = 0, version_count = 1;

    int size_or_zero(int v) const{ return v == -1 ? 0 : nodes[static_cast<std::size_t>(v)].size; }
    S aggregate_or_e(int v) const{ return v == -1 ? MonoidActLen.e() : nodes[static_cast<std::size_t>(v)].aggregate; }
    S rev_aggregate_or_e(int v) const{ return v == -1 ? MonoidActLen.e() : nodes[static_cast<std::size_t>(v)].rev_aggregate; }
    [[noreturn]] static void throw_capacity(){
        throw std::runtime_error("library assertion fault: capacity exceeded (PersistentLazyWeightedBalancedTree).");
    }
    int copy_node(int v){
        if constexpr(MAX_NODE < 0) nodes.ensure(cpplib_detail::checked_int_size(static_cast<std::size_t>(used) + 1));
        if(v == -1) return -1;
        if(used == MAX_NODE)[[unlikely]] throw_capacity();
        nodes[static_cast<std::size_t>(used)] = nodes[static_cast<std::size_t>(v)];
        return used++;
    }
    int new_node(const S& value){
        if constexpr(MAX_NODE < 0) nodes.ensure(cpplib_detail::checked_int_size(static_cast<std::size_t>(used) + 1));
        if(used == MAX_NODE)[[unlikely]] throw_capacity();
        int v = used++;
        nodes[static_cast<std::size_t>(v)] = {-1, -1, 1, false, false, value, value, value, MonoidActLen.id()};
        return v;
    }
    void pull(int v){
        const auto x_node_index_1 = static_cast<std::size_t>(v);
        int ls = size_or_zero(nodes[x_node_index_1].left), rs = size_or_zero(nodes[x_node_index_1].right);
        nodes[x_node_index_1].size = ls + 1 + rs;
        S prefix = MonoidActLen.op(aggregate_or_e(nodes[x_node_index_1].left), ls, nodes[x_node_index_1].value, 1);
        nodes[x_node_index_1].aggregate = MonoidActLen.op(prefix, ls + 1, aggregate_or_e(nodes[x_node_index_1].right), rs);
        S rev_prefix = MonoidActLen.op(rev_aggregate_or_e(nodes[x_node_index_1].right), rs, nodes[x_node_index_1].value, 1);
        nodes[x_node_index_1].rev_aggregate = MonoidActLen.op(rev_prefix, rs + 1, rev_aggregate_or_e(nodes[x_node_index_1].left), ls);
    }
    void all_apply(int v, const T& f){
        if(v == -1) return;
        const auto x_node_index_2 = static_cast<std::size_t>(v);
        nodes[x_node_index_2].value = MonoidActLen.mapping(f, nodes[x_node_index_2].value, 1);
        nodes[x_node_index_2].aggregate = MonoidActLen.mapping(f, nodes[x_node_index_2].aggregate, nodes[x_node_index_2].size);
        nodes[x_node_index_2].rev_aggregate = MonoidActLen.mapping(f, nodes[x_node_index_2].rev_aggregate, nodes[x_node_index_2].size);
        if(nodes[x_node_index_2].has_lazy) nodes[x_node_index_2].lazy = MonoidActLen.composition(f, nodes[x_node_index_2].lazy);
        else{ nodes[x_node_index_2].lazy = f; nodes[x_node_index_2].has_lazy = true; }
    }
    void toggle(int v){
        if(v == -1) return;
        const auto x_node_index_3 = static_cast<std::size_t>(v);
        std::swap(nodes[x_node_index_3].left, nodes[x_node_index_3].right);
        std::swap(nodes[x_node_index_3].aggregate, nodes[x_node_index_3].rev_aggregate);
        nodes[x_node_index_3].reversed = !nodes[x_node_index_3].reversed;
    }
    int expose(int root){
        int v = copy_node(root);
        const auto x_node_index_4 = static_cast<std::size_t>(v);
        if(!nodes[x_node_index_4].reversed && !nodes[x_node_index_4].has_lazy) return v;
        if(nodes[x_node_index_4].left != -1){
            nodes[x_node_index_4].left = copy_node(nodes[x_node_index_4].left);
            if(nodes[x_node_index_4].reversed) toggle(nodes[x_node_index_4].left);
            if(nodes[x_node_index_4].has_lazy) all_apply(nodes[x_node_index_4].left, nodes[x_node_index_4].lazy);
        }
        if(nodes[x_node_index_4].right != -1){
            nodes[x_node_index_4].right = copy_node(nodes[x_node_index_4].right);
            if(nodes[x_node_index_4].reversed) toggle(nodes[x_node_index_4].right);
            if(nodes[x_node_index_4].has_lazy) all_apply(nodes[x_node_index_4].right, nodes[x_node_index_4].lazy);
        }
        nodes[x_node_index_4].reversed = nodes[x_node_index_4].has_lazy = false;
        nodes[x_node_index_4].lazy = MonoidActLen.id();
        return v;
    }

    int rotate_left(int root){
        int r = expose(nodes[static_cast<std::size_t>(root)].right);
        nodes[static_cast<std::size_t>(root)].right = nodes[static_cast<std::size_t>(r)].left;
        nodes[static_cast<std::size_t>(r)].left = root;
        pull(root); pull(r);
        return r;
    }
    int rotate_right(int root){
        int l = expose(nodes[static_cast<std::size_t>(root)].left);
        nodes[static_cast<std::size_t>(root)].left = nodes[static_cast<std::size_t>(l)].right;
        nodes[static_cast<std::size_t>(l)].right = root;
        pull(root); pull(l);
        return l;
    }
    int rotate_left_right(int root){
        int l = expose(nodes[static_cast<std::size_t>(root)].left);
        int m = expose(nodes[static_cast<std::size_t>(l)].right);
        nodes[static_cast<std::size_t>(root)].left = nodes[static_cast<std::size_t>(m)].right;
        nodes[static_cast<std::size_t>(l)].right = nodes[static_cast<std::size_t>(m)].left;
        nodes[static_cast<std::size_t>(m)].left = l;
        nodes[static_cast<std::size_t>(m)].right = root;
        pull(l); pull(root); pull(m);
        return m;
    }
    int rotate_right_left(int root){
        int r = expose(nodes[static_cast<std::size_t>(root)].right);
        int m = expose(nodes[static_cast<std::size_t>(r)].left);
        nodes[static_cast<std::size_t>(root)].right = nodes[static_cast<std::size_t>(m)].left;
        nodes[static_cast<std::size_t>(r)].left = nodes[static_cast<std::size_t>(m)].right;
        nodes[static_cast<std::size_t>(m)].left = root;
        nodes[static_cast<std::size_t>(m)].right = r;
        pull(root); pull(r); pull(m);
        return m;
    }
    int balance(int root){
        int l = nodes[static_cast<std::size_t>(root)].left;
        int r = nodes[static_cast<std::size_t>(root)].right;
        int ls = size_or_zero(l), rs = size_or_zero(r);
        if(ls + rs <= 1){ pull(root); return root; }
        if(ls > DELTA * rs){
            const auto x_node_index_5 = static_cast<std::size_t>(l);
            return size_or_zero(nodes[x_node_index_5].right) < RATIO * size_or_zero(nodes[x_node_index_5].left) ? rotate_right(root) : rotate_left_right(root);
        }
        if(rs > DELTA * ls){
            const auto x_node_index_6 = static_cast<std::size_t>(r);
            return size_or_zero(nodes[x_node_index_6].left) < RATIO * size_or_zero(nodes[x_node_index_6].right) ? rotate_left(root) : rotate_right_left(root);
        }
        pull(root);
        return root;
    }

    int link(int left, int pivot, int right){
        int ls = size_or_zero(left), rs = size_or_zero(right);
        if(ls > DELTA * rs){
            int root = expose(left);
            nodes[static_cast<std::size_t>(root)].right = link(nodes[static_cast<std::size_t>(root)].right, pivot, right);
            pull(root);
            return balance(root);
        }
        if(rs > DELTA * ls){
            int root = expose(right);
            nodes[static_cast<std::size_t>(root)].left = link(left, pivot, nodes[static_cast<std::size_t>(root)].left);
            pull(root);
            return balance(root);
        }
        const auto x_node_index_7 = static_cast<std::size_t>(pivot);
        nodes[x_node_index_7].left = left; nodes[x_node_index_7].right = right;
        pull(pivot);
        return pivot;
    }
    std::pair<int, int> pop_min(int root){
        int v = expose(root);
        int l = nodes[static_cast<std::size_t>(v)].left;
        int r = nodes[static_cast<std::size_t>(v)].right;
        if(l == -1){ nodes[static_cast<std::size_t>(v)].right = -1; pull(v); return {r, v}; }
        auto [new_left, pivot] = pop_min(l);
        return {link(new_left, v, r), pivot};
    }
    std::pair<int, int> pop_max(int root){
        int v = expose(root);
        int l = nodes[static_cast<std::size_t>(v)].left;
        int r = nodes[static_cast<std::size_t>(v)].right;
        if(r == -1){ nodes[static_cast<std::size_t>(v)].left = -1; pull(v); return {l, v}; }
        auto [new_right, pivot] = pop_max(r);
        return {link(l, v, new_right), pivot};
    }
    int merge(int left, int right){
        if(left == -1) return right;
        if(right == -1) return left;
        if(size_or_zero(left) >= size_or_zero(right)){
            auto [head, pivot] = pop_max(left);
            return link(head, pivot, right);
        }
        auto [tail, pivot] = pop_min(right);
        return link(left, pivot, tail);
    }
    std::pair<int, int> split(int root, int k){
        if(root == -1) return {-1, -1};
        if(k == 0) return {-1, root};
        if(k == size_or_zero(root)) return {root, -1};
        int v = expose(root);
        int l = nodes[static_cast<std::size_t>(v)].left;
        int r = nodes[static_cast<std::size_t>(v)].right;
        int ls = size_or_zero(l);
        if(k <= ls){
            auto [head, tail] = split(l, k);
            return {head, link(tail, v, r)};
        }
        auto [head, tail] = split(r, k - ls - 1);
        return {link(l, v, head), tail};
    }

    int insert_rec(int root, int p, const S& value){
        if(root == -1) return new_node(value);
        int v = expose(root);
        int ls = size_or_zero(nodes[static_cast<std::size_t>(v)].left);
        if(p <= ls) nodes[static_cast<std::size_t>(v)].left = insert_rec(nodes[static_cast<std::size_t>(v)].left, p, value);
        else nodes[static_cast<std::size_t>(v)].right = insert_rec(nodes[static_cast<std::size_t>(v)].right, p - ls - 1, value);
        pull(v);
        return balance(v);
    }
    int erase_rec(int root, int p){
        int v = expose(root);
        int l = nodes[static_cast<std::size_t>(v)].left;
        int r = nodes[static_cast<std::size_t>(v)].right;
        int ls = size_or_zero(l);
        if(p < ls) nodes[static_cast<std::size_t>(v)].left = erase_rec(l, p);
        else if(p > ls) nodes[static_cast<std::size_t>(v)].right = erase_rec(r, p - ls - 1);
        else return merge(l, r);
        pull(v);
        return balance(v);
    }
    int set_rec(int root, int p, const S& value){
        int v = expose(root);
        const auto x_node_index_8 = static_cast<std::size_t>(v);
        int ls = size_or_zero(nodes[x_node_index_8].left);
        if(p < ls) nodes[x_node_index_8].left = set_rec(nodes[x_node_index_8].left, p, value);
        else if(p > ls) nodes[x_node_index_8].right = set_rec(nodes[x_node_index_8].right, p - ls - 1, value);
        else nodes[x_node_index_8].value = value;
        pull(v);
        return v;
    }
    int apply_rec(int root, int l, int r, const T& f){
        if(l == r) return root;
        int n = size_or_zero(root);
        if(l == 0 && r == n){ int v = copy_node(root); all_apply(v, f); return v; }
        int v = expose(root);
        const auto x_node_index_9 = static_cast<std::size_t>(v);
        int ls = size_or_zero(nodes[x_node_index_9].left);
        if(l < ls) nodes[x_node_index_9].left = apply_rec(nodes[x_node_index_9].left, l, std::min(r, ls), f);
        if(l <= ls && ls < r) nodes[x_node_index_9].value = MonoidActLen.mapping(f, nodes[x_node_index_9].value, 1);
        if(ls + 1 < r) nodes[x_node_index_9].right = apply_rec(nodes[x_node_index_9].right, std::max(0, l - ls - 1), r - ls - 1, f);
        pull(v);
        return v;
    }
    int build(const std::vector<S>& values, int l, int r){
        if(l == r) return -1;
        int m = (l + r) >> 1;
        int v = new_node(values[static_cast<std::size_t>(m)]);
        nodes[static_cast<std::size_t>(v)].left = build(values, l, m);
        nodes[static_cast<std::size_t>(v)].right = build(values, m + 1, r);
        pull(v);
        return v;
    }

    [[noreturn]] static void throw_range(){
        throw std::runtime_error("library assertion fault: range violation (PersistentLazyWeightedBalancedTree).");
    }
    void check_version(int version) const{ if(version < 0 || version_count <= version)[[unlikely]] throw_range(); }
    void check_insert_position(int version, int p) const{
        check_version(version);
        if(p < 0 || size_or_zero(roots[static_cast<std::size_t>(version)]) < p)[[unlikely]] throw_range();
    }
    void check_position(int version, int p) const{
        check_version(version);
        if(p < 0 || size_or_zero(roots[static_cast<std::size_t>(version)]) <= p)[[unlikely]] throw_range();
    }
    void check_range(int version, int l, int r) const{
        check_version(version);
        if(l < 0 || r < l || size_or_zero(roots[static_cast<std::size_t>(version)]) < r)[[unlikely]] throw_range();
    }
    template<class F> int make_version(F&& operation){
        if(version_count == MAX_VERSION)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (PersistentLazyWeightedBalancedTree).");
        }
        int checkpoint = used;
        try{
            int root = operation();
            if constexpr(MAX_VERSION < 0) roots.ensure(static_cast<std::size_t>(version_count) + 1);
            roots[static_cast<std::size_t>(version_count)] = root;
            return version_count++;
        }catch(...){ used = checkpoint; throw; }
    }

    QueryResult combine(const QueryResult& a, const QueryResult& b) const{
        if(a.length == 0) return b;
        if(b.length == 0) return a;
        return {MonoidActLen.op(a.value, a.length, b.value, b.length), a.length + b.length};
    }
    T child_action(const T& action, const Node& x) const{
        return x.has_lazy ? MonoidActLen.composition(action, x.lazy) : action;
    }
    S get_rec(int v, int p, bool flip, const T& action) const{
        const auto x_node_index_10 = static_cast<std::size_t>(v);
        int l = flip ? nodes[x_node_index_10].right : nodes[x_node_index_10].left;
        int r = flip ? nodes[x_node_index_10].left : nodes[x_node_index_10].right;
        bool child_flip = flip ^ nodes[x_node_index_10].reversed;
        T next_action = child_action(action, nodes[x_node_index_10]);
        int ls = size_or_zero(l);
        if(p < ls) return get_rec(l, p, child_flip, next_action);
        if(p == ls) return MonoidActLen.mapping(action, nodes[x_node_index_10].value, 1);
        return get_rec(r, p - ls - 1, child_flip, next_action);
    }
    QueryResult prod_rec(int v, int l, int r, bool flip, const T& action) const{
        if(l == r) return {MonoidActLen.e(), 0};
        int n = size_or_zero(v);
        if(l == 0 && r == n){
            S value = flip ? rev_aggregate_or_e(v) : aggregate_or_e(v);
            return {MonoidActLen.mapping(action, value, n), n};
        }
        const auto x_node_index_11 = static_cast<std::size_t>(v);
        int left = flip ? nodes[x_node_index_11].right : nodes[x_node_index_11].left;
        int right = flip ? nodes[x_node_index_11].left : nodes[x_node_index_11].right;
        bool child_flip = flip ^ nodes[x_node_index_11].reversed;
        T next_action = child_action(action, nodes[x_node_index_11]);
        int ls = size_or_zero(left);
        QueryResult result{MonoidActLen.e(), 0};
        if(l < ls) result = prod_rec(left, l, std::min(r, ls), child_flip, next_action);
        if(l <= ls && ls < r) result = combine(result, {MonoidActLen.mapping(action, nodes[x_node_index_11].value, 1), 1});
        if(ls + 1 < r){
            result = combine(result, prod_rec(right, std::max(0, l - ls - 1), r - ls - 1, child_flip, next_action));
        }
        return result;
    }
    void collect(int v, bool flip, const T& action, std::vector<S>& result) const{
        if(v == -1) return;
        const auto x_node_index_12 = static_cast<std::size_t>(v);
        int l = flip ? nodes[x_node_index_12].right : nodes[x_node_index_12].left;
        int r = flip ? nodes[x_node_index_12].left : nodes[x_node_index_12].right;
        bool child_flip = flip ^ nodes[x_node_index_12].reversed;
        T next_action = child_action(action, nodes[x_node_index_12]);
        collect(l, child_flip, next_action, result);
        result.push_back(MonoidActLen.mapping(action, nodes[x_node_index_12].value, 1));
        collect(r, child_flip, next_action, result);
    }

public:
    PersistentLazyWeightedBalancedTree(){ roots.ensure(1); roots.fill(-1); }
    explicit PersistentLazyWeightedBalancedTree(const std::vector<S>& values): PersistentLazyWeightedBalancedTree(){
        if(values.size() > static_cast<std::size_t>(MAX_NODE))[[unlikely]] throw_capacity();
        roots[0] = build(values, 0, cpplib_detail::checked_int_size(values.size()));
    }
    int versions() const{ return version_count; }
    int size(int version = 0) const{ check_version(version); return size_or_zero(roots[static_cast<std::size_t>(version)]); }
    bool empty(int version = 0) const{ return size(version) == 0; }
    int insert(int p, const S& value, int version = 0){
        check_insert_position(version, p);
        return make_version([&]{ return insert_rec(roots[static_cast<std::size_t>(version)], p, value); });
    }
    int erase(int p, int version = 0){
        check_position(version, p);
        return make_version([&]{ return erase_rec(roots[static_cast<std::size_t>(version)], p); });
    }
    int set(int p, const S& value, int version = 0){
        check_position(version, p);
        return make_version([&]{ return set_rec(roots[static_cast<std::size_t>(version)], p, value); });
    }
    int apply(int l, int r, const T& f, int version = 0){
        check_range(version, l, r);
        return make_version([&]{ return apply_rec(roots[static_cast<std::size_t>(version)], l, r, f); });
    }
    int reverse(int l, int r, int version = 0){
        check_range(version, l, r);
        return make_version([&]{
            int root = roots[static_cast<std::size_t>(version)];
            if(l == r) return root;
            if(l == 0 && r == size_or_zero(root)){ root = copy_node(root); toggle(root); return root; }
            auto [left, right] = split(root, l);
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
        return prod_rec(roots[static_cast<std::size_t>(version)], l, r, false, MonoidActLen.id()).value;
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

#endif  // CPPLIB_SRC_STRUCTURE_BBST_SEQUENCE_PERSISTENT_LAZY_WEIGHTED_BALANCED_TREE_HPP_INCLUDED
