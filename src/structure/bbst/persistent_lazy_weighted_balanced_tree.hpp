#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<auto MonoidActLen, int MAX_NODE, int MAX_VERSION>
struct PersistentLazyWeightedBalancedTree{
    static_assert(MAX_NODE > 0);
    static_assert(MAX_VERSION > 0);
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

    std::array<Node, MAX_NODE> nodes{};
    std::array<int, MAX_VERSION> roots{};
    int used = 0, version_count = 1;

    int size_or_zero(int v) const{ return v == -1 ? 0 : nodes[static_cast<std::size_t>(v)].size; }
    S aggregate_or_e(int v) const{ return v == -1 ? MonoidActLen.e() : nodes[static_cast<std::size_t>(v)].aggregate; }
    S rev_aggregate_or_e(int v) const{ return v == -1 ? MonoidActLen.e() : nodes[static_cast<std::size_t>(v)].rev_aggregate; }
    [[noreturn]] static void throw_capacity(){
        throw std::runtime_error("library assertion fault: capacity exceeded (PersistentLazyWeightedBalancedTree).");
    }
    int copy_node(int v){
        if(v == -1) return -1;
        if(used == MAX_NODE)[[unlikely]] throw_capacity();
        nodes[static_cast<std::size_t>(used)] = nodes[static_cast<std::size_t>(v)];
        return used++;
    }
    int new_node(const S& value){
        if(used == MAX_NODE)[[unlikely]] throw_capacity();
        int v = used++;
        nodes[static_cast<std::size_t>(v)] = {-1, -1, 1, false, false, value, value, value, MonoidActLen.id()};
        return v;
    }
    void pull(int v){
        auto& x = nodes[static_cast<std::size_t>(v)];
        int ls = size_or_zero(x.left), rs = size_or_zero(x.right);
        x.size = ls + 1 + rs;
        S prefix = MonoidActLen.op(aggregate_or_e(x.left), ls, x.value, 1);
        x.aggregate = MonoidActLen.op(prefix, ls + 1, aggregate_or_e(x.right), rs);
        S rev_prefix = MonoidActLen.op(rev_aggregate_or_e(x.right), rs, x.value, 1);
        x.rev_aggregate = MonoidActLen.op(rev_prefix, rs + 1, rev_aggregate_or_e(x.left), ls);
    }
    void all_apply(int v, const T& f){
        if(v == -1) return;
        auto& x = nodes[static_cast<std::size_t>(v)];
        x.value = MonoidActLen.mapping(f, x.value, 1);
        x.aggregate = MonoidActLen.mapping(f, x.aggregate, x.size);
        x.rev_aggregate = MonoidActLen.mapping(f, x.rev_aggregate, x.size);
        if(x.has_lazy) x.lazy = MonoidActLen.composition(f, x.lazy);
        else{ x.lazy = f; x.has_lazy = true; }
    }
    void toggle(int v){
        if(v == -1) return;
        auto& x = nodes[static_cast<std::size_t>(v)];
        std::swap(x.left, x.right);
        std::swap(x.aggregate, x.rev_aggregate);
        x.reversed = !x.reversed;
    }
    int expose(int root){
        int v = copy_node(root);
        auto& x = nodes[static_cast<std::size_t>(v)];
        if(!x.reversed && !x.has_lazy) return v;
        if(x.left != -1){
            x.left = copy_node(x.left);
            if(x.reversed) toggle(x.left);
            if(x.has_lazy) all_apply(x.left, x.lazy);
        }
        if(x.right != -1){
            x.right = copy_node(x.right);
            if(x.reversed) toggle(x.right);
            if(x.has_lazy) all_apply(x.right, x.lazy);
        }
        x.reversed = x.has_lazy = false;
        x.lazy = MonoidActLen.id();
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
            const auto& x = nodes[static_cast<std::size_t>(l)];
            return size_or_zero(x.right) < RATIO * size_or_zero(x.left) ? rotate_right(root) : rotate_left_right(root);
        }
        if(rs > DELTA * ls){
            const auto& x = nodes[static_cast<std::size_t>(r)];
            return size_or_zero(x.left) < RATIO * size_or_zero(x.right) ? rotate_left(root) : rotate_right_left(root);
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
        auto& x = nodes[static_cast<std::size_t>(pivot)];
        x.left = left; x.right = right;
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
        auto& x = nodes[static_cast<std::size_t>(v)];
        int ls = size_or_zero(x.left);
        if(p < ls) x.left = set_rec(x.left, p, value);
        else if(p > ls) x.right = set_rec(x.right, p - ls - 1, value);
        else x.value = value;
        pull(v);
        return v;
    }
    int apply_rec(int root, int l, int r, const T& f){
        if(l == r) return root;
        int n = size_or_zero(root);
        if(l == 0 && r == n){ int v = copy_node(root); all_apply(v, f); return v; }
        int v = expose(root);
        auto& x = nodes[static_cast<std::size_t>(v)];
        int ls = size_or_zero(x.left);
        if(l < ls) x.left = apply_rec(x.left, l, std::min(r, ls), f);
        if(l <= ls && ls < r) x.value = MonoidActLen.mapping(f, x.value, 1);
        if(ls + 1 < r) x.right = apply_rec(x.right, std::max(0, l - ls - 1), r - ls - 1, f);
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
        const auto& x = nodes[static_cast<std::size_t>(v)];
        int l = flip ? x.right : x.left;
        int r = flip ? x.left : x.right;
        bool child_flip = flip ^ x.reversed;
        T next_action = child_action(action, x);
        int ls = size_or_zero(l);
        if(p < ls) return get_rec(l, p, child_flip, next_action);
        if(p == ls) return MonoidActLen.mapping(action, x.value, 1);
        return get_rec(r, p - ls - 1, child_flip, next_action);
    }
    QueryResult prod_rec(int v, int l, int r, bool flip, const T& action) const{
        if(l == r) return {MonoidActLen.e(), 0};
        int n = size_or_zero(v);
        if(l == 0 && r == n){
            S value = flip ? rev_aggregate_or_e(v) : aggregate_or_e(v);
            return {MonoidActLen.mapping(action, value, n), n};
        }
        const auto& x = nodes[static_cast<std::size_t>(v)];
        int left = flip ? x.right : x.left;
        int right = flip ? x.left : x.right;
        bool child_flip = flip ^ x.reversed;
        T next_action = child_action(action, x);
        int ls = size_or_zero(left);
        QueryResult result{MonoidActLen.e(), 0};
        if(l < ls) result = prod_rec(left, l, std::min(r, ls), child_flip, next_action);
        if(l <= ls && ls < r) result = combine(result, {MonoidActLen.mapping(action, x.value, 1), 1});
        if(ls + 1 < r){
            result = combine(result, prod_rec(right, std::max(0, l - ls - 1), r - ls - 1, child_flip, next_action));
        }
        return result;
    }
    void collect(int v, bool flip, const T& action, std::vector<S>& result) const{
        if(v == -1) return;
        const auto& x = nodes[static_cast<std::size_t>(v)];
        int l = flip ? x.right : x.left;
        int r = flip ? x.left : x.right;
        bool child_flip = flip ^ x.reversed;
        T next_action = child_action(action, x);
        collect(l, child_flip, next_action, result);
        result.push_back(MonoidActLen.mapping(action, x.value, 1));
        collect(r, child_flip, next_action, result);
    }

public:
    PersistentLazyWeightedBalancedTree(){ roots.fill(-1); }
    explicit PersistentLazyWeightedBalancedTree(const std::vector<S>& values): PersistentLazyWeightedBalancedTree(){
        if(values.size() > static_cast<std::size_t>(MAX_NODE))[[unlikely]] throw_capacity();
        roots[0] = build(values, 0, static_cast<int>(values.size()));
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
