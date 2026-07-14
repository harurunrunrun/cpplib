#pragma once
#include <vector>
#include <stdexcept>
#include <type_traits>
#include <functional>
#include <limits>
#include <algorithm>
#include <utility>
#include "../types/monoid_act_len.hpp"


template<auto Monoid, long long MAX_SIZE>
struct DynamicLazySegtree{
    static_assert(0 < MAX_SIZE && MAX_SIZE < 0x3FFFFFFFFFFFFFFF);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;
    using T = typename std::remove_cvref_t<decltype(Monoid)>::T;
    


    private:
        struct Node{
            S val = Monoid.e();
            T lazy = Monoid.id();
            Node* left = nullptr;
            Node* right = nullptr;
            long long len;
            explicit Node(long long node_length):len(node_length){}
        };

        void delete_subtree(Node* node){
            if(node == nullptr) return;
            delete_subtree(node->left);
            delete_subtree(node->right);
            delete node;
        }

        Node* root;
        long long _n;

        static S eval_node(const Node* node, const T& lazy_acc, long long len){
            if(node == nullptr){
                return Monoid.mapping(lazy_acc, Monoid.e(), len);
            }
            return Monoid.mapping(lazy_acc, node->val, node->len);
        }

        static T next_lazy_acc(const Node* node, const T& lazy_acc){
            if(node == nullptr){
                return lazy_acc;
            }
            return Monoid.composition(lazy_acc, node->lazy);
        }

        void update(Node* node, long long l, long long r){
            long long mid = (l + r) >> 1;
            S left = Monoid.e();
            S right = Monoid.e();
            if(node->left != nullptr){
                left = node->left->val;
            }
            if(node->right != nullptr){
                right = node->right->val;
            }
            node->val = Monoid.op(left, mid - l, right, r - mid);
        }

        void all_apply(Node* node, T f){
            node->val = Monoid.mapping(f, node->val, node->len);
            node->lazy = Monoid.composition(f, node->lazy);
        }

        void push(Node* node, long long l, long long r){
            long long mid = (l + r) >> 1;

            if(node->left == nullptr){
                node->left = new Node(mid - l);
            }
            if(node->right == nullptr){
                node->right = new Node(r - mid);
            }
            all_apply(node->left, node->lazy);
            all_apply(node->right, node->lazy);
            node->lazy = Monoid.id();
        }

        void internal_set(Node* node, long long l, long long r, long long idx, const S& x){
            if(r - l == 1){
                node->val = x;
                return;
            }
            push(node, l, r);
            long long mid = (l + r) >> 1;
            if(idx < mid){
                internal_set(node->left, l, mid, idx, x);
            }else{
                internal_set(node->right, mid, r, idx, x);
            }
            update(node, l, r);
        }

        S internal_get(const Node* node, long long l, long long r, long long idx, const T& lazy_acc)const{
            if(r - l == 1){
                return eval_node(node, lazy_acc, r - l);
            }
            long long mid = (l + r) >> 1;
            T nxt_lazy_acc = next_lazy_acc(node, lazy_acc);
            if(idx < mid){
                return internal_get(node == nullptr ? nullptr : node->left, l, mid, idx, nxt_lazy_acc);
            }else{
                return internal_get(node == nullptr ? nullptr : node->right, mid, r, idx, nxt_lazy_acc);
            }
        }

        std::pair<S, long long> internal_prod(const Node* node, long long now_l, long long now_r, long long q_l, long long q_r, const T& lazy_acc)const{
            if(now_r <= q_l || q_r <= now_l){
                return {Monoid.e(), 0};
            }
            if(q_l <= now_l && now_r <= q_r){
                return {eval_node(node, lazy_acc, now_r - now_l), now_r - now_l};
            }
            long long mid = (now_l + now_r) >> 1;
            T nxt_lazy_acc = next_lazy_acc(node, lazy_acc);

            auto [left, left_len] = internal_prod(
                node == nullptr ? nullptr : node->left,
                now_l, mid, q_l, q_r, nxt_lazy_acc
            );

            auto [right, right_len] = internal_prod(
                node == nullptr ? nullptr : node->right,
                mid, now_r, q_l, q_r, nxt_lazy_acc
            );
            
            return {Monoid.op(left, left_len, right, right_len), left_len + right_len};
        }

        void internal_apply(Node* node, long long now_l, long long now_r, long long idx, const T& f){
            if(now_r - now_l == 1){
                node->val = Monoid.mapping(f, node->val, node->len);
                return;
            }
            push(node, now_l, now_r);
            long long mid = (now_l + now_r) >> 1;
            if(idx < mid){
                internal_apply(node->left, now_l, mid, idx, f);
            }else{
                internal_apply(node->right, mid, now_r, idx, f);
            }
            update(node, now_l, now_r);
        }

