#ifndef CPPLIB_SRC_STRUCTURE_TREE_TREAP_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_TREAP_HPP_INCLUDED

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

template<class Key, int MAX_SIZE, class Compare = std::less<Key>>
struct TreapSet{
    static_assert(MAX_SIZE > 0);

private:
    struct State{
        std::array<int, MAX_SIZE> left{};
        std::array<int, MAX_SIZE> right{};
        std::array<int, MAX_SIZE> size{};
        std::array<int, MAX_SIZE> free_stack{};
        std::array<std::uint64_t, MAX_SIZE> priority{};
        std::array<Key, MAX_SIZE> key;
    };

    int root_node = -1;
    int used = 0;
    int free_count = 0;
    std::uint64_t rng_state;
    Compare comp;
    std::unique_ptr<State> state;

    bool equal_key(const Key& a, const Key& b) const{
        return !comp(a, b) && !comp(b, a);
    }
    int size_or_zero(int v) const{
        return v == -1 ? 0 : state->size[v];
    }
    void pull(int v){
        state->size[v] = 1 + size_or_zero(state->left[v]) + size_or_zero(state->right[v]);
    }
    std::uint64_t next_priority(){
        rng_state += 0x9e3779b97f4a7c15ULL;
        std::uint64_t z = rng_state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }
    int new_node(const Key& x){
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
        state->key[v] = x;
        return v;
    }
    int merge(int a, int b){
        if(a == -1) return b;
        if(b == -1) return a;
        if(state->priority[a] < state->priority[b]){
            state->right[a] = merge(state->right[a], b);
            pull(a);
            return a;
        }else{
            state->left[b] = merge(a, state->left[b]);
            pull(b);
            return b;
        }
    }
    std::pair<int, int> split(int v, const Key& x){
        if(v == -1) return {-1, -1};
        if(comp(state->key[v], x)){
            auto [a, b] = split(state->right[v], x);
            state->right[v] = a;
            pull(v);
            return {v, b};
        }else{
            auto [a, b] = split(state->left[v], x);
            state->left[v] = b;
            pull(v);
            return {a, v};
        }
    }
    bool erase_node(int& v, const Key& x){
        if(v == -1) return false;
        if(equal_key(state->key[v], x)){
            int old = v;
            v = merge(state->left[v], state->right[v]);
            state->free_stack[free_count++] = old;
            return true;
        }
        bool erased;
        if(comp(x, state->key[v])) erased = erase_node(state->left[v], x);
        else erased = erase_node(state->right[v], x);
        if(erased) pull(v);
        return erased;
    }

public:
    explicit TreapSet(
        std::uint64_t seed = 0x123456789abcdef0ULL,
        const Compare& compare = Compare()
    ): rng_state(seed), comp(compare), state(std::make_unique<State>()){}

    TreapSet(const TreapSet&) = delete;
    TreapSet& operator=(const TreapSet&) = delete;
    TreapSet(TreapSet&&) = default;
    TreapSet& operator=(TreapSet&&) = default;

    int size() const{ return size_or_zero(root_node); }
    bool empty() const{ return root_node == -1; }
    void clear(){
        root_node = -1;
        used = 0;
        free_count = 0;
    }

    bool contains(const Key& x) const{
        int v = root_node;
        while(v != -1){
            if(equal_key(state->key[v], x)) return true;
            v = comp(x, state->key[v]) ? state->left[v] : state->right[v];
        }
        return false;
    }
    bool insert(const Key& x){
        if(contains(x)) return false;
        auto [l, r] = split(root_node, x);
        root_node = merge(merge(l, new_node(x)), r);
        return true;
    }
    bool erase(const Key& x){
        return erase_node(root_node, x);
    }

    int order_of_key(const Key& x) const{
        int v = root_node;
        int result = 0;
        while(v != -1){
            if(comp(state->key[v], x)){
                result += size_or_zero(state->left[v]) + 1;
                v = state->right[v];
            }else{
                v = state->left[v];
            }
        }
        return result;
    }
    int order_of_key_upper(const Key& x) const{
        int v = root_node;
        int result = 0;
        while(v != -1){
            if(!comp(x, state->key[v])){
                result += size_or_zero(state->left[v]) + 1;
                v = state->right[v];
            }else{
                v = state->left[v];
            }
        }
        return result;
    }
    std::optional<Key> kth(int k) const{
        if(k < 0 || size() <= k) return std::nullopt;
        int v = root_node;
        while(v != -1){
            int ls = size_or_zero(state->left[v]);
            if(k < ls){
                v = state->left[v];
            }else if(k == ls){
                return state->key[v];
            }else{
                k -= ls + 1;
                v = state->right[v];
            }
        }
        return std::nullopt;
    }
    std::optional<Key> lower_bound(const Key& x) const{
        int v = root_node, best = -1;
        while(v != -1){
            if(!comp(state->key[v], x)){
                best = v;
                v = state->left[v];
            }else{
                v = state->right[v];
            }
        }
        if(best == -1) return std::nullopt;
        return state->key[best];
    }
    std::optional<Key> upper_bound(const Key& x) const{
        int v = root_node, best = -1;
        while(v != -1){
            if(comp(x, state->key[v])){
                best = v;
                v = state->left[v];
            }else{
                v = state->right[v];
            }
        }
        if(best == -1) return std::nullopt;
        return state->key[best];
    }
    std::optional<Key> max_leq(const Key& x) const{
        int v = root_node, best = -1;
        while(v != -1){
            if(!comp(x, state->key[v])){
                best = v;
                v = state->right[v];
            }else{
                v = state->left[v];
            }
        }
        if(best == -1) return std::nullopt;
        return state->key[best];
    }
    std::optional<Key> max_less(const Key& x) const{
        int v = root_node, best = -1;
        while(v != -1){
            if(comp(state->key[v], x)){
                best = v;
                v = state->right[v];
            }else{
                v = state->left[v];
            }
        }
        if(best == -1) return std::nullopt;
        return state->key[best];
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TREE_TREAP_HPP_INCLUDED
