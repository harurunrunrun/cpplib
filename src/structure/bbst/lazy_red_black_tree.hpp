#pragma once

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

template<class Key, auto MonoidActLen, int MAX_SIZE, class Compare = std::less<Key>>
struct LazyRedBlackTree{
    static_assert(MAX_SIZE > 0);
    using S = typename std::remove_cvref_t<decltype(MonoidActLen)>::S;
    using T = typename std::remove_cvref_t<decltype(MonoidActLen)>::T;

private:
    struct State{
        std::array<int, MAX_SIZE> left{};
        std::array<int, MAX_SIZE> right{};
        std::array<int, MAX_SIZE> size{};
        std::array<int, MAX_SIZE> free_stack{};
        std::array<bool, MAX_SIZE> red{};
        std::array<Key, MAX_SIZE> key;
        std::array<Key, MAX_SIZE> min_key;
        std::array<Key, MAX_SIZE> max_key;
        std::array<S, MAX_SIZE> value;
        std::array<S, MAX_SIZE> aggregate;
        std::array<T, MAX_SIZE> lazy;
    };

    int root_node = -1;
    int used = 0;
    int free_count = 0;
    Compare comp;
    std::unique_ptr<State> state;

    bool equal_key(const Key& a, const Key& b) const{
        return !comp(a, b) && !comp(b, a);
    }
    bool is_red(int v) const{
        return v != -1 && state->red[v];
    }
    int size_or_zero(int v) const{
        return v == -1 ? 0 : state->size[v];
    }
    int left_of(int v) const{
        return v == -1 ? -1 : state->left[v];
    }
    S aggregate_or_e(int v) const{
        return v == -1 ? MonoidActLen.e() : state->aggregate[v];
    }
    void pull(int v){
        int l = state->left[v], r = state->right[v];
        int ls = size_or_zero(l), rs = size_or_zero(r);
        state->size[v] = ls + 1 + rs;
        if(l != -1) state->min_key[v] = state->min_key[l];
        else state->min_key[v] = state->key[v];
        if(r != -1) state->max_key[v] = state->max_key[r];
        else state->max_key[v] = state->key[v];

        S prefix = MonoidActLen.op(aggregate_or_e(l), ls, state->value[v], 1);
        state->aggregate[v] = MonoidActLen.op(prefix, ls + 1, aggregate_or_e(r), rs);
    }
    void all_apply(int v, const T& f){
        if(v == -1) return;
        state->value[v] = MonoidActLen.mapping(f, state->value[v], 1);
        state->aggregate[v] = MonoidActLen.mapping(f, state->aggregate[v], state->size[v]);
        state->lazy[v] = MonoidActLen.composition(f, state->lazy[v]);
    }
    void push(int v){
        if(v == -1) return;
        all_apply(state->left[v], state->lazy[v]);
        all_apply(state->right[v], state->lazy[v]);
        state->lazy[v] = MonoidActLen.id();
    }
    int new_node(const Key& key, const S& value){
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
        state->red[v] = true;
        state->key[v] = key;
        state->min_key[v] = key;
        state->max_key[v] = key;
        state->value[v] = value;
        state->aggregate[v] = value;
        state->lazy[v] = MonoidActLen.id();
        return v;
    }
    void release_node(int v){
        state->free_stack[free_count++] = v;
    }
    int rotate_left(int h){
        push(h);
        int x = state->right[h];
        push(x);
        state->right[h] = state->left[x];
        state->left[x] = h;
        state->red[x] = state->red[h];
        state->red[h] = true;
        pull(h);
        pull(x);
        return x;
    }
    int rotate_right(int h){
        push(h);
        int x = state->left[h];
        push(x);
        state->left[h] = state->right[x];
        state->right[x] = h;
        state->red[x] = state->red[h];
        state->red[h] = true;
        pull(h);
        pull(x);
        return x;
    }
    void color_flip(int h){
        state->red[h] = !state->red[h];
        if(state->left[h] != -1) state->red[state->left[h]] = !state->red[state->left[h]];
        if(state->right[h] != -1) state->red[state->right[h]] = !state->red[state->right[h]];
    }
    int fix_up(int h){
        push(h);
        if(is_red(state->right[h]) && !is_red(state->left[h])) h = rotate_left(h);
        if(is_red(state->left[h]) && is_red(left_of(state->left[h]))) h = rotate_right(h);
        if(is_red(state->left[h]) && is_red(state->right[h])) color_flip(h);
        pull(h);
        return h;
    }
    int move_red_left(int h){
        push(h);
        color_flip(h);
        if(is_red(left_of(state->right[h]))){
            state->right[h] = rotate_right(state->right[h]);
            h = rotate_left(h);
            color_flip(h);
        }
        return h;
    }
    int move_red_right(int h){
        push(h);
        color_flip(h);
        if(is_red(left_of(state->left[h]))){
            h = rotate_right(h);
            color_flip(h);
        }
        return h;
    }
    int min_node(int h){
        while(true){
            push(h);
            if(state->left[h] == -1) return h;
            h = state->left[h];
        }
    }
    int insert_rec(int h, const Key& key, const S& value, bool& inserted){
        if(h == -1){
            inserted = true;
            return new_node(key, value);
        }
        push(h);
        if(comp(key, state->key[h])){
            state->left[h] = insert_rec(state->left[h], key, value, inserted);
        }else if(comp(state->key[h], key)){
            state->right[h] = insert_rec(state->right[h], key, value, inserted);
        }else{
            inserted = false;
        }
        return fix_up(h);
    }
    int erase_min(int h){
        push(h);
        if(state->left[h] == -1){
            release_node(h);
            return -1;
        }
        if(!is_red(state->left[h]) && !is_red(left_of(state->left[h]))){
            h = move_red_left(h);
        }
        state->left[h] = erase_min(state->left[h]);
        return fix_up(h);
    }
    int erase_rec(int h, const Key& key){
        push(h);
        if(comp(key, state->key[h])){
            if(state->left[h] != -1){
                if(!is_red(state->left[h]) && !is_red(left_of(state->left[h]))){
                    h = move_red_left(h);
                }
                state->left[h] = erase_rec(state->left[h], key);
            }
        }else{
            if(is_red(state->left[h])) h = rotate_right(h);
            if(equal_key(key, state->key[h]) && state->right[h] == -1){
                release_node(h);
                return -1;
            }
            if(state->right[h] != -1){
                if(!is_red(state->right[h]) && !is_red(left_of(state->right[h]))){
                    h = move_red_right(h);
                }
                if(equal_key(key, state->key[h])){
                    int m = min_node(state->right[h]);
                    state->key[h] = state->key[m];
                    state->value[h] = state->value[m];
                    state->right[h] = erase_min(state->right[h]);
                }else{
                    state->right[h] = erase_rec(state->right[h], key);
                }
            }
        }
        return fix_up(h);
    }
    bool disjoint(int v, const Key& l, const Key& r) const{
        return comp(state->max_key[v], l) || !comp(state->min_key[v], r);
    }
    bool covered(int v, const Key& l, const Key& r) const{
        return !comp(state->min_key[v], l) && comp(state->max_key[v], r);
    }
    bool in_range(const Key& key, const Key& l, const Key& r) const{
        return !comp(key, l) && comp(key, r);
    }
    std::pair<S, int> prod_rec(int v, const Key& l, const Key& r){
        if(v == -1 || disjoint(v, l, r)) return {MonoidActLen.e(), 0};
        if(covered(v, l, r)) return {state->aggregate[v], state->size[v]};
        push(v);
        auto [lv, ls] = prod_rec(state->left[v], l, r);
        S mid = MonoidActLen.e();
        int ms = 0;
        if(in_range(state->key[v], l, r)){
            mid = state->value[v];
            ms = 1;
        }
        auto [rv, rs] = prod_rec(state->right[v], l, r);
        pull(v);
        S prefix = MonoidActLen.op(lv, ls, mid, ms);
        return {MonoidActLen.op(prefix, ls + ms, rv, rs), ls + ms + rs};
    }
    void apply_rec(int v, const Key& l, const Key& r, const T& f){
        if(v == -1 || disjoint(v, l, r)) return;
        if(covered(v, l, r)){
            all_apply(v, f);
            return;
        }
        push(v);
        apply_rec(state->left[v], l, r, f);
        if(in_range(state->key[v], l, r)){
            state->value[v] = MonoidActLen.mapping(f, state->value[v], 1);
        }
        apply_rec(state->right[v], l, r, f);
        pull(v);
    }

public:
    explicit LazyRedBlackTree(const Compare& compare = Compare()):
        comp(compare), state(std::make_unique<State>()){}

