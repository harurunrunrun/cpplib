#ifndef CPPLIB_SRC_STRUCTURE_BBST_RED_BLACK_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_RED_BLACK_TREE_HPP_INCLUDED

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>

template<class Key, int MAX_SIZE, class Compare = std::less<Key>>
struct RedBlackSet{
    static_assert(MAX_SIZE > 0);

private:
    struct State{
        std::array<int, MAX_SIZE> left{};
        std::array<int, MAX_SIZE> right{};
        std::array<int, MAX_SIZE> size{};
        std::array<int, MAX_SIZE> free_stack{};
        std::array<bool, MAX_SIZE> red{};
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
    bool is_red(int v) const{
        return v != -1 && state->red[v];
    }
    int size_or_zero(int v) const{
        return v == -1 ? 0 : state->size[v];
    }
    int left_of(int v) const{
        return v == -1 ? -1 : state->left[v];
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
        state->left[v] = state->right[v] = -1;
        state->size[v] = 1;
        state->red[v] = true;
        state->key[v] = x;
        return v;
    }
    void release_node(int v){
        state->free_stack[free_count++] = v;
    }
    int rotate_left(int h){
        int x = state->right[h];
        state->right[h] = state->left[x];
        state->left[x] = h;
        state->red[x] = state->red[h];
        state->red[h] = true;
        pull(h);
        pull(x);
        return x;
    }
    int rotate_right(int h){
        int x = state->left[h];
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
        if(is_red(state->right[h]) && !is_red(state->left[h])) h = rotate_left(h);
        if(is_red(state->left[h]) && is_red(left_of(state->left[h]))) h = rotate_right(h);
        if(is_red(state->left[h]) && is_red(state->right[h])) color_flip(h);
        pull(h);
        return h;
    }
    int move_red_left(int h){
        color_flip(h);
        if(is_red(left_of(state->right[h]))){
            state->right[h] = rotate_right(state->right[h]);
            h = rotate_left(h);
            color_flip(h);
        }
        return h;
    }
    int move_red_right(int h){
        color_flip(h);
        if(is_red(left_of(state->left[h]))){
            h = rotate_right(h);
            color_flip(h);
        }
        return h;
    }
    int min_node(int h) const{
        while(state->left[h] != -1) h = state->left[h];
        return h;
    }
    int insert_rec(int h, const Key& x, bool& inserted){
        if(h == -1){
            inserted = true;
            return new_node(x);
        }
        if(comp(x, state->key[h])){
            state->left[h] = insert_rec(state->left[h], x, inserted);
        }else if(comp(state->key[h], x)){
            state->right[h] = insert_rec(state->right[h], x, inserted);
        }else{
            inserted = false;
        }
        return fix_up(h);
    }
    int erase_min(int h){
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
    int erase_rec(int h, const Key& x){
        if(comp(x, state->key[h])){
            if(state->left[h] != -1){
                if(!is_red(state->left[h]) && !is_red(left_of(state->left[h]))){
                    h = move_red_left(h);
                }
                state->left[h] = erase_rec(state->left[h], x);
            }
        }else{
            if(is_red(state->left[h])) h = rotate_right(h);
            if(equal_key(x, state->key[h]) && state->right[h] == -1){
                release_node(h);
                return -1;
            }
            if(state->right[h] != -1){
                if(!is_red(state->right[h]) && !is_red(left_of(state->right[h]))){
                    h = move_red_right(h);
                }
                if(equal_key(x, state->key[h])){
                    int m = min_node(state->right[h]);
                    state->key[h] = state->key[m];
                    state->right[h] = erase_min(state->right[h]);
                }else{
                    state->right[h] = erase_rec(state->right[h], x);
                }
            }
        }
        return fix_up(h);
    }

public:
    explicit RedBlackSet(const Compare& compare = Compare()):
        comp(compare), state(std::make_unique<State>()){}

    RedBlackSet(const RedBlackSet&) = delete;
    RedBlackSet& operator=(const RedBlackSet&) = delete;
    RedBlackSet(RedBlackSet&&) = default;
    RedBlackSet& operator=(RedBlackSet&&) = default;

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
        bool inserted = false;
        root_node = insert_rec(root_node, x, inserted);
        state->red[root_node] = false;
        return inserted;
    }
    bool erase(const Key& x){
        if(!contains(x)) return false;
        if(!is_red(state->left[root_node]) && !is_red(state->right[root_node])){
            state->red[root_node] = true;
        }
        root_node = erase_rec(root_node, x);
        if(root_node != -1) state->red[root_node] = false;
        return true;
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

#endif  // CPPLIB_SRC_STRUCTURE_BBST_RED_BLACK_TREE_HPP_INCLUDED
