// competitive-verifier: STANDALONE

#include <cassert>
#include <random>
#include <vector>

#define WST_NO_AVX2
#define Bit01WST BitAssignRangeSumTree
#include "../../src/structure/segtree/specialized/bitassign_rangesum.hpp"
#undef Bit01WST
#define Bit01WST BitAssignFlipRangeSumTree
#include "../../src/structure/segtree/specialized/bitassign_rangesum_rangeflip.hpp"
#undef Bit01WST

static_assert(BIT01_WST_USE_AVX2 == 0);

int main(){
    std::mt19937 rng(20261114);
    for(int n: {1, 63, 64, 65, 700, 3000}){
        std::vector<int> naive(static_cast<std::size_t>(n));
        for(int& value: naive) value = static_cast<int>(rng() & 1);
        BitAssignRangeSumTree assign_tree(naive);
        BitAssignFlipRangeSumTree flip_tree(naive);
        std::vector<int> flip_naive = naive;
        for(int step = 0; step < 2000; ++step){
            int l = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
            int r = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
            if(l > r) std::swap(l, r);
            const unsigned kind = rng() % 3;
            if(kind == 0){
                const bool value = (rng() & 1) != 0;
                assign_tree.assign(l, r, value);
                flip_tree.assign(l, r, value);
                for(int i = l; i < r; ++i){
                    naive[static_cast<std::size_t>(i)] = value;
                    flip_naive[static_cast<std::size_t>(i)] = value;
                }
            }else if(kind == 1){
                flip_tree.flip(l, r);
                for(int i = l; i < r; ++i) flip_naive[static_cast<std::size_t>(i)] ^= 1;
            }else{
                long long expected = 0, flip_expected = 0;
                for(int i = l; i < r; ++i){
                    expected += naive[static_cast<std::size_t>(i)];
                    flip_expected += flip_naive[static_cast<std::size_t>(i)];
                }
                assert(static_cast<long long>(assign_tree.sum(l, r)) == expected);
                assert(static_cast<long long>(flip_tree.sum(l, r)) == flip_expected);
                if(l < n){
                    assert(static_cast<int>(assign_tree.get(l)) == naive[static_cast<std::size_t>(l)]);
                    assert(static_cast<int>(flip_tree.get(l)) == flip_naive[static_cast<std::size_t>(l)]);
                }
            }
        }
    }
}
