#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_DYNAMIC_SEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_DYNAMIC_SEGTREE_HPP_INCLUDED
#include <vector>
#include <stdexcept>
#include <type_traits>
#include <functional>
#include <limits>
#include <algorithm>
#include "../types/monoid.hpp"


template<auto Monoid, long long MAX_SIZE>
struct DynamicSegtree{
    static_assert(0 < MAX_SIZE && MAX_SIZE < 0x3FFFFFFFFFFFFFFF);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;
    
    private:
        struct Node{
            S val = Monoid.e();
            Node* left = nullptr;
            Node* right = nullptr;
        };

        void delete_subtree(Node* node){
            if(node == nullptr) return;
            delete_subtree(node->left);
            delete_subtree(node->right);
            delete node;
        }

        Node* root;

        void internal_set(Node* node, long long l, long long r, long long idx, const S& x){
            if(r - l == 1){
                node->val = x;
                return;
            }
            long long mid = (l + r) >> 1;
            if(idx < mid){
                if(node->left == nullptr){
                    node->left = new Node();
                }
                internal_set(node->left, l, mid, idx, x);
            }else{
                if(node->right == nullptr){
                    node->right = new Node();
                }
                internal_set(node->right, mid, r, idx, x);
            }
            S left = Monoid.e();
            S right = Monoid.e();
            if(node->left != nullptr){
                left = node->left->val;
            }
            if(node->right != nullptr){
                right = node->right->val;
            }
            node->val = Monoid.op(left, right);
        }

        S internal_get(Node* node, long long l, long long r, long long idx)const{
            if(r - l == 1){
                return node->val;
            }
            long long mid = (l + r) >> 1;
            if(idx < mid){
                if(node->left == nullptr){
                    return Monoid.e();
                }else{
                    return internal_get(node->left, l, mid, idx);
                }
            }else{
                if(node->right == nullptr){
                    return Monoid.e();
                }else{
                    return internal_get(node->right, mid, r, idx);
                }
            }
        }

        S internal_prod(Node* node, long long now_l, long long now_r, long long q_l, long long q_r)const{
            if(now_r <= q_l || q_r <= now_l){
                return Monoid.e();
            }
            if(q_l <= now_l && now_r <=q_r){
                return node->val;
            }
            long long mid = (now_l + now_r) >> 1;
            S left = Monoid.e();
            S right = Monoid.e();
            if(node->left != nullptr){
                left = internal_prod(node->left, now_l, mid, q_l, q_r);
            }
            if(node->right != nullptr){
                right = internal_prod(node->right, mid, now_r, q_l, q_r);
            }
            return Monoid.op(left, right);
        }

        long long internal_max_right(Node* node, long long now_l, long long now_r, S& acc, long long l, auto f, auto&&... args)const{
            if(now_r <= l){
                return MAX_SIZE;
            }
            if(l <= now_l){
                S nxt = Monoid.op(acc, node->val);
                if(std::invoke(f, nxt, args...)){
                    acc = nxt;
                    return MAX_SIZE;
                }
                if(now_r - now_l == 1){
                    return now_l;
                }
            }
            long long mid = (now_l + now_r) >> 1;
            if(node->left != nullptr){
                long long res = internal_max_right(node->left, now_l, mid, acc, l, f, args...);
                if(res != MAX_SIZE){
                    return res;
                }
            }
            if(node->right != nullptr){
                return internal_max_right(node->right, mid, now_r, acc, l, f, args...);
            }else{
                return MAX_SIZE;
            }
        }

        long long internal_min_left(Node* node, long long now_l, long long now_r, S& acc, long long r, auto f, auto&&... args)const{
            if(r <= now_l){
                return -1;
            }
            if(now_r <= r){
                S nxt = Monoid.op(node->val, acc);
                if(std::invoke(f, nxt, args...)){
                    acc = nxt;
                    return -1;
                }
                if(now_r - now_l == 1){
                    return now_r;
                }
            }
            long long mid = (now_l + now_r) >> 1;
            if(node->right != nullptr){
                long long res = internal_min_left(node->right, mid, now_r, acc, r, f, args...);
                if(res != -1){
                    return res;
                }
            }
            if(node->left != nullptr){
                return internal_min_left(node->left, now_l, mid, acc, r, f, args...);
            }else{
                return -1;
            }
        }


    public:
        explicit DynamicSegtree(){
            root = new Node();
        }

        ~DynamicSegtree(){
            delete_subtree(root);
        }

        DynamicSegtree(const DynamicSegtree&) = delete;
        DynamicSegtree& operator=(const DynamicSegtree&) = delete;

        void set(long long k, const S& x){
            if(k < 0 || MAX_SIZE <= k)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (set).");
            }
            internal_set(root, 0, MAX_SIZE, k, x);
        }

        S get(long long k)const{
            if(k < 0 || MAX_SIZE <= k)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (get).");
            }
            return internal_get(root, 0, MAX_SIZE, k);
        }

        S prod(long long l, long long r)const{
            if(l < 0 || l > r || MAX_SIZE < r)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (prod).");
            }
            return internal_prod(root, 0, MAX_SIZE, l, r);
        }

        S all_prod()const{
            return root->val;
        }

        long long max_right(long long l, auto f, auto&&... args)const{
            if(l < 0 || MAX_SIZE < l)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (max_right).");
            }
            if(!std::invoke(f, Monoid.e(), args...))[[unlikely]]{
                throw std::runtime_error("library assertion fault: f(e) must be true (max_right).");
            }
            if(l == MAX_SIZE) return MAX_SIZE;
            S acc = Monoid.e();
            return internal_max_right(root, 0, MAX_SIZE, acc, l, f, args...);
        }

        long long min_left(long long r, auto f, auto&&... args)const{
            if(r < 0 || MAX_SIZE < r)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (min_left).");
            }
            if(!std::invoke(f, Monoid.e(), args...))[[unlikely]]{
                throw std::runtime_error("library assertion fault: f(e) must be true (min_left).");
            }
            if(r == 0) return 0;
            S acc = Monoid.e();
            return std::max(0LL, internal_min_left(root, 0, MAX_SIZE, acc, r, f, args...));
        }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_DYNAMIC_SEGTREE_HPP_INCLUDED
