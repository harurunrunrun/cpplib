#ifndef CPPLIB_SRC_STRUCTURE_TREE_IMPLICIT_TREAP_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_IMPLICIT_TREAP_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<auto Monoid, int MAX_SIZE>
struct ImplicitTreap{
    static_assert(MAX_SIZE > 0);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

private:
    struct State{
        std::array<int, MAX_SIZE> left{};
        std::array<int, MAX_SIZE> right{};
        std::array<int, MAX_SIZE> size{};
        std::array<int, MAX_SIZE> free_stack{};
        std::array<std::uint64_t, MAX_SIZE> priority{};
        std::array<bool, MAX_SIZE> reversed{};
        std::array<S, MAX_SIZE> value;
        std::array<S, MAX_SIZE> aggregate;
        std::array<S, MAX_SIZE> rev_aggregate;
    };

    int root_node = -1;
    int used = 0;
    int free_count = 0;
    std::uint64_t rng_state;
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
    std::uint64_t next_priority(){
        rng_state += 0x9e3779b97f4a7c15ULL;
        std::uint64_t z = rng_state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
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
        state->priority[v] = next_priority();
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
    int merge(int a, int b){
        if(a == -1) return b;
        if(b == -1) return a;
        if(state->priority[a] < state->priority[b]){
            push(a);
            state->right[a] = merge(state->right[a], b);
            pull(a);
            return a;
        }else{
            push(b);
            state->left[b] = merge(a, state->left[b]);
            pull(b);
            return b;
        }
    }
    std::pair<int, int> split(int v, int k){
        if(v == -1) return {-1, -1};
        push(v);
        int ls = size_or_zero(state->left[v]);
        if(k <= ls){
            auto [a, b] = split(state->left[v], k);
            state->left[v] = b;
            pull(v);
            return {a, v};
        }else{
            auto [a, b] = split(state->right[v], k - ls - 1);
            state->right[v] = a;
            pull(v);
            return {v, b};
        }
    }
    void rebuild(int root){
        if(root == -1) return;
        std::vector<int> order;
        order.reserve(static_cast<std::size_t>(used));
        std::vector<int> stack = {root};
        while(!stack.empty()){
            int v = stack.back();
            stack.pop_back();
            order.push_back(v);
            if(state->left[v] != -1) stack.push_back(state->left[v]);
            if(state->right[v] != -1) stack.push_back(state->right[v]);
        }
        for(int i = static_cast<int>(order.size()) - 1; i >= 0; i--) pull(order[static_cast<std::size_t>(i)]);
    }

public:
    explicit ImplicitTreap(std::uint64_t seed = 0x123456789abcdef0ULL):
        rng_state(seed), state(std::make_unique<State>()){}
    explicit ImplicitTreap(const std::vector<S>& values, std::uint64_t seed = 0x123456789abcdef0ULL):
        ImplicitTreap(seed){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (constructor).");
        }
        std::vector<int> stack;
        stack.reserve(values.size());
        for(const S& x: values){
            int v = new_node(x);
            int last = -1;
            while(!stack.empty() && state->priority[v] < state->priority[stack.back()]){
                last = stack.back();
                stack.pop_back();
            }
            state->left[v] = last;
            if(!stack.empty()) state->right[stack.back()] = v;
            stack.push_back(v);
        }
        if(!stack.empty()){
            root_node = stack.front();
            rebuild(root_node);
        }
    }

    ImplicitTreap(const ImplicitTreap&) = delete;
    ImplicitTreap& operator=(const ImplicitTreap&) = delete;
    ImplicitTreap(ImplicitTreap&&) = default;
    ImplicitTreap& operator=(ImplicitTreap&&) = default;

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

#endif  // CPPLIB_SRC_STRUCTURE_TREE_IMPLICIT_TREAP_HPP_INCLUDED
