#include <array>
#include <memory>
#include <immintrin.h>
#include <cstddef>
#include <algorithm>
#include <stdexcept>


template<long long MAX_SIZE>
struct Wide_Segtree{
    private:
        #if defined(__AVX512F__)
        static const int B = 3;
        #elif defined(__AVX2__)
        static const int B = 2;
        #else
        static const int B = 1;
        #endif

        static const int node_size = 1 << B;

        struct Node{
            std::array<long long, node_size> sum{};
            std::array<std::unique_ptr<Node>, node_size> nodes{};
        };

        std::unique_ptr<Node> root;

        inline long long child_index(long long l, long long r, long long idx){
            return (long long)(((__int128_t)(idx - l + 1) * node_size - 1) / (r - l));
        }

        inline long long child_l(long long l, long long r, long long child){
            return l + (long long)(((__int128_t)child * (r - l)) >> B);
        }

        inline long long child_r(long long l, long long r, long long child){
            return std::min<long long>(
                MAX_SIZE,
                l + (long long)(((__int128_t)(child + 1) * (r - l)) >> B)
            );
        }

        void internal_copy(Node* mine, const Node* other){
            mine->sum = other->sum;
            for(int i = 0; i < node_size; i++){
                if(other->nodes[i]){
                    mine->nodes[i] = std::make_unique<Node>();
                    internal_copy(mine->nodes[i].get(), other->nodes[i].get());
                }
            }
        }

        inline void add_parallel(std::array<long long, node_size>& a, int l, int r, long long x) {
        #if defined(__AVX512F__)
            static_assert(node_size == 8);

            const __m512i v  = _mm512_loadu_si512(a.data());
            const __m512i vx = _mm512_set1_epi64(x);

            const __mmask8 mask =
                static_cast<__mmask8>(((1u << (r - l)) - 1u) << l);

            const __m512i res = _mm512_mask_add_epi64(v, mask, v, vx);

            _mm512_storeu_si512(a.data(), res);

        #elif defined(__AVX2__)
            static_assert(node_size == 4);

            const __m256i v  =
                _mm256_loadu_si256(reinterpret_cast<const __m256i*>(a.data()));
            const __m256i vx = _mm256_set1_epi64x(x);

            // lanes: 0, 1, 2, 3
            const __m256i idx = _mm256_set_epi64x(3, 2, 1, 0);

            // mask = (l <= idx && idx < r)
            const __m256i ge_l =
                _mm256_cmpgt_epi64(idx, _mm256_set1_epi64x(static_cast<long long>(l) - 1));
            const __m256i lt_r =
                _mm256_cmpgt_epi64(_mm256_set1_epi64x(r), idx);

            const __m256i mask = _mm256_and_si256(ge_l, lt_r);
            const __m256i add  = _mm256_and_si256(vx, mask);

            const __m256i res = _mm256_add_epi64(v, add);

            _mm256_storeu_si256(reinterpret_cast<__m256i*>(a.data()), res);

        #else
            static_assert(node_size == 2);

            for (int i = l; i < r; ++i) {
                a[i] += x;
            }
        #endif
        }

        long long internal_set(Node* node, long long l, long long r, const long long& idx, const long long& x){
            if(r - l <= node_size){
                long long prev_val = node->sum[idx - l];
                if(idx - l > 0){
                    prev_val -= node->sum[idx - l - 1];
                }
                long long diff = x - prev_val;
                add_parallel(node->sum, idx - l, node_size, diff);
                return diff;
            }
            long long child = child_index(l, r, idx);
            if(!node->nodes[child]){
                node->nodes[child] = std::make_unique<Node>();
            }
            long long nl = child_l(l, r, child);
            long long nr = child_r(l, r, child);
            long long diff = internal_set(node->nodes[child].get(), nl, nr, idx, x);
            add_parallel(node->sum, child, node_size, diff);
            return diff;
        }

        long long internal_get(Node* node, long long l, long long r, const long long& idx){
            if(r - l <= node_size){
                long long val = node->sum[idx - l];
                if(idx - l > 0){
                    val -= node->sum[idx - l - 1];
                }
                return val;
            }
            long long child = child_index(l, r, idx);
            if(!node->nodes[child]){
                return 0LL;
            }
            long long nl = child_l(l, r, child);
            long long nr = child_r(l, r, child);
            long long val = internal_get(node->nodes[child].get(), nl, nr, idx);
            return val;
        }

