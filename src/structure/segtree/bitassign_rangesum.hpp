#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#pragma GCC target("avx2,popcnt")
#include <immintrin.h>
#define BIT01_WST_USE_AVX2 1
#endif

#ifndef BIT01_WST_USE_AVX2
#define BIT01_WST_USE_AVX2 0
#endif

class Bit01WST {
    using u64 = std::uint64_t;
    using u32 = std::uint32_t;

    static constexpr int B = 16;

    static constexpr std::int8_t NO   = -1;
    static constexpr std::int8_t SET0 = 0;
    static constexpr std::int8_t SET1 = 1;

    std::size_t n_ = 0;
    std::size_t block_cnt_ = 0;
    std::size_t leaf_size_ = 1;
    std::size_t leaf_offset_ = 0;

    std::vector<u64> bits_;
    std::vector<u32> sum_;
    std::vector<std::int8_t> lazy_;

    static u64 mask_range(int l, int r) {
        if (l >= r) return 0;

        u64 left = (l == 0 ? ~0ULL : (~0ULL << l));
        u64 right = (r == 64 ? ~0ULL : ((1ULL << r) - 1));

        return left & right;
    }

    u64 valid_mask(std::size_t b) const {
        if (b >= block_cnt_) return 0;

        std::size_t rem = n_ - b * 64;

        if (rem >= 64) return ~0ULL;
        return (1ULL << rem) - 1;
    }

    u32 valid_bits(std::size_t block_l, std::size_t len_blocks) const {
        if (block_l >= block_cnt_) return 0;

        std::size_t block_r = std::min(block_cnt_, block_l + len_blocks);
        std::size_t res = (block_r - block_l) * 64;

        if (block_r == block_cnt_ && (n_ & 63)) {
            res -= 64 - (n_ & 63);
        }

        return static_cast<u32>(res);
    }

    void pull(std::size_t v) {
        std::size_t c = B * v + 1;

#if BIT01_WST_USE_AVX2
        __m256i a = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(&sum_[c])
        );
        __m256i b = _mm256_loadu_si256(
            reinterpret_cast<const __m256i*>(&sum_[c + 8])
        );

        __m256i s = _mm256_add_epi32(a, b);

        __m128i lo = _mm256_castsi256_si128(s);
        __m128i hi = _mm256_extracti128_si256(s, 1);
        __m128i t = _mm_add_epi32(lo, hi);

        t = _mm_add_epi32(
            t,
            _mm_shuffle_epi32(t, _MM_SHUFFLE(1, 0, 3, 2))
        );

        t = _mm_add_epi32(
            t,
            _mm_shuffle_epi32(t, _MM_SHUFFLE(2, 3, 0, 1))
        );

