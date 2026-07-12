#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <numeric>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include "../../../src/structure/segtree/segtree_beats.hpp"

using ll = long long;
using Beats = SegmentTreeBeats<ll, 64>;

static_assert(!std::is_copy_constructible_v<Beats>);
static_assert(!std::is_move_constructible_v<Beats>);

inline void run_segtree_beats_standalone_check(){
    constexpr int N = 37;
    std::mt19937_64 rng(0x123456789abcdef0ULL);

    std::vector<ll> expected(N);
    for(ll& x: expected){
        x = (ll)(rng() % 201) - 100;
    }
    Beats seg(expected);

    for(int q = 0; q < 30000; q++){
        int l = (int)(rng() % (N + 1));
        int r = (int)(rng() % (N + 1));
        if(l > r){
            std::swap(l, r);
        }

        int type = (int)(rng() % 8);
        if(type == 0){
            ll x = (ll)(rng() % 301) - 150;
            seg.range_chmin(l, r, x);
            for(int k = l; k < r; k++){
                expected[k] = std::min(expected[k], x);
            }
        }else if(type == 1){
            ll x = (ll)(rng() % 301) - 150;
            seg.range_chmax(l, r, x);
            for(int k = l; k < r; k++){
                expected[k] = std::max(expected[k], x);
            }
        }else if(type == 2){
            ll x = (ll)(rng() % 101) - 50;
            seg.range_add(l, r, x);
            for(int k = l; k < r; k++){
                expected[k] += x;
            }
        }else if(type == 3){
            ll sum = std::accumulate(
                expected.begin() + l, expected.begin() + r, 0LL
            );
            assert(seg.range_sum(l, r) == sum);
        }else if(type == 4){
            int k = (int)(rng() % N);
            ll x = (ll)(rng() % 301) - 150;
            seg.set(k, x);
            expected[k] = x;
        }else if(type == 5){
            int k = (int)(rng() % N);
            assert(seg.get(k) == expected[k]);
        }else{
            int left = (int)(rng() % N);
            int right = left + 1 + (int)(rng() % (N - left));
            if(type == 6){
                assert(
                    seg.range_min(left, right) ==
                    *std::min_element(
                        expected.begin() + left, expected.begin() + right
                    )
                );
            }else{
                assert(
                    seg.range_max(left, right) ==
                    *std::max_element(
                        expected.begin() + left, expected.begin() + right
                    )
                );
            }
        }

        ll sum = std::accumulate(expected.begin(), expected.end(), 0LL);
        assert(seg.all_sum() == sum);
        assert(seg.all_min() == *std::min_element(expected.begin(), expected.end()));
        assert(seg.all_max() == *std::max_element(expected.begin(), expected.end()));

        for(int rep = 0; rep < 3; rep++){
            int k = (int)(rng() % N);
            assert(seg.get(k) == expected[k]);
        }
    }

    std::array<ll, 3> padded_initial{0, 0, 0};
    SegmentTreeBeats<ll, 5> padded(padded_initial);
    padded.range_add(2, 3, 5);
    assert(padded.get(2) == 5);
    assert(padded.get(2) == 5);

    std::array<ll, 2> cross_initial{1, 5};
    SegmentTreeBeats<ll, 2> cross(cross_initial);
    cross.range_chmin(0, 2, 3);
    cross.range_chmax(0, 2, 4);
    assert(cross.range_sum(0, 2) == 8);

    SegmentTreeBeats<ll, 2> reverse_cross(cross_initial);
    reverse_cross.range_chmax(0, 2, 3);
    reverse_cross.range_chmin(0, 2, 2);
    assert(reverse_cross.range_sum(0, 2) == 4);

    SegmentTreeBeats<ll, 4> empty(0);
    assert(empty.all_sum() == 0);
    assert(empty.range_sum(0, 0) == 0);

    bool thrown = false;
    try{
        empty.all_min();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        seg.range_max(5, 5);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}
