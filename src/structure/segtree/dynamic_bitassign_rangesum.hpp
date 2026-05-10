#ifndef DYNAMIC_BIT01_WST_NOFLIP_HPP
#define DYNAMIC_BIT01_WST_NOFLIP_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#pragma GCC target("avx2,popcnt")
#include <immintrin.h>
#define DYNAMIC_BIT01_WST_NOFLIP_USE_AVX2 1
#endif

#ifndef DYNAMIC_BIT01_WST_NOFLIP_USE_AVX2
#define DYNAMIC_BIT01_WST_NOFLIP_USE_AVX2 0
#endif

class DynamicBit01WSTNoFlip {
    using u64 = std::uint64_t;

    static constexpr int B = 16;

    static constexpr std::int8_t NO   = -1;
    static constexpr std::int8_t SET0 = 0;
    static constexpr std::int8_t SET1 = 1;

    struct Node {
        u64 sum = 0;
        u64 bits = 0;
        std::int8_t lazy = NO;

        std::array<int, B> child{};
        std::array<u64, B> child_sum{};

        Node() {
            child.fill(-1);
            child_sum.fill(0);
        }
    };

    std::size_t n_ = 0;
    std::size_t block_cnt_ = 0;
    std::size_t leaf_size_ = 1;

    std::vector<Node> node_;

    static u64 mask_range(int l, int r) {
        if (l >= r) return 0;

        u64 left = (l == 0 ? ~0ULL : (~0ULL << l));
        u64 right = (r == 64 ? ~0ULL : ((1ULL << r) - 1));

        return left & right;
    }

    u64 valid_mask(std::size_t block) const {
        if (block >= block_cnt_) return 0;

        std::size_t rem = n_ - block * 64;

        if (rem >= 64) return ~0ULL;
        return (1ULL << rem) - 1;
    }

    u64 valid_bits(std::size_t block_l, std::size_t len_blocks) const {
        if (block_l >= block_cnt_) return 0;

        std::size_t block_r = std::min(block_cnt_, block_l + len_blocks);
        u64 res = static_cast<u64>((block_r - block_l) * 64);

        if (block_r == block_cnt_ && (n_ & 63)) {
            res -= static_cast<u64>(64 - (n_ & 63));
        }

        return res;
    }

    u64 valid_bits_range(std::size_t block_l, std::size_t block_r) const {
        if (block_l >= block_r) return 0;
        return valid_bits(block_l, block_r - block_l);
    }

    static u64 reduce16(const std::array<u64, B>& a) {
#if DYNAMIC_BIT01_WST_NOFLIP_USE_AVX2
        __m256i x0 = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(a.data())
        );
        __m256i x1 = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(a.data() + 4)
        );
        __m256i x2 = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(a.data() + 8)
        );
        __m256i x3 = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(a.data() + 12)
        );

        __m256i s0 = _mm256_add_epi64(x0, x1);
        __m256i s1 = _mm256_add_epi64(x2, x3);
        __m256i s = _mm256_add_epi64(s0, s1);

        __m128i lo = _mm256_castsi256_si128(s);
        __m128i hi = _mm256_extracti128_si256(s, 1);
        __m128i t = _mm_add_epi64(lo, hi);

        __m128i h = _mm_unpackhi_epi64(t, t);
        t = _mm_add_epi64(t, h);

        return static_cast<u64>(_mm_cvtsi128_si64(t));
#else
        u64 s = 0;
        for (int i = 0; i < B; i++) {
            s += a[i];
        }
        return s;