        sum_[v] = static_cast<u32>(_mm_cvtsi128_si32(t));
#else
        u32 s = 0;
        for (int i = 0; i < B; i++) {
            s += sum_[c + i];
        }
        sum_[v] = s;
#endif
    }

    void apply_assign_node(
        std::size_t v,
        std::size_t block_l,
        std::size_t len_blocks,
        bool x
    ) {
        sum_[v] = x ? valid_bits(block_l, len_blocks) : 0;

        if (len_blocks == 1) {
            if (block_l < block_cnt_) {
                bits_[block_l] = x ? valid_mask(block_l) : 0;
            }
            lazy_[v] = NO;
        } else {
            lazy_[v] = x ? SET1 : SET0;
        }
    }

    void push(std::size_t v, std::size_t block_l, std::size_t len_blocks) {
        std::int8_t tag = lazy_[v];

        if (tag == NO || len_blocks == 1) return;

        std::size_t child_len = len_blocks / B;
        std::size_t c = B * v + 1;

        for (int i = 0; i < B; i++) {
            apply_assign_node(
                c + i,
                block_l + child_len * i,
                child_len,
                tag == SET1
            );
        }

        lazy_[v] = NO;
    }

    void materialize_leaf(std::size_t block) {
        std::size_t v = 0;
        std::size_t l = 0;
        std::size_t len = leaf_size_;

        while (len > 1) {
            push(v, l, len);

            std::size_t child_len = len / B;
            std::size_t k = (block - l) / child_len;

            v = B * v + 1 + k;
            l += child_len * k;
            len = child_len;
        }
    }

    void pull_path(std::size_t block) {
        std::size_t v = leaf_offset_ + block;

        while (v > 0) {
            v = (v - 1) / B;
            lazy_[v] = NO;
            pull(v);
        }
    }

    void assign_bits(std::size_t block, u64 mask, bool x) {
        mask &= valid_mask(block);
        if (mask == 0) return;

        materialize_leaf(block);

        if (x) {
            bits_[block] |= mask;
        } else {
            bits_[block] &= ~mask;
        }

        sum_[leaf_offset_ + block] =
            static_cast<u32>(__builtin_popcountll(bits_[block]));

        pull_path(block);
    }

    u32 sum_bits(std::size_t block, u64 mask) {
        mask &= valid_mask(block);
        if (mask == 0) return 0;

        materialize_leaf(block);

        return static_cast<u32>(__builtin_popcountll(bits_[block] & mask));
    }

    void assign_blocks_dfs(
        std::size_t v,
        std::size_t l,
        std::size_t len,
        std::size_t ql,
        std::size_t qr,
        bool x
    ) {
        if (qr <= l || l + len <= ql) return;

        if (ql <= l && l + len <= qr) {
            apply_assign_node(v, l, len, x);
            return;
        }

        push(v, l, len);

        std::size_t child_len = len / B;

        std::size_t first = (ql <= l ? 0 : (ql - l) / child_len);
        std::size_t last = (
            qr >= l + len
                ? B
                : (qr - l + child_len - 1) / child_len
        );

        if (last > B) last = B;

        std::size_t c = B * v + 1;

        for (std::size_t i = first; i < last; i++) {
            assign_blocks_dfs(
                c + i,
                l + child_len * i,
                child_len,
                ql,
                qr,
                x
            );
        }

        lazy_[v] = NO;
        pull(v);
    }

    u32 sum_blocks_dfs(
        std::size_t v,
        std::size_t l,
        std::size_t len,
        std::size_t ql,
        std::size_t qr
    ) {
        if (qr <= l || l + len <= ql) return 0;

        if (ql <= l && l + len <= qr) {
            return sum_[v];
        }

        push(v, l, len);

        std::size_t child_len = len / B;

        std::size_t first = (ql <= l ? 0 : (ql - l) / child_len);
        std::size_t last = (
            qr >= l + len
                ? B
                : (qr - l + child_len - 1) / child_len
        );

        if (last > B) last = B;

        std::size_t c = B * v + 1;
        u32 res = 0;

        for (std::size_t i = first; i < last; i++) {
            res += sum_blocks_dfs(
                c + i,
                l + child_len * i,
                child_len,
                ql,
                qr
            );
        }

        return res;
    }

    void assign_blocks(std::size_t l, std::size_t r, bool x) {
        if (l >= r) return;
        assign_blocks_dfs(0, 0, leaf_size_, l, r, x);
    }

    u32 sum_blocks(std::size_t l, std::size_t r) {
        if (l >= r) return 0;
        return sum_blocks_dfs(0, 0, leaf_size_, l, r);
    }

public:
    Bit01WST() = default;

    explicit Bit01WST(std::size_t n) {
        init(n);
    }

    explicit Bit01WST(const std::vector<int>& a) {
        init(a.size());

        for (std::size_t i = 0; i < a.size(); i++) {
            if (a[i]) {
                bits_[i >> 6] |= 1ULL << (i & 63);
            }
        }

        build();
    }

    void init(std::size_t n) {
        n_ = n;
        block_cnt_ = (n_ + 63) >> 6;

        leaf_size_ = 1;

        while (leaf_size_ < std::max<std::size_t>(1, block_cnt_)) {
            leaf_size_ *= B;
        }

        leaf_offset_ = (leaf_size_ - 1) / (B - 1);

        std::size_t node_cnt = leaf_offset_ + leaf_size_;

        bits_.assign(block_cnt_, 0);
        sum_.assign(node_cnt, 0);
        lazy_.assign(node_cnt, NO);
    }

    void build() {
        std::fill(sum_.begin(), sum_.end(), 0);
        std::fill(lazy_.begin(), lazy_.end(), NO);

        for (std::size_t b = 0; b < block_cnt_; b++) {
            bits_[b] &= valid_mask(b);

            sum_[leaf_offset_ + b] =
                static_cast<u32>(__builtin_popcountll(bits_[b]));
        }

        for (std::size_t v = leaf_offset_; v-- > 0;) {
            pull(v);
        }
    }

    std::size_t size() const {
        return n_;
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
    long long sum(std::size_t l, std::size_t r) {
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

        long long res = 0;

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