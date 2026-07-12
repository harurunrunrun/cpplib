#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<auto Monoid, int MAX_SIZE>
struct ReversibleSplayTree{
    static_assert(MAX_SIZE > 0);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

private:
    struct State{
        std::array<int, MAX_SIZE> left{};
        std::array<int, MAX_SIZE> right{};
        std::array<int, MAX_SIZE> parent{};
        std::array<int, MAX_SIZE> size{};
        std::array<int, MAX_SIZE> free_stack{};
        std::array<bool, MAX_SIZE> reversed{};
        std::array<S, MAX_SIZE> value;
        std::array<S, MAX_SIZE> aggregate;
        std::array<S, MAX_SIZE> rev_aggregate;
    };

    int root_node = -1;
    int used = 0;
    int free_count = 0;
    std::unique_ptr<State> state;

    int size_or_zero(int v) const{
        return v == -1 ? 0 : state->size[v];
    }
    S aggregate_or_e(int v) const{
        return v == -1 ? Monoid.e() : state->aggregate[v];
    }
    S rev_aggregate_or_e(int v) const{
        return v == -1 ? Monoid.e() : state->rev_aggregate[v];
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
        state->left[v] = state->right[v] = state->parent[v] = -1;
        state->size[v] = 1;
        state->reversed[v] = false;
        state->value[v] = x;
        state->aggregate[v] = x;
        state->rev_aggregate[v] = x;
        return v;
    }
    void pull(int v){
        int l = state->left[v], r = state->right[v];
        state->size[v] = 1 + size_or_zero(l) + size_or_zero(r);
        state->aggregate[v] = Monoid.op(
            aggregate_or_e(l),
            Monoid.op(state->value[v], aggregate_or_e(r))
        );
        state->rev_aggregate[v] = Monoid.op(
            rev_aggregate_or_e(r),
            Monoid.op(state->value[v], rev_aggregate_or_e(l))
        );
    }
    void toggle(int v){
        if(v == -1) return;
        std::swap(state->left[v], state->right[v]);
        std::swap(state->aggregate[v], state->rev_aggregate[v]);
        state->reversed[v] = !state->reversed[v];
    }
    void push(int v){
        if(v == -1 || !state->reversed[v]) return;
        toggle(state->left[v]);
        toggle(state->right[v]);
        state->reversed[v] = false;
    }
    void rotate(int v){
        int p = state->parent[v];
        int g = state->parent[p];
        bool dir = state->right[p] == v;
        int b = dir ? state->left[v] : state->right[v];
        if(g != -1){
            if(state->left[g] == p) state->left[g] = v;
            else state->right[g] = v;
        }
        state->parent[v] = g;
        if(dir){
            state->left[v] = p;
            state->right[p] = b;
        }else{
            state->right[v] = p;
            state->left[p] = b;
        }
        if(b != -1) state->parent[b] = p;
        state->parent[p] = v;
        pull(p);
        pull(v);
    }
    void splay(int v){
        if(v == -1) return;
        static std::array<int, MAX_SIZE> stack;
        int count = 0;
        for(int x = v; x != -1; x = state->parent[x]) stack[count++] = x;
        while(count > 0) push(stack[--count]);
        while(state->parent[v] != -1){
            int p = state->parent[v];
            int g = state->parent[p];
            if(g == -1){
                rotate(v);
            }else{
                bool zigzig = (state->left[g] == p) == (state->left[p] == v);
                if(zigzig){
                    rotate(p);
                    rotate(v);
                }else{
                    rotate(v);
                    rotate(v);
                }
            }
        }
        root_node = v;
    }
    int kth_node(int root, int k){
        int v = root;
        while(true){
            push(v);
            int ls = size_or_zero(state->left[v]);
            if(k < ls){
                v = state->left[v];
            }else if(k == ls){
                splay(v);
                return v;
            }else{
                k -= ls + 1;
                v = state->right[v];
            }
        }
    }
    std::pair<int, int> split_root(int root, int k){
        if(root == -1) return {-1, -1};
        if(k == 0) return {-1, root};
        if(k == size_or_zero(root)) return {root, -1};
        root_node = root;
        int v = kth_node(root, k);
        int a = state->left[v];
        state->left[v] = -1;
        state->parent[a] = -1;
        pull(v);
        return {a, v};
    }
    int merge_root(int a, int b){
        if(a == -1) return b;
        if(b == -1) return a;
        root_node = a;
        int v = kth_node(a, size_or_zero(a) - 1);
        state->right[v] = b;
        state->parent[b] = v;
        pull(v);
        return v;
    }
    int build(const std::vector<S>& values, int l, int r){
        if(l == r) return -1;
        int m = (l + r) >> 1;
        int v = new_node(values[static_cast<std::size_t>(m)]);
        int left = build(values, l, m);
        int right = build(values, m + 1, r);
        state->left[v] = left;
        state->right[v] = right;
        if(left != -1) state->parent[left] = v;
        if(right != -1) state->parent[right] = v;
        pull(v);
        return v;
    }

public:
    ReversibleSplayTree(): state(std::make_unique<State>()){}
    explicit ReversibleSplayTree(const std::vector<S>& values):
        ReversibleSplayTree(){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (constructor).");
        }
        root_node = build(values, 0, static_cast<int>(values.size()));
    }

    ReversibleSplayTree(const ReversibleSplayTree&) = delete;
    ReversibleSplayTree& operator=(const ReversibleSplayTree&) = delete;
    ReversibleSplayTree(ReversibleSplayTree&&) = default;
    ReversibleSplayTree& operator=(ReversibleSplayTree&&) = default;

    int size() const{ return size_or_zero(root_node); }
    bool empty() const{ return root_node == -1; }
    void clear(){
        root_node = -1;
        used = 0;
        free_count = 0;
    }

    void insert(int p, const S& x){
        check_insert_position(p, "library assertion fault: range violation (insert).");
        auto [a, b] = split_root(root_node, p);
        root_node = merge_root(merge_root(a, new_node(x)), b);
    }
    void push_front(const S& x){ insert(0, x); }
    void push_back(const S& x){ insert(size(), x); }

    void erase(int p){
        check_position(p, "library assertion fault: range violation (erase).");
        auto [a, b] = split_root(root_node, p);
        auto [c, d] = split_root(b, 1);
        state->free_stack[free_count++] = c;
        root_node = merge_root(a, d);
    }
    void set(int p, const S& x){
        check_position(p, "library assertion fault: range violation (set).");
        auto [a, b] = split_root(root_node, p);
        auto [c, d] = split_root(b, 1);
        state->value[c] = x;
        pull(c);
        root_node = merge_root(merge_root(a, c), d);
    }
    S get(int p){
        check_position(p, "library assertion fault: range violation (get).");
        auto [a, b] = split_root(root_node, p);
        auto [c, d] = split_root(b, 1);
        S result = state->value[c];
        root_node = merge_root(merge_root(a, c), d);
        return result;
    }
    void reverse(int l, int r){
        check_range(l, r, "library assertion fault: range violation (reverse).");
        auto [a, b] = split_root(root_node, l);
        auto [c, d] = split_root(b, r - l);
        toggle(c);
        root_node = merge_root(merge_root(a, c), d);
    }
    S prod(int l, int r){
        check_range(l, r, "library assertion fault: range violation (prod).");
        auto [a, b] = split_root(root_node, l);
        auto [c, d] = split_root(b, r - l);
        S result = aggregate_or_e(c);
        root_node = merge_root(merge_root(a, c), d);
        return result;
    }
    S all_prod() const{
        return aggregate_or_e(root_node);
    }
};
