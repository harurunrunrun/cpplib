#include <type_traits>
#include <array>
#include <memory>
#include <algorithm>
#include <optional>
#include <immintrin.h>
#include <bit>
#include <vector>
#include <stdexcept>
#include <limits>


#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
#define INT_SET_GNU_X86_TARGET 1
#define INT_SET_TARGET_AVX512F __attribute__((target("avx512f")))
#define INT_SET_TARGET_AVX2 __attribute__((target("avx2")))
#else
#define INT_SET_GNU_X86_TARGET 0
#define INT_SET_TARGET_AVX512F
#define INT_SET_TARGET_AVX2
#endif

template<class L, L MAX_SIZE>
struct Int_Set{
    static_assert(std::is_same_v<L, unsigned int> || std::is_same_v<L, unsigned long long>);
    static_assert(0< MAX_SIZE && MAX_SIZE <= (L)std::numeric_limits<int>::max());
    
    private:
        static const short node_size = 16;
        static const short shift_num = 4;
        static_assert(1 << shift_num == node_size);

        struct Node{
            std::array<std::unique_ptr<Node>, node_size> nodes{};
            unsigned short exist = 0;
            unsigned short full = 0;
            std::array<int, node_size> sum;
            std::array<int, node_size> lazy;
            bool is_lazy = false;
            Node() = default;
        };

        std::unique_ptr<Node> root;

        void internal_copy(Node* mine, const Node* other){
            mine->exist = other->exist;
            mine->full = other->full;
            mine->sum = other->sum;
            mine->lazy = other->lazy;
            mine->is_lazy = other->is_lazy;
            for(unsigned short i = 0; i < node_size; i++){
                if(other->nodes[i]){
                    mine->nodes[i] = std::make_unique<Node>();
                    internal_copy(mine->nodes[i].get(), other->nodes[i].get());
                }
            }
        }

        inline unsigned short full_mask(int n){
            return (unsigned short)(((unsigned int)1 << n) - 1); 
        }

        inline L child_index(L l, L r, L x){
            return (L)(((__uint128_t)(x - l + 1) * node_size - 1) / (r - l));
        }

        inline L child_l(L l, L r, L child){
            return l + (L)(((__uint128_t)child * (r - l)) >> shift_num);
        }

        inline L child_r(L l, L r, L child){
            return std::min<L>(
                MAX_SIZE,
                l + (L)(((__uint128_t)(child + 1) * (r - l)) >> shift_num)
            );
        }

        static inline void apply_diff_scalar(std::array<int, 16>& a, std::array<int, 16>& diff){
            int cur = 0;
            for(int i = 0; i < 16; i++){
                cur += diff[i];
                a[i] += cur;
                diff[i] = 0;
            }
        }

#if INT_SET_GNU_X86_TARGET || defined(__AVX512F__)
        template<int K>
        INT_SET_TARGET_AVX512F
        static inline __m512i shift_left_epi32_avx512(__m512i v) {
            static_assert(1 <= K && K < 16);

            alignas(64) static constexpr int idx_arr[16] = {
                0 >= K ? 0 : 0 - K,
                1 >= K ? 1 - K : 0,
                2 >= K ? 2 - K : 0,
                3 >= K ? 3 - K : 0,
                4 >= K ? 4 - K : 0,
                5 >= K ? 5 - K : 0,
                6 >= K ? 6 - K : 0,
                7 >= K ? 7 - K : 0,
                8 >= K ? 8 - K : 0,
                9 >= K ? 9 - K : 0,
                10 >= K ? 10 - K : 0,
                11 >= K ? 11 - K : 0,
                12 >= K ? 12 - K : 0,
                13 >= K ? 13 - K : 0,
                14 >= K ? 14 - K : 0,
                15 >= K ? 15 - K : 0
            };

            __m512i idx = _mm512_load_si512(reinterpret_cast<const __m512i*>(idx_arr));
            constexpr __mmask16 mask = static_cast<__mmask16>(0xFFFFu << K);
            return _mm512_maskz_permutexvar_epi32(mask, idx, v);
        }

