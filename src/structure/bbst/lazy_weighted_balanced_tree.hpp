#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<auto MonoidActLen, int MAX_SIZE>
struct LazyWeightedBalancedTree{
    static_assert(MAX_SIZE > 0);
    using S = typename std::remove_cvref_t<decltype(MonoidActLen)>::S;
    using T = typename std::remove_cvref_t<decltype(MonoidActLen)>::T;

private:
    struct State{
        std::array<int, MAX_SIZE> left{};
        std::array<int, MAX_SIZE> right{};
        std::array<int, MAX_SIZE> size{};
        std::array<int, MAX_SIZE> free_stack{};
        std::array<bool, MAX_SIZE> reversed{};
        std::array<S, MAX_SIZE> value;
        std::array<S, MAX_SIZE> aggregate;
        std::array<S, MAX_SIZE> rev_aggregate;
        std::array<T, MAX_SIZE> lazy;
    };

    int root_node = -1;
    int used = 0;
    int free_count = 0;
    std::unique_ptr<State> state;

    int size_or_zero(int v) const{
        return v == -1 ? 0 : state->size[v];
    }
    S aggregate_or_e(int v) const{
        return v == -1 ? MonoidActLen.e() : state->aggregate[v];
    }
    S rev_aggregate_or_e(int v) const{
        return v == -1 ? MonoidActLen.e() : state->rev_aggregate[v];
    }
    void check_insert_position(int p, const char* message) const{
        if(p < 0 || size() < p)[[unlikely]] throw std::runtime_error(message);
    }
    void check_position(int p, const char* message) const{
        if(p < 0 || size() <= p)[[unlikely]] throw std::runtime_error(message);
    }
    void check_range(int l, int r, const char* message) const{
        if(l < 0 || r < l || size() < r)[[unlikely]] throw std::runtime_error(message);
    }
    int new_node(const S& x){
        int v;
        if(free_count > 0){
            v = state->free_stack[--free_count];
        }else{
            if(used == MAX_SIZE)[[unlikely]]{
                throw std::runtime_error("library assertion fault: capacity exceeded (insert).");
            }
            v = used++;
        }
        state->left[v] = state->right[v] = -1;
        state->size[v] = 1;
        state->reversed[v] = false;
        state->value[v] = x;
        state->aggregate[v] = x;
        state->rev_aggregate[v] = x;
        state->lazy[v] = MonoidActLen.id();
        return v;
    }
    void pull(int v){
        int l = state->left[v], r = state->right[v];
        int ls = size_or_zero(l), rs = size_or_zero(r);
        state->size[v] = ls + 1 + rs;

        S prefix = MonoidActLen.op(aggregate_or_e(l), ls, state->value[v], 1);
        state->aggregate[v] = MonoidActLen.op(prefix, ls + 1, aggregate_or_e(r), rs);

        S rev_prefix = MonoidActLen.op(rev_aggregate_or_e(r), rs, state->value[v], 1);
        state->rev_aggregate[v] = MonoidActLen.op(rev_prefix, rs + 1, rev_aggregate_or_e(l), ls);
    }
    void all_apply(int v, const T& f){
        if(v == -1) return;
        state->value[v] = MonoidActLen.mapping(f, state->value[v], 1);
        state->aggregate[v] = MonoidActLen.mapping(f, state->aggregate[v], state->size[v]);
        state->rev_aggregate[v] = MonoidActLen.mapping(f, state->rev_aggregate[v], state->size[v]);
        state->lazy[v] = MonoidActLen.composition(f, state->lazy[v]);
    }
    void toggle(int v){
        if(v == -1) return;
        std::swap(state->left[v], state->right[v]);
        std::swap(state->aggregate[v], state->rev_aggregate[v]);
        state->reversed[v] = !state->reversed[v];
    }
    void push(int v){
        if(v == -1) return;
        if(state->reversed[v]){
            toggle(state->left[v]);
            toggle(state->right[v]);
            state->reversed[v] = false;
        }
        all_apply(state->left[v], state->lazy[v]);
        all_apply(state->right[v], state->lazy[v]);
        state->lazy[v] = MonoidActLen.id();
    }
    int rotate_left(int v){
        push(v);
        int r = state->right[v];
        push(r);
        state->right[v] = state->left[r];
        state->left[r] = v;
        pull(v);
        pull(r);
        return r;
    }
    int rotate_right(int v){
        push(v);
        int l = state->left[v];
        push(l);
        state->left[v] = state->right[l];
        state->right[l] = v;
        pull(v);
        pull(l);
        return l;
    }
    int maintain(int v, bool right_heavy){
        if(v == -1) return v;
        push(v);
        if(!right_heavy){
            int l = state->left[v];
            if(l == -1) return v;
            push(l);
            if(size_or_zero(state->left[l]) > size_or_zero(state->right[v])){
                v = rotate_right(v);
            }else if(size_or_zero(state->right[l]) > size_or_zero(state->right[v])){
                state->left[v] = rotate_left(l);
                v = rotate_right(v);
            }else{
                return v;
            }
        }else{
            int r = state->right[v];
            if(r == -1) return v;
            push(r);
            if(size_or_zero(state->right[r]) > size_or_zero(state->left[v])){
                v = rotate_left(v);
            }else if(size_or_zero(state->left[r]) > size_or_zero(state->left[v])){
                state->right[v] = rotate_right(r);
                v = rotate_left(v);
            }else{
                return v;
            }
        }
        state->left[v] = maintain(state->left[v], false);
        state->right[v] = maintain(state->right[v], true);
        v = maintain(v, false);
        v = maintain(v, true);
        pull(v);
        return v;
    }
    int balance(int v){
        if(v == -1) return v;
        pull(v);
        v = maintain(v, false);
        v = maintain(v, true);
        pull(v);
        return v;
    }
    std::pair<int, int> split(int v, int k){
        if(v == -1) return {-1, -1};
        push(v);
        int ls = size_or_zero(state->left[v]);
        if(k <= ls){
            auto [a, b] = split(state->left[v], k);
            state->left[v] = b;
            return {a, balance(v)};
        }else{
            auto [a, b] = split(state->right[v], k - ls - 1);
            state->right[v] = a;
            return {balance(v), b};
        }
    }
    int merge(int a, int b){
        if(a == -1) return b;
        if(b == -1) return a;
        if(size_or_zero(a) >= size_or_zero(b)){
            push(a);
            state->right[a] = merge(state->right[a], b);
            return balance(a);
        }else{
            push(b);
            state->left[b] = merge(a, state->left[b]);
            return balance(b);
        }
    }
    int build(const std::vector<S>& values, int l, int r){
        if(l == r) return -1;
        int m = (l + r) >> 1;
        int v = new_node(values[static_cast<std::size_t>(m)]);
        state->left[v] = build(values, l, m);
        state->right[v] = build(values, m + 1, r);
        pull(v);
        return v;
    }

public:
    LazyWeightedBalancedTree(): state(std::make_unique<State>()){}
    explicit LazyWeightedBalancedTree(const std::vector<S>& values):
        LazyWeightedBalancedTree(){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (constructor).");
        }
        root_node = build(values, 0, static_cast<int>(values.size()));
    }

    LazyWeightedBalancedTree(const LazyWeightedBalancedTree&) = delete;
    LazyWeightedBalancedTree& operator=(const LazyWeightedBalancedTree&) = delete;
    LazyWeightedBalancedTree(LazyWeightedBalancedTree&&) = default;
    LazyWeightedBalancedTree& operator=(LazyWeightedBalancedTree&&) = default;

    int size() const{ return size_or_zero(root_node); }
    bool empty() const{ return root_node == -1; }
    void clear(){
        root_node = -1;
        used = 0;
        free_count = 0;
    }

    void insert(int p, const S& x){
        check_insert_position(p, "library assertion fault: range violation (insert).");
        auto [a, b] = split(root_node, p);
        root_node = merge(merge(a, new_node(x)), b);
    }
    void push_front(const S& x){ insert(0, x); }
    void push_back(const S& x){ insert(size(), x); }

    void erase(int p){
        check_position(p, "library assertion fault: range violation (erase).");
        auto [a, b] = split(root_node, p);
        auto [c, d] = split(b, 1);
        state->free_stack[free_count++] = c;
        root_node = merge(a, d);
    }
    void set(int p, const S& x){
        check_position(p, "library assertion fault: range violation (set).");
        auto [a, b] = split(root_node, p);
        auto [c, d] = split(b, 1);
        state->value[c] = x;
        state->lazy[c] = MonoidActLen.id();
        pull(c);
        root_node = merge(merge(a, c), d);
    }
    S get(int p){
        check_position(p, "library assertion fault: range violation (get).");
        auto [a, b] = split(root_node, p);
        auto [c, d] = split(b, 1);
        S result = state->value[c];
        root_node = merge(merge(a, c), d);
        return result;
    }
    void apply(int l, int r, const T& f){
        check_range(l, r, "library assertion fault: range violation (apply).");
        auto [a, b] = split(root_node, l);
        auto [c, d] = split(b, r - l);
        all_apply(c, f);
        root_node = merge(merge(a, c), d);
    }
    void reverse(int l, int r){
        check_range(l, r, "library assertion fault: range violation (reverse).");
        auto [a, b] = split(root_node, l);
        auto [c, d] = split(b, r - l);
        toggle(c);
        root_node = merge(merge(a, c), d);
    }
    S prod(int l, int r){
        check_range(l, r, "library assertion fault: range violation (prod).");
        auto [a, b] = split(root_node, l);
        auto [c, d] = split(b, r - l);
        S result = aggregate_or_e(c);
        root_node = merge(merge(a, c), d);
        return result;
    }
    S all_prod() const{
        return aggregate_or_e(root_node);
    }
};
