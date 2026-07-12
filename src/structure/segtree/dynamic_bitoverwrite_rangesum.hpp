#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#if defined(__GNUC__) && !defined(__clang__) && \
    (defined(__x86_64__) || defined(__i386__)) && !defined(WST_NO_AVX2)
#pragma GCC target("avx2,popcnt,bmi,bmi2")
#define WST_HAS_AVX2 1
#include <immintrin.h>
#endif

template<int B = 16>
struct DynamicWideSetSum {
    using u64 = std::uint64_t;

    static_assert(B >= 2);
#if defined(WST_HAS_AVX2)
    static_assert(B % 4 == 0, "B should be a multiple of 4 for AVX2");
#endif

    static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

    struct Node {
        u64 sum = 0;
        u64 bit = 0;
        std::uint8_t full = 0;
        std::array<u64, B> child_sum{};
        std::array<std::uint8_t, B> child_full{};
        std::array<std::size_t, B> child{};

        Node() {
            child.fill(npos);
        }
    };

    std::size_t n = 0;          // 元の配列長
    std::size_t w = 0;          // uint64_t ブロック数
    std::size_t root = npos;
    std::vector<Node> seg;

    DynamicWideSetSum() = default;

    explicit DynamicWideSetSum(std::size_t n_) {
        init(n_);
    }

    void init(std::size_t n_) {
        n = n_;
        w = (n >> 6) + ((n & 63) != 0);

        seg.clear();
        root = w == 0 ? npos : make_node();
    }

    // a[l, r) = 1
    void set(std::size_t l, std::size_t r) {
        assert(l <= r && r <= n);
        if (l == r || root == npos) return;
        set_rec(root, 0, w, l, r);
    }

    // sum a[l, r)
    u64 sum(std::size_t l, std::size_t r) const {
        assert(l <= r && r <= n);
        if (l == r || root == npos) return 0;
        return sum_rec(root, 0, w, l, r);
    }