#endif
    }

    int new_node() {
        node_.emplace_back();
        return static_cast<int>(node_.size()) - 1;
    }

    void clear_children(int v) {
        node_[v].child.fill(-1);
        node_[v].child_sum.fill(0);
    }

    void pull(int v) {
        node_[v].sum = reduce16(node_[v].child_sum);
    }

    void apply_assign(int v, std::size_t l, std::size_t len, bool x) {
        node_[v].sum = x ? valid_bits(l, len) : 0;

        if (len == 1) {
            node_[v].bits = x ? valid_mask(l) : 0;
            node_[v].lazy = NO;
        } else {
            clear_children(v);
            node_[v].bits = 0;
            node_[v].lazy = x ? SET1 : SET0;
        }
    }

    void break_lazy(int v, std::size_t l, std::size_t len) {
        std::int8_t tag = node_[v].lazy;

        if (tag == NO || len == 1) return;

        std::size_t child_len = len / B;

        node_[v].child.fill(-1);

        for (int i = 0; i < B; i++) {
            std::size_t cl = l + child_len * static_cast<std::size_t>(i);

            node_[v].child_sum[i] =
                (tag == SET1 ? valid_bits(cl, child_len) : 0);
        }

        node_[v].lazy = NO;
    }

    int ensure_child(int v, int k, std::size_t l, std::size_t len) {
        if (node_[v].child[k] != -1) {
            return node_[v].child[k];
        }

        int c = new_node();

        u64 full = valid_bits(l, len);
        u64 s = node_[v].child_sum[k];

        assert(s == 0 || s == full);

        if (s == full && full != 0) {
            apply_assign(c, l, len, true);
        }

        node_[v].child[k] = c;
        return c;
    }

    void set_child_assign(
        int v,
        int k,
        std::size_t l,
        std::size_t len,
        bool x
    ) {
        node_[v].child[k] = -1;
        node_[v].child_sum[k] = x ? valid_bits(l, len) : 0;
    }

    void normalize(int v, std::size_t l, std::size_t len) {
        if (len == 1) return;

        u64 full = valid_bits(l, len);

        if (node_[v].sum == 0) {
            apply_assign(v, l, len, false);
        } else if (node_[v].sum == full) {
            apply_assign(v, l, len, true);
        }
    }

    void assign_bits_dfs(
        int v,
        std::size_t l,
        std::size_t len,
        std::size_t block,
        u64 mask,
        bool x
    ) {
        if (len == 1) {
            mask &= valid_mask(l);

            if (x) {
                node_[v].bits |= mask;
            } else {
                node_[v].bits &= ~mask;
            }

            node_[v].sum =
                static_cast<u64>(__builtin_popcountll(node_[v].bits));
            node_[v].lazy = NO;

            return;
        }

        break_lazy(v, l, len);

        std::size_t child_len = len / B;
        int k = static_cast<int>((block - l) / child_len);
        std::size_t cl = l + child_len * static_cast<std::size_t>(k);

        int c = ensure_child(v, k, cl, child_len);

        assign_bits_dfs(c, cl, child_len, block, mask, x);

        node_[v].child_sum[k] = node_[c].sum;
        pull(v);
        normalize(v, l, len);
    }

    u64 sum_bits_dfs(
        int v,
        std::size_t l,
        std::size_t len,
        std::size_t block,
        u64 mask
    ) {
        if (len == 1) {
            mask &= valid_mask(l);
            return static_cast<u64>(
                __builtin_popcountll(node_[v].bits & mask)
            );
        }

        if (node_[v].lazy == SET0) {
            return 0;
        }

        if (node_[v].lazy == SET1) {
            return static_cast<u64>(
                __builtin_popcountll(valid_mask(block) & mask)
            );
        }

        std::size_t child_len = len / B;
        int k = static_cast<int>((block - l) / child_len);
        std::size_t cl = l + child_len * static_cast<std::size_t>(k);

        int c = node_[v].child[k];

        if (c != -1) {
            return sum_bits_dfs(c, cl, child_len, block, mask);
        }

        u64 full = valid_bits(cl, child_len);
        u64 s = node_[v].child_sum[k];

        assert(s == 0 || s == full);

        if (s == 0) return 0;

        return static_cast<u64>(
            __builtin_popcountll(valid_mask(block) & mask)
        );
    }

    void assign_blocks_dfs(
        int v,
        std::size_t l,
        std::size_t len,
        std::size_t ql,
        std::size_t qr,
        bool x
    ) {
        if (qr <= l || l + len <= ql) return;

        if (ql <= l && l + len <= qr) {
            apply_assign(v, l, len, x);
            return;
        }

        break_lazy(v, l, len);

        std::size_t child_len = len / B;

        std::size_t first = (ql <= l ? 0 : (ql - l) / child_len);
        std::size_t last = (
            qr >= l + len
                ? B
                : (qr - l + child_len - 1) / child_len
        );

        if (last > B) last = B;

        for (std::size_t i = first; i < last; i++) {
            int k = static_cast<int>(i);
            std::size_t cl = l + child_len * i;
            std::size_t cr = cl + child_len;

            if (ql <= cl && cr <= qr) {
                set_child_assign(v, k, cl, child_len, x);
            } else {
                int c = ensure_child(v, k, cl, child_len);

                assign_blocks_dfs(
                    c,
                    cl,
                    child_len,
                    ql,
                    qr,
                    x
                );

                node_[v].child_sum[k] = node_[c].sum;
            }
        }

        pull(v);
        normalize(v, l, len);
    }

    u64 sum_blocks_dfs(
        int v,
        std::size_t l,
        std::size_t len,
        std::size_t ql,
        std::size_t qr
    ) {
        if (qr <= l || l + len <= ql) return 0;

        if (ql <= l && l + len <= qr) {
            return node_[v].sum;
        }

        if (node_[v].lazy == SET0) {
            return 0;
        }

        if (node_[v].lazy == SET1) {
            std::size_t L = std::max(l, ql);
            std::size_t R = std::min(l + len, qr);
            return valid_bits_range(L, R);
        }

        std::size_t child_len = len / B;

        std::size_t first = (ql <= l ? 0 : (ql - l) / child_len);
        std::size_t last = (
            qr >= l + len
                ? B
                : (qr - l + child_len - 1) / child_len
        );

        if (last > B) last = B;

        u64 res = 0;

        for (std::size_t i = first; i < last; i++) {
            int k = static_cast<int>(i);
            std::size_t cl = l + child_len * i;
            std::size_t cr = cl + child_len;

            int c = node_[v].child[k];

            if (c != -1) {
                res += sum_blocks_dfs(c, cl, child_len, ql, qr);
            } else {
                u64 full = valid_bits(cl, child_len);
                u64 s = node_[v].child_sum[k];

                assert(s == 0 || s == full);

                if (s != 0) {
                    std::size_t L = std::max(cl, ql);
                    std::size_t R = std::min(cr, qr);
                    res += valid_bits_range(L, R);
                }
            }
        }

        return res;
    }

    void assign_bits(std::size_t block, u64 mask, bool x) {
        mask &= valid_mask(block);

        if (mask == 0) return;

        assign_bits_dfs(0, 0, leaf_size_, block, mask, x);
    }

    u64 sum_bits(std::size_t block, u64 mask) {
        mask &= valid_mask(block);

        if (mask == 0) return 0;

        return sum_bits_dfs(0, 0, leaf_size_, block, mask);
    }

    void assign_blocks(std::size_t l, std::size_t r, bool x) {
        if (l >= r) return;
        assign_blocks_dfs(0, 0, leaf_size_, l, r, x);
    }

    u64 sum_blocks(std::size_t l, std::size_t r) {
        if (l >= r) return 0;
        return sum_blocks_dfs(0, 0, leaf_size_, l, r);
    }