        void internal_apply(Node* node, long long now_l, long long now_r, long long q_l, long long q_r, const T& f){
            if(now_r <= q_l || q_r <= now_l){
                return;
            }
            if(q_l <= now_l && now_r <= q_r){
                all_apply(node, f);
                return;
            }
            push(node, now_l, now_r);
            long long mid = (now_l + now_r) >> 1;
            internal_apply(node->left, now_l, mid, q_l, q_r, f);
            internal_apply(node->right, mid, now_r, q_l, q_r, f);
            update(node, now_l, now_r);
        }

        long long internal_max_right(const Node* node, long long now_l, long long now_r, S& acc, long long& acc_len, long long l, auto g, const T& lazy_acc, auto&&... args)const{
            if(now_r <= l){
                return _n;
            }
            if(l <= now_l){
                S cur = eval_node(node, lazy_acc, now_r - now_l);
                S nxt = Monoid.op(acc, acc_len, cur, now_r - now_l);
                if(std::invoke(g, nxt, args...)){
                    acc = nxt;
                    acc_len += now_r - now_l;
                    return _n;
                }
                if(now_r - now_l == 1){
                    return now_l;
                }
            }
            long long mid = (now_l + now_r) >> 1;
            T nxt_lazy_acc = next_lazy_acc(node, lazy_acc);
            long long res = internal_max_right(node == nullptr ? nullptr : node->left, now_l, mid, acc, acc_len, l, g, nxt_lazy_acc, args...);
            if(res != _n){
                return res;
            }
            return internal_max_right(node == nullptr ? nullptr : node->right, mid, now_r, acc, acc_len, l, g, nxt_lazy_acc, args...);
        }

        long long internal_min_left(const Node* node, long long now_l, long long now_r, S& acc, long long& acc_len, long long r, auto g, const T& lazy_acc, auto&&... args)const{
            if(r <= now_l){
                return -1;
            }
            if(now_r <= r){
                S cur = eval_node(node, lazy_acc, now_r - now_l);
                S nxt = Monoid.op(cur, now_r - now_l, acc, acc_len);
                if(std::invoke(g, nxt, args...)){
                    acc = nxt;
                    acc_len += now_r - now_l;
                    return -1;
                }
                if(now_r - now_l == 1){
                    return now_r;
                }
            }
            long long mid = (now_l + now_r) >> 1;
            T nxt_lazy_acc = next_lazy_acc(node, lazy_acc);
            long long res = internal_min_left(node == nullptr ? nullptr : node->right, mid, now_r, acc, acc_len, r, g, nxt_lazy_acc, args...);
            if(res != -1){
                return res;
            }
            return internal_min_left(node == nullptr ? nullptr : node->left, now_l, mid, acc, acc_len, r, g, nxt_lazy_acc, args...);
        }


    public:
        explicit DynamicLazySegtree(long long n = MAX_SIZE):_n(n){
            if(n < 0 || MAX_SIZE < n)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (constructor).");
            }
            root = new Node(_n);
        }

        ~DynamicLazySegtree(){
            delete_subtree(root);
        }

        DynamicLazySegtree(const DynamicLazySegtree&) = delete;
        DynamicLazySegtree& operator=(const DynamicLazySegtree&) = delete;

        void set(long long k, const S& x){
            if(k < 0 || _n <= k)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (set).");
            }
            internal_set(root, 0, _n, k, x);
        }

        S get(long long k)const{
            if(k < 0 || _n <= k)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (get).");
            }
            return internal_get(root, 0, _n, k, Monoid.id());
        }

        S prod(long long l, long long r)const{
            if(l < 0 || l > r || _n < r)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (prod).");
            }
            if(l == r){
                return Monoid.e();
            }
            return internal_prod(root, 0, _n, l, r, Monoid.id()).first;
        }

        S all_prod()const{
            return root->val;
        }

        void apply(long long k, const T& f){
            if(k < 0 || _n <= k)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (apply).");
            }
            internal_apply(root, 0, _n, k, f);
        }

        void apply(long long l, long long r, const T& f){
            if(l < 0 || l > r || _n < r)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (apply).");
            }
            if(l == r){
                return;
            }
            internal_apply(root, 0, _n, l, r, f);
        }

        long long max_right(long long l, auto g, auto&&... args)const{
            if(l < 0 || _n < l)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (max_right).");
            }
            if(!std::invoke(g, Monoid.e(), args...))[[unlikely]]{
                throw std::runtime_error("library assertion fault: f(e) must be true (max_right).");
            }
            if(l == _n) return _n;
            S acc = Monoid.e();
            long long acc_len = 0;
            return internal_max_right(root, 0, _n, acc, acc_len, l, g, Monoid.id(), args...);
        }

        long long min_left(long long r, auto g, auto&&... args)const{
            if(r < 0 || _n < r)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (min_left).");
            }
            if(!std::invoke(g, Monoid.e(), args...))[[unlikely]]{
                throw std::runtime_error("library assertion fault: f(e) must be true (min_left).");
            }
            if(r == 0) return 0;
            S acc = Monoid.e();
            long long acc_len = 0;
            return std::max(0LL, internal_min_left(root, 0, _n, acc, acc_len, r, g, Monoid.id(), args...));
        }
};