    u64 all_sum() const {
        return root == npos ? 0 : seg[root].sum;
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

    std::size_t make_node() {
        seg.emplace_back();
        return seg.size() - 1;
    }

    std::size_t word_to_bit(std::size_t x) const {
        return x >= w ? n : std::min(n, x << 6);
    }

    std::size_t node_len(std::size_t wl, std::size_t span) const {
        return word_to_bit(wl + span) - word_to_bit(wl);
    }

    void child_range(std::size_t wl, std::size_t span, std::size_t k,
                     std::size_t& cwl, std::size_t& cspan) const {
        const std::size_t unit = (span + (std::size_t)B - 1) / (std::size_t)B;
        cwl = wl + k * unit;

        if (cwl >= wl + span) {
            cspan = 0;
            return;
        }

        const std::size_t rwl = std::min(wl + span, cwl + unit);
        cspan = rwl - cwl;
    }

    u64 sum_slice(const std::array<u64, B>& a, std::size_t l, std::size_t r) const {
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

    void set_node_full(std::size_t idx, std::size_t wl, std::size_t span) {
        Node& v = seg[idx];
        const std::size_t len = node_len(wl, span);

        v.sum = (u64)len;
        v.full = 1;
        v.child.fill(npos);

        if (span == 1) {
            v.bit = low_mask((unsigned)len);
        }
    }

    void set_child_full(std::size_t idx, std::size_t k,
                        std::size_t cwl, std::size_t cspan) {
        Node& v = seg[idx];
        v.child_sum[k] = (u64)node_len(cwl, cspan);
        v.child_full[k] = 1;
        v.child[k] = npos;
    }

    void pull(std::size_t idx, std::size_t wl, std::size_t span) {
        Node& v = seg[idx];
        const u64 s = sum_slice(v.child_sum, 0, (std::size_t)B);

        v.sum = s;

        if (s == (u64)node_len(wl, span)) {
            set_node_full(idx, wl, span);
        } else {
            v.full = 0;
        }
    }

    void set_rec(std::size_t idx, std::size_t wl, std::size_t span,
                 std::size_t ql, std::size_t qr) {
        const std::size_t l = word_to_bit(wl);
        const std::size_t r = word_to_bit(wl + span);

        if (qr <= l || r <= ql) return;
        if (seg[idx].full) return;

        if (ql <= l && r <= qr) {
            set_node_full(idx, wl, span);
            return;
        }

        if (span == 1) {
            const std::size_t il = std::max(l, ql);
            const std::size_t ir = std::min(r, qr);

            const unsigned lo = (unsigned)(il - l);
            const unsigned hi = (unsigned)(ir - l);

            const u64 m = mask_range(lo, hi);
            const u64 nb = seg[idx].bit | m;

            if (nb != seg[idx].bit) {
                seg[idx].bit = nb;
                seg[idx].sum = popcnt(nb);

                if (seg[idx].sum == (u64)(r - l)) {
                    seg[idx].full = 1;
                }
            }

            return;
        }

        for (std::size_t k = 0; k < (std::size_t)B; ++k) {
            std::size_t cwl = 0;
            std::size_t cspan = 0;
            child_range(wl, span, k, cwl, cspan);
            if (cspan == 0) break;

            const std::size_t cl = word_to_bit(cwl);
            const std::size_t cr = word_to_bit(cwl + cspan);

            if (qr <= cl || cr <= ql) continue;
            if (seg[idx].child_full[k]) continue;

            if (ql <= cl && cr <= qr) {
                set_child_full(idx, k, cwl, cspan);
                continue;
            }

            if (seg[idx].child[k] == npos) {
                seg[idx].child[k] = make_node();
            }

            set_rec(seg[idx].child[k], cwl, cspan, ql, qr);

            const std::size_t cidx = seg[idx].child[k];
            seg[idx].child_sum[k] = seg[cidx].sum;
            seg[idx].child_full[k] = seg[cidx].full;

            if (seg[idx].child_full[k]) {
                seg[idx].child[k] = npos;
            }
        }

        pull(idx, wl, span);
    }

    u64 sum_rec(std::size_t idx, std::size_t wl, std::size_t span,
                std::size_t ql, std::size_t qr) const {
        const Node& v = seg[idx];
        const std::size_t l = word_to_bit(wl);
        const std::size_t r = word_to_bit(wl + span);

        if (qr <= l || r <= ql) return 0;

        const std::size_t il = std::max(l, ql);
        const std::size_t ir = std::min(r, qr);

        if (ql <= l && r <= qr) {
            return v.sum;
        }

        if (v.full) {
            return (u64)(ir - il);
        }

        if (span == 1) {
            const unsigned lo = (unsigned)(il - l);
            const unsigned hi = (unsigned)(ir - l);

            return popcnt(v.bit & mask_range(lo, hi));
        }

        u64 res = 0;

        for (std::size_t k = 0; k < (std::size_t)B; ) {
            std::size_t cwl = 0;
            std::size_t cspan = 0;
            child_range(wl, span, k, cwl, cspan);
            if (cspan == 0) break;

            const std::size_t cl = word_to_bit(cwl);
            const std::size_t cr = word_to_bit(cwl + cspan);

            if (qr <= cl || cr <= ql) {
                ++k;
                continue;
            }

            if (ql <= cl && cr <= qr) {
                const std::size_t s = k;

                do {
                    ++k;
                    if (k >= (std::size_t)B) break;
                    child_range(wl, span, k, cwl, cspan);
                    if (cspan == 0) break;
                    const std::size_t nl = word_to_bit(cwl);
                    const std::size_t nr = word_to_bit(cwl + cspan);
                    if (!(ql <= nl && nr <= qr)) break;
                } while (true);

                res += sum_slice(v.child_sum, s, k);
            } else {
                const std::size_t sl = std::max(cl, ql);
                const std::size_t sr = std::min(cr, qr);

                if (v.child_full[k]) {
                    res += (u64)(sr - sl);
                } else if (v.child[k] != npos) {
                    res += sum_rec(v.child[k], cwl, cspan, ql, qr);
                }

                ++k;
            }
        }

        return res;
    }
};