        INT_SET_TARGET_AVX512F
        static inline __m512i prefix_sum16_epi32_avx512(__m512i v) {
            v = _mm512_add_epi32(v, shift_left_epi32_avx512<1>(v));
            v = _mm512_add_epi32(v, shift_left_epi32_avx512<2>(v));
            v = _mm512_add_epi32(v, shift_left_epi32_avx512<4>(v));
            v = _mm512_add_epi32(v, shift_left_epi32_avx512<8>(v));
            return v;
        }

        INT_SET_TARGET_AVX512F
        static inline void apply_diff_avx512(std::array<int, 16>& a, std::array<int, 16>& diff){
            __m512i d = _mm512_loadu_si512(diff.data());
            d = prefix_sum16_epi32_avx512(d);
            __m512i old = _mm512_loadu_si512(a.data());
            __m512i res = _mm512_add_epi32(old, d);
            _mm512_storeu_si512(a.data(), res);
            __m512i zero = _mm512_setzero_si512();
            _mm512_storeu_si512(diff.data(), zero);
        }
#endif

#if INT_SET_GNU_X86_TARGET || defined(__AVX2__)
        template<int K>
        INT_SET_TARGET_AVX2
        static inline __m256i shift_left_epi32_avx2(__m256i v) {
            static_assert(1 <= K && K < 8);

            alignas(32) static constexpr int idx_arr[8] = {
                0 >= K ? 0 : 0 - K,
                1 >= K ? 1 - K : 0,
                2 >= K ? 2 - K : 0,
                3 >= K ? 3 - K : 0,
                4 >= K ? 4 - K : 0,
                5 >= K ? 5 - K : 0,
                6 >= K ? 6 - K : 0,
                7 >= K ? 7 - K : 0
            };
            alignas(32) static constexpr int mask_arr[8] = {
                0 >= K ? -1 : 0,
                1 >= K ? -1 : 0,
                2 >= K ? -1 : 0,
                3 >= K ? -1 : 0,
                4 >= K ? -1 : 0,
                5 >= K ? -1 : 0,
                6 >= K ? -1 : 0,
                7 >= K ? -1 : 0
            };

            __m256i idx = _mm256_load_si256(reinterpret_cast<const __m256i*>(idx_arr));
            __m256i mask = _mm256_load_si256(reinterpret_cast<const __m256i*>(mask_arr));
            return _mm256_and_si256(_mm256_permutevar8x32_epi32(v, idx), mask);
        }

        INT_SET_TARGET_AVX2
        static inline __m256i prefix_sum8_epi32_avx2(__m256i v) {
            v = _mm256_add_epi32(v, shift_left_epi32_avx2<1>(v));
            v = _mm256_add_epi32(v, shift_left_epi32_avx2<2>(v));
            v = _mm256_add_epi32(v, shift_left_epi32_avx2<4>(v));
            return v;
        }

        INT_SET_TARGET_AVX2
        static inline void apply_diff_avx2(std::array<int, 16>& a, std::array<int, 16>& diff){
            __m256i d0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(diff.data()));
            __m256i d1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(diff.data() + 8));

            d0 = prefix_sum8_epi32_avx2(d0);
            d1 = prefix_sum8_epi32_avx2(d1);

            alignas(32) int tmp[8];
            _mm256_store_si256(reinterpret_cast<__m256i*>(tmp), d0);
            d1 = _mm256_add_epi32(d1, _mm256_set1_epi32(tmp[7]));

            __m256i a0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(a.data()));
            __m256i a1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(a.data() + 8));
            a0 = _mm256_add_epi32(a0, d0);
            a1 = _mm256_add_epi32(a1, d1);
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(a.data()), a0);
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(a.data() + 8), a1);

            __m256i zero = _mm256_setzero_si256();
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(diff.data()), zero);
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(diff.data() + 8), zero);
        }