public:
    DynamicBit01WSTNoFlip() {
        init(0);
    }

    explicit DynamicBit01WSTNoFlip(std::size_t n) {
        init(n);
    }

    explicit DynamicBit01WSTNoFlip(const std::vector<int>& a) {
        init(a.size());

        for (std::size_t i = 0; i < a.size(); i++) {
            if (a[i]) {
                assign(i, i + 1, true);
            }
        }
    }

    void init(std::size_t n) {
        n_ = n;
        block_cnt_ = (n_ + 63) >> 6;

        leaf_size_ = 1;

        while (leaf_size_ < std::max<std::size_t>(1, block_cnt_)) {
            leaf_size_ *= B;
        }

        node_.clear();
        node_.emplace_back();
    }

    std::size_t size() const {
        return n_;
    }

    std::size_t node_count() const {
        return node_.size();
    }

    // a[l, r) <- x
    void assign(std::size_t l, std::size_t r, bool x) {
        assert(l <= r && r <= n_);

        if (l == r) return;

        std::size_t lb = l >> 6;
        std::size_t rb = (r - 1) >> 6;

        if (lb == rb) {
            assign_bits(
                lb,
                mask_range(
                    static_cast<int>(l & 63),
                    static_cast<int>(((r - 1) & 63) + 1)
                ),
                x
            );
            return;
        }

        std::size_t full_l = lb;

        if (l & 63) {
            assign_bits(
                lb,
                mask_range(static_cast<int>(l & 63), 64),
                x
            );

            full_l = lb + 1;
        }

        std::size_t full_r = rb + 1;

        if (r & 63) {
            assign_bits(
                rb,
                mask_range(0, static_cast<int>(r & 63)),
                x
            );

            full_r = rb;
        }

        assign_blocks(full_l, full_r, x);
    }

    // sum a[l, r)
    u64 sum(std::size_t l, std::size_t r) {
        assert(l <= r && r <= n_);

        if (l == r) return 0;

        std::size_t lb = l >> 6;
        std::size_t rb = (r - 1) >> 6;

        if (lb == rb) {
            return sum_bits(
                lb,
                mask_range(
                    static_cast<int>(l & 63),
                    static_cast<int>(((r - 1) & 63) + 1)
                )
            );
        }

        u64 res = 0;

        std::size_t full_l = lb;

        if (l & 63) {
            res += sum_bits(
                lb,
                mask_range(static_cast<int>(l & 63), 64)
            );

            full_l = lb + 1;
        }

        std::size_t full_r = rb + 1;

        if (r & 63) {
            res += sum_bits(
                rb,
                mask_range(0, static_cast<int>(r & 63))
            );

            full_r = rb;
        }

        res += sum_blocks(full_l, full_r);

        return res;
    }

    bool get(std::size_t i) {
        assert(i < n_);
        return sum(i, i + 1) != 0;
    }
};

#endif