        void internal_add(Node* node, long long l, long long r, const long long& idx, const long long& x){
            if(r - l <= node_size){
                add_parallel(node->sum, idx - l, node_size, x);
                return;
            }
            long long child = child_index(l, r, idx);
            if(!node->nodes[child]){
                node->nodes[child] = std::make_unique<Node>();
            }
            long long nl = child_l(l, r, child);
            long long nr = child_r(l, r, child);
            internal_add(node->nodes[child].get(), nl, nr, idx, x);
            add_parallel(node->sum, child, node_size, x);
            return;
        }

        long long internal_prod(Node* node, long long l, long long r, const long long& ql, const long long& qr){
            if(!node || r <= ql || qr <= l){
                return 0LL;
            }
            if(ql <= l && r <= qr){
                if(r - l <= node_size){
                    return node->sum[r - l - 1];
                }else{
                    return node->sum[node_size - 1];
                }
            }
            if(r - l <= node_size){
                long long res;
                if(qr <= r){
                    res = node->sum[qr - 1 - l];
                }else{
                    res = node->sum[r - 1 - l];
                }
                if(ql > l){
                    res -= node->sum[ql - l - 1];
                }
                return res;
            }
            long long child_left = node_size;
            long long child_right = node_size;
            if(l <= ql && ql < r){
                child_left = child_index(l, r, ql);
            }
            if(l < qr && qr <= r){
                child_right = child_index(l, r, qr - 1);
            }
            if(child_left < node_size && child_right < node_size){
                if(child_left == child_right){
                    if(node->nodes[child_left]){
                        long long nl = child_l(l, r, child_left);
                        long long nr = child_r(l, r, child_left);
                        return internal_prod(node->nodes[child_left].get(), nl, nr, ql, qr);
                    }else{
                        return 0LL;
                    }
                }else{
                    long long res = node->sum[child_right - 1] - node->sum[child_left];
                    if(node->nodes[child_left]){
                        long long nl = child_l(l, r, child_left);
                        long long nr = child_r(l, r, child_left);
                        res += internal_prod(node->nodes[child_left].get(), nl, nr, ql, qr);
                    }
                    if(node->nodes[child_right]){
                        long long nl = child_l(l, r, child_right);
                        long long nr = child_r(l, r, child_right);
                        res += internal_prod(node->nodes[child_right].get(), nl, nr, ql, qr);
                    }
                    return res;
                }
            }else if(child_left < node_size){
                long long res = node->sum[node_size - 1] - node->sum[child_left];
                if(node->nodes[child_left]){
                    long long nl = child_l(l, r, child_left);
                    long long nr = child_r(l, r, child_left);
                    res += internal_prod(node->nodes[child_left].get(), nl, nr, ql, qr);
                }
                return res;
            }else if(child_right < node_size){
                long long res = 0;
                if(child_right > 0){
                    res = node->sum[child_right - 1];
                }
                if(node->nodes[child_right]){
                    long long nl = child_l(l, r, child_right);
                    long long nr = child_r(l, r, child_right);
                    res += internal_prod(node->nodes[child_right].get(), nl, nr, ql, qr);
                }
                return res;
            }else{
                __builtin_unreachable();
            }
        }

    public:
        explicit Wide_Segtree(){
            root = std::make_unique<Node>();
        }

        Wide_Segtree(const Wide_Segtree& other){
            if(other.root){
                root = std::make_unique<Node>();
                internal_copy(root.get(), other.root.get());
            }
        }

        Wide_Segtree& operator=(const Wide_Segtree& other){
            if(this != &other){
                if(other.root){
                    auto new_root = std::make_unique<Node>();
                    internal_copy(new_root.get(), other.root.get());
                    root = std::move(new_root);
                }else{
                    root.reset();
                }
            }
            return *this;
        }

        void set(const long long& idx, const long long& x){
            if(idx < 0 || MAX_SIZE <= idx)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (set).");
            }
            internal_set(root.get(), 0LL, MAX_SIZE, idx, x);
        }

        long long get(const long long& idx){
            if(idx < 0 || MAX_SIZE <= idx)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (get).");
            }
            return internal_get(root.get(), 0LL, MAX_SIZE, idx);
        }

        void add(const long long& idx, const long long& x){
            if(idx < 0 || MAX_SIZE <= idx)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (add).");
            }
            internal_add(root.get(), 0LL, MAX_SIZE, idx, x);
        }

        long long prod(const long long& l, const long long& r){
            if(l < 0 || r < 0 || l > r || MAX_SIZE < r)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (prod).");
            }
            if(l == r){
                return 0LL;
            }
        return internal_prod(root.get(), 0LL, MAX_SIZE, l, r);
        }

        long long all_prod(){
            return root.get()->sum[node_size - 1];
        }
};