    LazyRedBlackTree(const LazyRedBlackTree&) = delete;
    LazyRedBlackTree& operator=(const LazyRedBlackTree&) = delete;
    LazyRedBlackTree(LazyRedBlackTree&&) = default;
    LazyRedBlackTree& operator=(LazyRedBlackTree&&) = default;

    int size() const{ return size_or_zero(root_node); }
    bool empty() const{ return root_node == -1; }
    void clear(){
        root_node = -1;
        used = 0;
        free_count = 0;
    }

    bool contains(const Key& key) const{
        int v = root_node;
        while(v != -1){
            if(equal_key(state->key[v], key)) return true;
            v = comp(key, state->key[v]) ? state->left[v] : state->right[v];
        }
        return false;
    }
    bool insert(const Key& key, const S& value){
        bool inserted = false;
        root_node = insert_rec(root_node, key, value, inserted);
        state->red[root_node] = false;
        return inserted;
    }
    bool erase(const Key& key){
        if(!contains(key)) return false;
        if(!is_red(state->left[root_node]) && !is_red(state->right[root_node])){
            state->red[root_node] = true;
        }
        root_node = erase_rec(root_node, key);
        if(root_node != -1) state->red[root_node] = false;
        return true;
    }
    std::optional<S> get(const Key& key){
        int v = root_node;
        while(v != -1){
            push(v);
            if(equal_key(state->key[v], key)) return state->value[v];
            v = comp(key, state->key[v]) ? state->left[v] : state->right[v];
        }
        return std::nullopt;
    }
    bool set(const Key& key, const S& value){
        int v = root_node;
        std::array<int, MAX_SIZE> path;
        int count = 0;
        while(v != -1){
            push(v);
            path[count++] = v;
            if(equal_key(state->key[v], key)){
                state->value[v] = value;
                while(count > 0) pull(path[--count]);
                return true;
            }
            v = comp(key, state->key[v]) ? state->left[v] : state->right[v];
        }
        return false;
    }
    void apply(const Key& l, const Key& r, const T& f){
        if(root_node == -1 || !comp(l, r)) return;
        apply_rec(root_node, l, r, f);
    }
    S prod(const Key& l, const Key& r){
        if(root_node == -1 || !comp(l, r)) return MonoidActLen.e();
        return prod_rec(root_node, l, r).first;
    }
    void all_apply(const T& f){
        all_apply(root_node, f);
    }
    S all_prod() const{
        return aggregate_or_e(root_node);
    }

    int order_of_key(const Key& key) const{
        int v = root_node;
        int result = 0;
        while(v != -1){
            if(comp(state->key[v], key)){
                result += size_or_zero(state->left[v]) + 1;
                v = state->right[v];
            }else{
                v = state->left[v];
            }
        }
        return result;
    }
    std::optional<Key> kth_key(int k) const{
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
};
