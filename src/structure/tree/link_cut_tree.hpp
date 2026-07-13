#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<auto Monoid, int MAX_SIZE>
struct LinkCutTree{
    static_assert(MAX_SIZE > 0);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

private:
    struct State{
        std::array<int, MAX_SIZE> left{};
        std::array<int, MAX_SIZE> right{};
        std::array<int, MAX_SIZE> parent{};
        std::array<int, MAX_SIZE> size{};
        std::array<bool, MAX_SIZE> reversed{};
        std::array<S, MAX_SIZE> value;
        std::array<S, MAX_SIZE> aggregate;
        std::array<S, MAX_SIZE> rev_aggregate;
    };

    int _n = 0;
    std::unique_ptr<State> state;

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _n <= v)[[unlikely]] throw std::runtime_error(message);
    }
    int size_or_zero(int v) const{
        return v == -1 ? 0 : state->size[v];
    }
    S aggregate_or_e(int v) const{
        return v == -1 ? Monoid.e() : state->aggregate[v];
    }
    S rev_aggregate_or_e(int v) const{
        return v == -1 ? Monoid.e() : state->rev_aggregate[v];
    }
    bool is_splay_root(int v) const{
        int p = state->parent[v];
        return p == -1 || (state->left[p] != v && state->right[p] != v);
    }
    void pull(int v){
        state->size[v] = size_or_zero(state->left[v]) + 1 + size_or_zero(state->right[v]);
        state->aggregate[v] = Monoid.op(
            aggregate_or_e(state->left[v]),
            Monoid.op(state->value[v], aggregate_or_e(state->right[v]))
        );
        state->rev_aggregate[v] = Monoid.op(
            rev_aggregate_or_e(state->right[v]),
            Monoid.op(state->value[v], rev_aggregate_or_e(state->left[v]))
        );
    }
    void toggle(int v){
        if(v == -1) return;
        std::swap(state->left[v], state->right[v]);
        std::swap(state->aggregate[v], state->rev_aggregate[v]);
        state->reversed[v] = !state->reversed[v];
    }
    void push(int v){
        if(!state->reversed[v]) return;
        toggle(state->left[v]);
        toggle(state->right[v]);
        state->reversed[v] = false;
    }
    void rotate(int v){
        int p = state->parent[v];
        int g = state->parent[p];
        bool dir = state->right[p] == v;
        int b = dir ? state->left[v] : state->right[v];
        if(!is_splay_root(p)){
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
        static std::array<int, MAX_SIZE> stack;
        int count = 0;
        int x = v;
        stack[count++] = x;
        while(!is_splay_root(x)){
            x = state->parent[x];
            stack[count++] = x;
        }
        while(count > 0) push(stack[--count]);
        while(!is_splay_root(v)){
            int p = state->parent[v];
            int g = state->parent[p];
            if(!is_splay_root(p)){
                bool zigzig = (state->left[p] == v) == (state->left[g] == p);
                rotate(zigzig ? p : v);
            }
            rotate(v);
        }
    }
    int access(int v){
        int last = -1;
        for(int x = v; x != -1; x = state->parent[x]){
            splay(x);
            state->right[x] = last;
            pull(x);
            last = x;
        }
        splay(v);
        return last;
    }
    void make_root_unchecked(int v){
        access(v);
        toggle(v);
    }
    int find_root_unchecked(int v){
        access(v);
        while(true){
            push(v);
            if(state->left[v] == -1) break;
            v = state->left[v];
        }
        splay(v);
        return v;
    }
    bool expose_path_unchecked(int u, int v){
        make_root_unchecked(u);
        if(find_root_unchecked(v) != u) return false;
        access(v);
        return true;
    }
    int kth_in_auxiliary_tree(int v, int k){
        while(true){
            push(v);
            const int left_size = size_or_zero(state->left[v]);
            if(k < left_size){
                v = state->left[v];
            }else if(k == left_size){
                splay(v);
                return v;
            }else{
                k -= left_size + 1;
                v = state->right[v];
            }
        }
    }

public:
    explicit LinkCutTree(int n = MAX_SIZE): _n(n), state(nullptr){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        state = std::make_unique<State>();
        for(int k = 0; k < MAX_SIZE; k++){
            state->left[k] = state->right[k] = state->parent[k] = -1;
            state->size[k] = 1;
            state->reversed[k] = false;
            state->value[k] = Monoid.e();
            state->aggregate[k] = Monoid.e();
            state->rev_aggregate[k] = Monoid.e();
        }
    }
    explicit LinkCutTree(const std::vector<S>& values):
        LinkCutTree(static_cast<int>(values.size())){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        for(int k = 0; k < _n; k++){
            state->value[k] = values[static_cast<std::size_t>(k)];
            state->aggregate[k] = state->value[k];
            state->rev_aggregate[k] = state->value[k];
        }
    }

    LinkCutTree(const LinkCutTree&) = delete;
    LinkCutTree& operator=(const LinkCutTree&) = delete;
    LinkCutTree(LinkCutTree&&) = default;
    LinkCutTree& operator=(LinkCutTree&&) = default;

    int size() const{ return _n; }

    void evert(int v){
        check_vertex(v, "library assertion fault: range violation (evert).");
        make_root_unchecked(v);
    }
    int root(int v){
        check_vertex(v, "library assertion fault: range violation (root).");
        return find_root_unchecked(v);
    }
    bool connected(int u, int v){
        check_vertex(u, "library assertion fault: range violation (connected).");
        check_vertex(v, "library assertion fault: range violation (connected).");
        if(u == v) return true;
        return find_root_unchecked(u) == find_root_unchecked(v);
    }

    bool link(int u, int v){
        check_vertex(u, "library assertion fault: range violation (link).");
        check_vertex(v, "library assertion fault: range violation (link).");
        make_root_unchecked(u);
        if(find_root_unchecked(v) == u) return false;
        state->parent[u] = v;
        return true;
    }
    bool cut(int u, int v){
        check_vertex(u, "library assertion fault: range violation (cut).");
        check_vertex(v, "library assertion fault: range violation (cut).");
        make_root_unchecked(u);
        access(v);
        if(state->left[v] != u || state->right[u] != -1) return false;
        state->left[v] = -1;
        state->parent[u] = -1;
        pull(v);
        return true;
    }

    void set(int v, const S& x){
        check_vertex(v, "library assertion fault: range violation (set).");
        access(v);
        state->value[v] = x;
        pull(v);
    }
    S get(int v){
        check_vertex(v, "library assertion fault: range violation (get).");
        access(v);
        return state->value[v];
    }
    S path_prod(int u, int v){
        check_vertex(u, "library assertion fault: range violation (path_prod).");
        check_vertex(v, "library assertion fault: range violation (path_prod).");
        make_root_unchecked(u);
        access(v);
        return state->aggregate[v];
    }
    int path_size(int u, int v){
        check_vertex(u, "library assertion fault: range violation (path_size).");
        check_vertex(v, "library assertion fault: range violation (path_size).");
        if(!expose_path_unchecked(u, v)) return -1;
        return state->size[v];
    }
    int kth_on_path(int u, int v, int k){
        check_vertex(u, "library assertion fault: range violation (kth_on_path).");
        check_vertex(v, "library assertion fault: range violation (kth_on_path).");
        if(k < 0 || !expose_path_unchecked(u, v) || state->size[v] <= k) return -1;
        return kth_in_auxiliary_tree(v, k);
    }
    int jump(int u, int v, int k){
        return kth_on_path(u, v, k);
    }
};
