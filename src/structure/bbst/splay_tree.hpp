#ifndef CPPLIB_SRC_STRUCTURE_BBST_SPLAY_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_SPLAY_TREE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>

template<class Key, int MAX_SIZE, class Compare = std::less<Key>>
struct SplaySet{
    static_assert(MAX_SIZE > 0);

private:
    struct State{
        std::array<int, MAX_SIZE> left{};
        std::array<int, MAX_SIZE> right{};
        std::array<int, MAX_SIZE> parent{};
        std::array<int, MAX_SIZE> size{};
        std::array<int, MAX_SIZE> free_stack{};
        std::array<Key, MAX_SIZE> key;
    };

    int root_node = -1;
    int used = 0;
    int free_count = 0;
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
        state->left[v] = state->right[v] = state->parent[v] = -1;
        state->size[v] = 1;
        state->key[v] = x;
        return v;
    }
    void release_node(int v){
        state->free_stack[free_count++] = v;
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
    int find_node(const Key& x){
        int v = root_node;
        int last = -1;
        while(v != -1){
            last = v;
            if(equal_key(state->key[v], x)){
                splay(v);
                return v;
            }
            v = comp(x, state->key[v]) ? state->left[v] : state->right[v];
        }
        splay(last);
        return -1;
    }

public:
    explicit SplaySet(const Compare& compare = Compare()):
        comp(compare), state(std::make_unique<State>()){}

    SplaySet(const SplaySet&) = delete;
    SplaySet& operator=(const SplaySet&) = delete;
    SplaySet(SplaySet&&) = default;
    SplaySet& operator=(SplaySet&&) = default;

    int size() const{ return size_or_zero(root_node); }
    bool empty() const{ return root_node == -1; }
    void clear(){
        root_node = -1;
        used = 0;
        free_count = 0;
    }

    bool contains(const Key& x){
        return find_node(x) != -1;
    }
    bool insert(const Key& x){
        if(root_node == -1){
            root_node = new_node(x);
            return true;
        }
        int v = root_node;
        int p = -1;
        while(v != -1){
            p = v;
            if(equal_key(state->key[v], x)){
                splay(v);
                return false;
            }
            v = comp(x, state->key[v]) ? state->left[v] : state->right[v];
        }
        int node = new_node(x);
        state->parent[node] = p;
        if(comp(x, state->key[p])) state->left[p] = node;
        else state->right[p] = node;
        splay(node);
        return true;
    }
    bool erase(const Key& x){
        int v = find_node(x);
        if(v == -1) return false;
        int l = state->left[v];
        int r = state->right[v];
        if(l != -1) state->parent[l] = -1;
        if(r != -1) state->parent[r] = -1;
        release_node(v);
        if(l == -1){
            root_node = r;
            return true;
        }
        root_node = l;
        int mx = l;
        while(state->right[mx] != -1) mx = state->right[mx];
        splay(mx);
        state->right[root_node] = r;
        if(r != -1) state->parent[r] = root_node;
        pull(root_node);
        return true;
    }

    int order_of_key(const Key& x){
        int v = root_node;
        int last = -1;
        int result = 0;
        while(v != -1){
            last = v;
            if(comp(state->key[v], x)){
                result += size_or_zero(state->left[v]) + 1;
                v = state->right[v];
            }else{
                v = state->left[v];
            }
        }
        splay(last);
        return result;
    }
    int order_of_key_upper(const Key& x){
        int v = root_node;
        int last = -1;
        int result = 0;
        while(v != -1){
            last = v;
            if(!comp(x, state->key[v])){
                result += size_or_zero(state->left[v]) + 1;
                v = state->right[v];
            }else{
                v = state->left[v];
            }
        }
        splay(last);
        return result;
    }
    std::optional<Key> kth(int k){
        if(k < 0 || size() <= k) return std::nullopt;
        int v = root_node;
        while(v != -1){
            int ls = size_or_zero(state->left[v]);
            if(k < ls){
                v = state->left[v];
            }else if(k == ls){
                Key result = state->key[v];
                splay(v);
                return result;
            }else{
                k -= ls + 1;
                v = state->right[v];
            }
        }
        return std::nullopt;
    }
    std::optional<Key> lower_bound(const Key& x){
        int v = root_node, best = -1, last = -1;
        while(v != -1){
            last = v;
            if(!comp(state->key[v], x)){
                best = v;
                v = state->left[v];
            }else{
                v = state->right[v];
            }
        }
        int target = best == -1 ? last : best;
        std::optional<Key> result = best == -1 ? std::nullopt : std::optional<Key>(state->key[best]);
        splay(target);
        return result;
    }
    std::optional<Key> upper_bound(const Key& x){
        int v = root_node, best = -1, last = -1;
        while(v != -1){
            last = v;
            if(comp(x, state->key[v])){
                best = v;
                v = state->left[v];
            }else{
                v = state->right[v];
            }
        }
        int target = best == -1 ? last : best;
        std::optional<Key> result = best == -1 ? std::nullopt : std::optional<Key>(state->key[best]);
        splay(target);
        return result;
    }
    std::optional<Key> max_leq(const Key& x){
        int v = root_node, best = -1, last = -1;
        while(v != -1){
            last = v;
            if(!comp(x, state->key[v])){
                best = v;
                v = state->right[v];
            }else{
                v = state->left[v];
            }
        }
        int target = best == -1 ? last : best;
        std::optional<Key> result = best == -1 ? std::nullopt : std::optional<Key>(state->key[best]);
        splay(target);
        return result;
    }
    std::optional<Key> max_less(const Key& x){
        int v = root_node, best = -1, last = -1;
        while(v != -1){
            last = v;
            if(comp(state->key[v], x)){
                best = v;
                v = state->right[v];
            }else{
                v = state->left[v];
            }
        }
        int target = best == -1 ? last : best;
        std::optional<Key> result = best == -1 ? std::nullopt : std::optional<Key>(state->key[best]);
        splay(target);
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_BBST_SPLAY_TREE_HPP_INCLUDED
