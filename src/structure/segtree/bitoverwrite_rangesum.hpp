#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <tuple>
#include <utility>
#include <vector>

#if defined(__GNUC__) && !defined(__clang__) && \
    (defined(__x86_64__) || defined(__i386__)) && !defined(WST_NO_AVX2)
#pragma GCC target("avx2,popcnt,bmi,bmi2")
#define WST_HAS_AVX2 1
#include <immintrin.h>
#endif

template<int B = 16>
struct WideSetSum {
    using u64 = std::uint64_t;

    static_assert(B >= 2);
#if defined(WST_HAS_AVX2)
    static_assert(B % 4 == 0, "B should be a multiple of 4 for AVX2");
#endif

    std::size_t n = 0;          // 元の配列長
    std::size_t w = 0;          // uint64_t ブロック数

    std::vector<u64> bit;       // 葉: 64個ずつ bit で持つ
    std::vector<std::vector<u64>> seg;       // WST の各階層の和
    std::vector<std::vector<std::uint8_t>> full;  // そのノード全体が 1 なら true
    std::vector<std::size_t> span;           // 各 level の 1 ノードが覆う uint64_t 個数

    WideSetSum() = default;

    explicit WideSetSum(std::size_t n_) {
        init(n_);
    }

    void init(std::size_t n_) {
        n = n_;
        w = (n + 63) >> 6;

        bit.assign(w, 0);
        seg.clear();
        full.clear();
        span.clear();

        if (w == 0) return;

        std::size_t sz = w;
        std::size_t sp = 1;

        while (true) {
            seg.emplace_back(sz, 0);
            full.emplace_back(sz, 0);
            span.push_back(sp);

            if (sz == 1) break;

            sz = (sz + B - 1) / B;

            if (sp <= std::numeric_limits<std::size_t>::max() / B) {
                sp *= B;
            } else {
                sp = std::numeric_limits<std::size_t>::max();
            }
        }
    }

    // a[l, r) = 1
    void set(std::size_t l, std::size_t r) {
        assert(l <= r && r <= n);
        if (l == r || w == 0) return;
        set_rec((int)seg.size() - 1, 0, l, r);
    }

    // sum a[l, r)
    u64 sum(std::size_t l, std::size_t r) const {
        assert(l <= r && r <= n);
        if (l == r || w == 0) return 0;
        return sum_rec((int)seg.size() - 1, 0, l, r);
    }

    u64 all_sum() const {
        return w == 0 ? 0 : seg.back()[0];
    }

private:
    static u64 low_mask(unsigned k) {
        return k >= 64 ? ~0ULL : ((1ULL << k) - 1);
    }

    static u64 mask_range(unsigned l, unsigned r) {
        // 1 word 内の bit 区間 [l, r)
        return low_mask(r) & ~low_mask(l);
    }

    static unsigned popcnt(u64 x) {
        return (unsigned)__builtin_popcountll((unsigned long long)x);
    }

    std::pair<std::size_t, std::size_t> node_range(int lv, std::size_t idx) const {
        const std::size_t lw = idx * span[lv];
        const std::size_t rw = std::min(w, lw + span[lv]);

        const std::size_t lb = std::min(n, lw << 6);
        const std::size_t rb = std::min(n, rw << 6);

        return {lb, rb};
    }

    std::size_t node_len(int lv, std::size_t idx) const {
        auto [l, r] = node_range(lv, idx);
        return r - l;
    }

    void set_node_full(int lv, std::size_t idx) {
        const std::size_t len = node_len(lv, idx);

        seg[lv][idx] = (u64)len;
        full[lv][idx] = 1;

        if (lv == 0) {
            bit[idx] = low_mask((unsigned)len);
        }
    }

    u64 sum_slice(const std::vector<u64>& a, std::size_t l, std::size_t r) const {
        u64 res = 0;
        std::size_t i = l;

#if defined(WST_HAS_AVX2)
        __m256i acc = _mm256_setzero_si256();

        for (; i + 4 <= r; i += 4) {
            __m256i v = _mm256_loadu_si256(
                (const __m256i*)(a.data() + i)
            );
            acc = _mm256_add_epi64(acc, v);
        }

        alignas(32) u64 tmp[4];
        _mm256_store_si256((__m256i*)tmp, acc);

        res += tmp[0] + tmp[1] + tmp[2] + tmp[3];
#endif

        for (; i < r; ++i) {
            res += a[i];
        }

        return res;
    }

    void pull(int lv, std::size_t idx) {
        const std::size_t cb = idx * (std::size_t)B;
        const std::size_t ce = std::min(seg[lv - 1].size(), cb + (std::size_t)B);

        const u64 s = sum_slice(seg[lv - 1], cb, ce);

        seg[lv][idx] = s;

        if (s == (u64)node_len(lv, idx)) {
            full[lv][idx] = 1;
        }
    }

    void set_rec(int lv, std::size_t idx, std::size_t ql, std::size_t qr) {
        auto [l, r] = node_range(lv, idx);

        if (qr <= l || r <= ql) return;
        if (full[lv][idx]) return;

        if (ql <= l && r <= qr) {
            set_node_full(lv, idx);
            return;
        }

        if (lv == 0) {
            const std::size_t il = std::max(l, ql);
            const std::size_t ir = std::min(r, qr);

            const unsigned lo = (unsigned)(il - l);
            const unsigned hi = (unsigned)(ir - l);

            const u64 m = mask_range(lo, hi);
            const u64 nb = bit[idx] | m;

            if (nb != bit[idx]) {
                bit[idx] = nb;
                seg[0][idx] = popcnt(nb);

                if (seg[0][idx] == (u64)(r - l)) {
                    full[0][idx] = 1;
                }
            }

            return;
        }

        const std::size_t cb = idx * (std::size_t)B;
        const std::size_t ce = std::min(seg[lv - 1].size(), cb + (std::size_t)B);

        for (std::size_t c = cb; c < ce; ++c) {
            auto [cl, cr] = node_range(lv - 1, c);
            if (qr <= cl || cr <= ql) continue;

            set_rec(lv - 1, c, ql, qr);
        }

        pull(lv, idx);
    }

    u64 sum_rec(int lv, std::size_t idx, std::size_t ql, std::size_t qr) const {
        auto [l, r] = node_range(lv, idx);

        if (qr <= l || r <= ql) return 0;

        if (ql <= l && r <= qr) {
            return seg[lv][idx];
        }

        const std::size_t il = std::max(l, ql);
        const std::size_t ir = std::min(r, qr);

        if (full[lv][idx]) {
            return (u64)(ir - il);
        }

        if (lv == 0) {
            const unsigned lo = (unsigned)(il - l);
            const unsigned hi = (unsigned)(ir - l);

            return popcnt(bit[idx] & mask_range(lo, hi));
        }

        u64 res = 0;

        const std::size_t cb = idx * (std::size_t)B;
        const std::size_t ce = std::min(seg[lv - 1].size(), cb + (std::size_t)B);

        for (std::size_t c = cb; c < ce; ) {
            auto [cl, cr] = node_range(lv - 1, c);

            if (qr <= cl || cr <= ql) {
                ++c;
                continue;
            }

            if (ql <= cl && cr <= qr) {
                std::size_t s = c;

                do {
                    ++c;
                    if (c >= ce) break;
                    std::tie(cl, cr) = node_range(lv - 1, c);
                } while (ql <= cl && cr <= qr);

                res += sum_slice(seg[lv - 1], s, c);
            } else {
                res += sum_rec(lv - 1, c, ql, qr);
                ++c;
            }
        }

        return res;
    }
};