#endif

        using apply_diff_func = void (*)(std::array<int, 16>&, std::array<int, 16>&);

        static inline apply_diff_func select_apply_diff(){
#if INT_SET_GNU_X86_TARGET
            __builtin_cpu_init();
            if(__builtin_cpu_supports("avx512f")){
                return apply_diff_avx512;
            }
            if(__builtin_cpu_supports("avx2")){
                return apply_diff_avx2;
            }
            return apply_diff_scalar;
#else
#if defined(__AVX512F__)
            return apply_diff_avx512;
#elif defined(__AVX2__)
            return apply_diff_avx2;
#else
            return apply_diff_scalar;
#endif
#endif
        }

        static inline void apply_diff(std::array<int, 16>& a, std::array<int, 16>& diff){
            static const apply_diff_func func = select_apply_diff();
            func(a, diff);
        }

        bool internal_insert(Node* node, L l, L r, const L& x){
            if(r - l <= node_size){
                bool res=!((node->exist >> (x - l)) & 1);
                if(res){
                    node->is_lazy = true;
                    node->lazy[x - l]++;
                }
                node->exist |= (1u << (x - l));
                node->full |= (1u << (x - l));
                return res;
            }
            L child = child_index(l, r, x);
            if((node->full >> child) & 1){
                return false;
            }
            if(!node->nodes[child]){
                node->nodes[child] = std::make_unique<Node>();
            }
            L nl = child_l(l, r, child);
            L nr = child_r(l, r, child);
            bool flag = internal_insert(node->nodes[child].get(), nl, nr, x);
            node->exist |= 1 << child;
            if(node->nodes[child]->full == full_mask(std::min<L>((L)node_size, nr - nl))){
                node->full |= 1u << child;
            }
            if(flag){
                node->is_lazy = true;
                node->lazy[child]++;
            }
            return flag;
        }

        bool internal_erase(Node* node, L l, L r, const L& x){
            if(r - l <= node_size){
                bool res = (node->exist >> (x - l)) & 1;
                if(res){
                    node->is_lazy = true;
                    node->lazy[x - l]--;
                }
                node->exist &= ~(1u << (x - l));
                node->full &= ~(1u << (x - l));
                return res;
            }
            L child = child_index(l, r, x);
            if(!node->nodes[child]){
                return false;
            }
            if(!((node->exist >> child) & 1)){
                return false;
            }
            L nl = child_l(l, r, child);
            L nr = child_r(l, r, child);
            bool flag = internal_erase(node->nodes[child].get(), nl, nr, x);
            if(node->nodes[child]->exist == 0){
                node->exist &= ~(1u << child);
            }
            node->full &= ~(1u << child);
            if(flag){
                node->is_lazy = true;
                node->lazy[child]--;
            }
            return flag;
        }

        bool internal_contain(Node* node, L l, L r, const L& x){
            if(r - l <= node_size){
                return (node->exist >> (x - l)) & 1;
            }
            L child = child_index(l, r, x);
            if(!node->nodes[child]){
                return false;
            }
            if(!((node->exist >> child) & 1)){
                return false;
            }
            if((node->full >> child) & 1){
                return true;
            }
            L nl = child_l(l, r, child);
            L nr = child_r(l, r, child);
            return internal_contain(node->nodes[child].get(), nl, nr, x);
        }

        L internal_range_sum(Node* node, L l, L r, const L& ql, const L& qr){
            if(!node || r <= ql || qr <= l){
                return (L)0;
            }
            if(node->is_lazy){
                apply_diff(node->sum, node->lazy);
                node->is_lazy = false;
            }
            if(ql <= l && r <= qr){
                if(r - l <= node_size){
                    return node->sum[r - l - 1];
                }else{
                    return node->sum[node_size - 1];
                }
            }
            if(r - l <= node_size){
                L res;
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
            L child_left = node_size;
            L child_right = node_size;
            if(l <= ql && ql < r){
                child_left = child_index(l, r, ql);
            }
            if(l < qr && qr <= r){
                child_right = child_index(l, r, qr - 1);
            }
            if(child_left < node_size && child_right < node_size){
                if(child_left == child_right){
                    if(node->nodes[child_left]){
                        L nl = child_l(l, r, child_left);
                        L nr = child_r(l, r, child_left);
                        return internal_range_sum(node->nodes[child_left].get(), nl, nr, ql, qr);
                    }else{
                        return (L)0;
                    }
                }else{
                    L res = node->sum[child_right - 1] - node->sum[child_left];
                    if(node->nodes[child_left]){
                        L nl = child_l(l, r, child_left);
                        L nr = child_r(l, r, child_left);
                        res += internal_range_sum(node->nodes[child_left].get(), nl, nr, ql, qr);
                    }
                    if(node->nodes[child_right]){
                        L nl = child_l(l, r, child_right);
                        L nr = child_r(l, r, child_right);
                        res += internal_range_sum(node->nodes[child_right].get(), nl, nr, ql, qr);
                    }
                    return res;
                }
            }else if(child_left < node_size){
                L res = node->sum[node_size - 1] - node->sum[child_left];
                if(node->nodes[child_left]){
                    L nl = child_l(l, r, child_left);
                    L nr = child_r(l, r, child_left);
                    res += internal_range_sum(node->nodes[child_left].get(), nl, nr, ql, qr);
                }
                return res;
            }else if(child_right < node_size){
                L res = 0;
                if(child_right > 0){
                    res = node->sum[child_right - 1];
                }
                if(node->nodes[child_right]){
                    L nl = child_l(l, r, child_right);
                    L nr = child_r(l, r, child_right);
                    res += internal_range_sum(node->nodes[child_right].get(), nl, nr, ql, qr);
                }
                return res;
            }else{
                __builtin_unreachable();
            }
        }

        std::optional<L> internal_least(Node* node, L l, L r, const L& x){
            if(node->exist == 0){
                return std::nullopt;
            }
            if(r - l <= node_size){
                if(x <= l){
                    return l + std::countr_zero(node->exist);
                }else{
                    unsigned short tmp = node->exist & (~0u << (x - l));
                    if(tmp == 0){
                        return std::nullopt;
                    }else{
                        return l + std::countr_zero(tmp);
                    }
                }
            }
            if(x <= l){
                L child = std::countr_zero(node->exist);
                L nl = child_l(l, r, child);
                L nr = child_r(l, r, child);
                return internal_least(node->nodes[child].get(), nl, nr, x);
            }else{
                L child_x = child_index(l, r, x);
                if(!node->nodes[child_x] || node->nodes[child_x]->exist == 0){
                    unsigned short tmp = node->exist & (~0u << child_x);
                    if(tmp == 0){
                        return std::nullopt;
                    }
                    L child = std::countr_zero(tmp);
                    L nl = child_l(l, r, child);
                    L nr = child_r(l, r, child);
                    return internal_least(node->nodes[child].get(), nl, nr, x);
                }else{
                    L nl_x = child_l(l, r, child_x);
                    L nr_x = child_r(l, r, child_x);
                    auto res = internal_least(node->nodes[child_x].get(), nl_x, nr_x, x);
                    if(res.has_value()){
                        return *res;
                    }else{
                        unsigned short tmp = node->exist & (~0u << (child_x + 1));
                        if(tmp == 0){
                            return std::nullopt;
                        }else{
                            L child = std::countr_zero(tmp);
                            L nl = child_l(l, r, child);
                            L nr = child_r(l, r, child);
                            return internal_least(node->nodes[child].get(), nl, nr, x);
                        }
                    }
                }
            }
        }

        std::optional<L> internal_most(Node* node, L l, L r, const L& x){
            if(node->exist == 0){
                return std::nullopt;
            }
            if(r - l <= node_size){
                if(r - 1 <= x){
                    return l + std::bit_width(node->exist) - 1;
                }else{
                    unsigned short mask = node->exist & ((1u << (x - l + 1)) - 1);
                    if(mask == 0){
                        return std::nullopt;
                    }else{
                        return l + std::bit_width(mask) - 1;
                    }
                }
            }
            if(r - 1 <= x){
                L child = std::bit_width(node->exist) - 1;
                L nl = child_l(l, r, child);
                L nr = child_r(l, r, child);
                return internal_most(node->nodes[child].get(), nl, nr, x);
            }else{
                L child_x = child_index(l, r, x);
                if(!node->nodes[child_x] || node->nodes[child_x]->exist == 0){
                    unsigned short mask = node->exist & ((1u << child_x) - 1);
                    if(mask == 0){
                        return std::nullopt;
                    }
                    L child = std::bit_width(mask) - 1;
                    L nl = child_l(l, r, child);
                    L nr = child_r(l, r, child);
                    return internal_most(node->nodes[child].get(), nl, nr, x);
                }else{
                    L nl_x = child_l(l, r, child_x);
                    L nr_x = child_r(l, r, child_x);
                    auto res = internal_most(node->nodes[child_x].get(), nl_x, nr_x, x);
                    if(res.has_value()){
                        return *res;
                    }else{
                        unsigned short mask = node->exist & ((1u << child_x) - 1);
                        if(mask == 0){
                            return std::nullopt;
                        }else{
                            L child = std::bit_width(mask) - 1;
                            L nl = child_l(l, r, child);
                            L nr = child_r(l, r, child);
                            return internal_most(node->nodes[child].get(), nl, nr, x);
                        }
                    }
                }
            }
        }

        void internal_list(Node* node, L l, L r, std::vector<L>& res){
            if(node->exist == 0){
                return;
            }
            if(r - l <= node_size){
                unsigned short bit = node->exist;
                while(bit > 0){
                    unsigned short pos = std::countr_zero(bit);
                    res.push_back(l + pos);
                    bit &= bit - 1;
                }
            }else{
                unsigned short bit = node->exist;
                while(bit > 0){
                    unsigned short pos = std::countr_zero(bit);
                    L nl = child_l(l, r, pos);
                    L nr = child_r(l, r, pos);
                    internal_list(node->nodes[pos].get(), nl, nr, res);
                    bit &= bit - 1;
                }
            }
        }

        inline L child_count(Node* node, int child){
            L res = (L)node->sum[child];
            if(child > 0){
                res -= (L)node->sum[child - 1];
            }
            return res;
        }

        // x <= y 繧呈ｺ�縺溘☆ y 縺ｮ縺��■縲∵��鬆��〒 k 逡ｪ逶ｮ
        std::optional<L> internal_kth_ge(Node* node, L l, L r, const L& x, L k){
            if(!node || node->exist == 0 || r <= x){
                return std::nullopt;
            }

            if(node->is_lazy){
                apply_diff(node->sum, node->lazy);
                node->is_lazy = false;
            }

            if(r - l <= node_size){
                unsigned int bit = node->exist;

                if(l < x){
                    bit &= (~0u << (int)(x - l));
                }

                if(k >= (L)std::popcount(bit)){
                    return std::nullopt;
                }

                while(k > 0){
                    bit &= bit - 1;
                    --k;
                }

                return l + (L)std::countr_zero(bit);
            }

            if(x <= l){
                L total = (L)node->sum[node_size - 1];
                if(k >= total){
                    return std::nullopt;
                }

                for(int child = 0; child < node_size; child++){
                    L cnt = child_count(node, child);
                    if(k < cnt){
                        L nl = child_l(l, r, child);
                        L nr = child_r(l, r, child);
                        return internal_kth_ge(node->nodes[child].get(), nl, nr, nl, k);
                    }
                    k -= cnt;
                }

                return std::nullopt;
            }

            L child_x = child_index(l, r, x);

            if(node->nodes[child_x]){
                L nl = child_l(l, r, child_x);
                L nr = child_r(l, r, child_x);

                L cnt = internal_range_sum(node->nodes[child_x].get(), nl, nr, x, nr);

                if(k < cnt){
                    return internal_kth_ge(node->nodes[child_x].get(), nl, nr, x, k);
                }

                k -= cnt;
            }

            for(int child = (int)child_x + 1; child < node_size; child++){
                L cnt = child_count(node, child);
                if(k < cnt){
                    L nl = child_l(l, r, child);
                    L nr = child_r(l, r, child);
                    return internal_kth_ge(node->nodes[child].get(), nl, nr, nl, k);
                }
                k -= cnt;
            }

            return std::nullopt;
        }

        // x >= y 繧呈ｺ�縺溘☆ y 縺ｮ縺��■縲��剄鬆��〒 k 逡ｪ逶ｮ
        std::optional<L> internal_kth_le(Node* node, L l, L r, const L& x, L k){
            if(!node || node->exist == 0 || x < l){
                return std::nullopt;
            }

            if(node->is_lazy){
                apply_diff(node->sum, node->lazy);
                node->is_lazy = false;
            }

            if(r - l <= node_size){
                unsigned int bit = node->exist;

                if(x < r - 1){
                    bit &= ((1u << ((int)(x - l) + 1)) - 1);
                }

                if(k >= (L)std::popcount(bit)){
                    return std::nullopt;
                }

                while(k > 0){
                    unsigned int pos = std::bit_width(bit) - 1;
                    bit &= ~(1u << pos);
                    --k;
                }

                return l + (L)(std::bit_width(bit) - 1);
            }

            if(r - 1 <= x){
                L total = (L)node->sum[node_size - 1];
                if(k >= total){
                    return std::nullopt;
                }

                for(int child = node_size - 1; child >= 0; child--){
                    L cnt = child_count(node, child);
                    if(k < cnt){
                        L nl = child_l(l, r, child);
                        L nr = child_r(l, r, child);
                        return internal_kth_le(node->nodes[child].get(), nl, nr, nr - 1, k);
                    }
                    k -= cnt;
                }

                return std::nullopt;
            }

            L child_x = child_index(l, r, x);

            if(node->nodes[child_x]){
                L nl = child_l(l, r, child_x);
                L nr = child_r(l, r, child_x);

                L cnt = internal_range_sum(node->nodes[child_x].get(), nl, nr, nl, x + 1);

                if(k < cnt){
                    return internal_kth_le(node->nodes[child_x].get(), nl, nr, x, k);
                }

                k -= cnt;
            }

            for(int child = (int)child_x - 1; child >= 0; child--){
                L cnt = child_count(node, child);
                if(k < cnt){
                    L nl = child_l(l, r, child);
                    L nr = child_r(l, r, child);
                    return internal_kth_le(node->nodes[child].get(), nl, nr, nr - 1, k);
                }
                k -= cnt;
            }

            return std::nullopt;
        }
    public:
        explicit Int_Set(){
            root = std::make_unique<Node>();
        }

        Int_Set(const Int_Set& other){
            if(other.root){
                root = std::make_unique<Node>();
                internal_copy(root.get(), other.root.get());
            }
        }

        Int_Set& operator=(const Int_Set& other){
            if(this!= &other){
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

        Int_Set(Int_Set&&) noexcept = default;
        Int_Set& operator=(Int_Set&&) noexcept = default;

        void insert(const L& x){
            if(x < 0 || MAX_SIZE <= x)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (insert).");
            }
            internal_insert(root.get(), (L)0, MAX_SIZE, x);
        }

        void insert(const std::make_signed_t<L>& x){
            if(x < 0 || x >= (std::make_signed_t<L>)MAX_SIZE)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (insert).");
            }
            insert(static_cast<L>(x));
        }

        void erase(const L& x){
            if(x < 0 || MAX_SIZE <= x)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (erase).");
            }
            internal_erase(root.get(), (L)0, MAX_SIZE, x);
        }

        void erase(const std::make_signed_t<L>& x){
            if(x < 0 || x >= (std::make_signed_t<L>)MAX_SIZE)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (erase).");
            }
            erase(static_cast<L>(x));
        }

        bool contain(const L& x){
            if(x < 0 || MAX_SIZE <= x)[[unlikely]]{
                return false;
            }
            return internal_contain(root.get(), (L)0, MAX_SIZE, x);
        }

        bool contain(const std::make_signed_t<L>& x){
            return contain(static_cast<L>(x));
        }

        L range_sum(const L& l, const L& r){
            if(l >= r){
                return (L)0;
            }
            return internal_range_sum(root.get(), (L)0, MAX_SIZE, l, r);
        }

        L range_sum(const std::make_signed_t<L>& l, const std::make_signed_t<L>& r){
            if(l >= r || r <= 0 || l >= (std::make_signed_t<L>)MAX_SIZE){
                return (L)0;
            }
            L ll = l <= 0 ? (L)0 : (L)l;
            L rr = r >= (std::make_signed_t<L>)MAX_SIZE ? MAX_SIZE : (L)r;
            return range_sum(ll, rr);
        }

        // x < y を満たす最小の y
        std::optional<L> least(const L& x){
            if(x >= MAX_SIZE){
                return std::nullopt;
            }
            return internal_least(root.get(), (L)0, MAX_SIZE, x);
        }

        std::optional<L> least(const std::make_signed_t<L>& x){
            if(x <= 0){
                return least((L)0);
            }
            if((L)x >= MAX_SIZE){
                return std::nullopt;
            }
            return least(static_cast<L>(x));
        }

        // x < y を満たす最小の y
        std::optional<L> more(const L& x){
            if(x >= MAX_SIZE - 1){
                return std::nullopt;
            }
            return least(x + 1);
        }

        std::optional<L> more(const std::make_signed_t<L>& x){
            if(x < 0){
                return least((L)0);
            }
            if((L)x >= MAX_SIZE){
                return std::nullopt;
            }
            return more(static_cast<L>(x));
        }

        // x >= y を満たす最大の y
        std::optional<L> most(const L& x){
            return internal_most(root.get(), (L)0, MAX_SIZE, x);
        }

        std::optional<L> most(const std::make_signed_t<L>& x){
            if(x < 0){
                return std::nullopt;
            }
            if((L)x >= MAX_SIZE){
                return most(MAX_SIZE - 1);
            }
            return most(static_cast<L>(x));
        }

        // x > y を満たす最大の y
        std::optional<L> less(const L& x){
            if(x == 0){
                return std::nullopt;
            }
            return most(x - 1);
        }

        std::optional<L> less(const std::make_signed_t<L>& x){
            if(x <= 0){
                return std::nullopt;
            }
            if((L)x >= MAX_SIZE){
                return less(MAX_SIZE);
            }
            return less(static_cast<L>(x));
        }

        bool empty(){
            return root->exist == 0;
        }

        std::optional<L> max(){
            return most(MAX_SIZE - 1);
        }

        std::optional<L> min(){
            return least(0);
        }

        void clear(){
            root = std::make_unique<Node>();
        }

        std::vector<L> list(){
            const L res_size = range_sum((L)0, MAX_SIZE);
            std::vector<L> res;
            res.reserve(res_size);
            internal_list(root.get(), (L)0, MAX_SIZE, res);
            return res;
        }

        // x <= y を満たす y のうち、昇順で k 番目
        std::optional<L> kth_ge(const L& x, const L& k){
            if(x >= MAX_SIZE){
                return std::nullopt;
            }
            return internal_kth_ge(root.get(), (L)0, MAX_SIZE, x, k);
        }

        std::optional<L> kth_ge(const std::make_signed_t<L>& x, const L& k){
            if(x <= 0){
                return kth_ge((L)0, k);
            }
            if((L)x >= MAX_SIZE){
                return std::nullopt;
            }
            return kth_ge((L)x, k);
        }

        std::optional<L> kth_ge(const L& x, const std::make_signed_t<L>& k){
            if(k < 0){
                return std::nullopt;
            }
            return kth_ge(x, (L)k);
        }

        std::optional<L> kth_ge(const std::make_signed_t<L>& x, const std::make_signed_t<L>& k){
            if(k < 0){
                return std::nullopt;
            }
            return kth_ge(x, (L)k);
        }

        // x < y を満たす y のうち、昇順で k 番目
        std::optional<L> kth_gt(const L& x, const L& k){
            if(x >= MAX_SIZE - 1){
                return std::nullopt;
            }
            return kth_ge(x + 1, k);
        }

        std::optional<L> kth_gt(const std::make_signed_t<L>& x, const L& k){
            if(x < 0){
                return kth_ge((L)0, k);
            }
            if((L)x >= MAX_SIZE - 1){
                return std::nullopt;
            }
            return kth_gt((L)x, k);
        }

        std::optional<L> kth_gt(const L& x, const std::make_signed_t<L>& k){
            if(k < 0){
                return std::nullopt;
            }
            return kth_gt(x, (L)k);
        }

        std::optional<L> kth_gt(const std::make_signed_t<L>& x, const std::make_signed_t<L>& k){
            if(k < 0){
                return std::nullopt;
            }
            return kth_gt(x, (L)k);
        }

        // x >= y を満たす y のうち、降順で k 番目
        std::optional<L> kth_le(const L& x, const L& k){
            if(x >= MAX_SIZE){
                return internal_kth_le(root.get(), (L)0, MAX_SIZE, MAX_SIZE - 1, k);
            }
            return internal_kth_le(root.get(), (L)0, MAX_SIZE, x, k);
        }

        std::optional<L> kth_le(const std::make_signed_t<L>& x, const L& k){
            if(x < 0){
                return std::nullopt;
            }
            if((L)x >= MAX_SIZE){
                return kth_le(MAX_SIZE - 1, k);
            }
            return kth_le((L)x, k);
        }

        std::optional<L> kth_le(const L& x, const std::make_signed_t<L>& k){
            if(k < 0){
                return std::nullopt;
            }
            return kth_le(x, (L)k);
        }

        std::optional<L> kth_le(const std::make_signed_t<L>& x, const std::make_signed_t<L>& k){
            if(k < 0){
                return std::nullopt;
            }
            return kth_le(x, (L)k);
        }

        // x > y を満たす y のうち、降順で k 番目
        std::optional<L> kth_lt(const L& x, const L& k){
            if(x == 0){
                return std::nullopt;
            }
            return kth_le(x - 1, k);
        }

        std::optional<L> kth_lt(const std::make_signed_t<L>& x, const L& k){
            if(x <= 0){
                return std::nullopt;
            }
            if((L)x >= MAX_SIZE){
                return kth_le(MAX_SIZE - 1, k);
            }
            return kth_lt((L)x, k);
        }

        std::optional<L> kth_lt(const L& x, const std::make_signed_t<L>& k){
            if(k < 0){
                return std::nullopt;
            }
            return kth_lt(x, (L)k);
        }

        std::optional<L> kth_lt(const std::make_signed_t<L>& x, const std::make_signed_t<L>& k){
            if(k < 0){
                return std::nullopt;
            }
            return kth_lt(x, (L)k);
        }
};

#undef INT_SET_GNU_X86_TARGET
#undef INT_SET_TARGET_AVX512F
#undef INT_SET_TARGET_AVX2
