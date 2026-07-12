// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <random>
#include <vector>

#include "../src/structure/segtree/rollback_lazysegtree.hpp"

struct SumLen{
    std::int64_t sum;
    int len;
};

SumLen op(SumLen a, SumLen b){
    return {a.sum + b.sum, a.len + b.len};
}

SumLen e(){
    return {0, 0};
}

SumLen mapping(std::int64_t f, SumLen x){
    return {x.sum + f * x.len, x.len};
}

std::int64_t composition(std::int64_t f, std::int64_t g){
    return f + g;
}

std::int64_t id(){
    return 0;
}

constexpr Monoid_Act<op, e, mapping, composition, id> range_add_sum{};

std::int64_t naive_prod(
    const std::vector<std::int64_t>& a,
    int l,
    int r
){
    std::int64_t res = 0;
    for(int k = l; k < r; k++){
        res += a[static_cast<std::size_t>(k)];
    }
    return res;
}

int main(){
    constexpr int n = 37;
    std::mt19937 rng(271828);
    std::vector<SumLen> init(n);
    std::vector<std::int64_t> initial(n);
    for(int k = 0; k < n; k++){
        initial[static_cast<std::size_t>(k)] = rng() % 20;
        init[static_cast<std::size_t>(k)] = {
            initial[static_cast<std::size_t>(k)],
            1
        };
    }

    RollbackLazySegtree<range_add_sum, 64, 1500> seg(init);
    std::vector<std::vector<std::int64_t>> states = {initial};

    for(int step = 0; step < 10000; step++){
        int type = static_cast<int>(rng() % 8);
        if(type <= 3 && states.size() < 1400){
            auto next = states.back();
            if(type == 0){
                int k = static_cast<int>(rng() % n);
                std::int64_t x = rng() % 100;
                next[static_cast<std::size_t>(k)] = x;
                seg.set(k, {x, 1});
            }else if(type == 1){
                int k = static_cast<int>(rng() % n);
                std::int64_t add = rng() % 10;
                next[static_cast<std::size_t>(k)] += add;
                seg.apply(k, add);
            }else{
                int l = static_cast<int>(rng() % (n + 1));
                int r = static_cast<int>(rng() % (n + 1));
                if(l > r){
                    std::swap(l, r);
                }
                std::int64_t add = rng() % 10;
                for(int k = l; k < r; k++){
                    next[static_cast<std::size_t>(k)] += add;
                }
                seg.apply(l, r, add);
            }
            states.push_back(next);
        }else if(type == 4 && states.size() > 1){
            seg.undo();
            states.pop_back();
        }else if(type == 5){
            int snapshot = static_cast<int>(rng() % states.size());
            seg.rollback(snapshot);
            states.resize(static_cast<std::size_t>(snapshot + 1));
        }else{
            int l = static_cast<int>(rng() % (n + 1));
            int r = static_cast<int>(rng() % (n + 1));
            if(l > r){
                std::swap(l, r);
            }
            assert(
                seg.prod(l, r).sum ==
                naive_prod(states.back(), l, r)
            );
            int k = static_cast<int>(rng() % n);
            assert(
                seg.get(k).sum ==
                states.back()[static_cast<std::size_t>(k)]
            );
            assert(
                seg.all_prod().sum ==
                naive_prod(states.back(), 0, n)
            );

            std::int64_t limit = rng() % 1000;
            int expected_right = l;
            std::int64_t sum = 0;
            while(
                expected_right < n &&
                sum + states.back()[static_cast<std::size_t>(expected_right)]
                    <= limit
            ){
                sum += states.back()[static_cast<std::size_t>(expected_right++)];
            }
            assert(seg.max_right(
                l,
                [](SumLen x, std::int64_t bound){
                    return x.sum <= bound;
                },
                limit
            ) == expected_right);

            int expected_left = r;
            sum = 0;
            while(
                expected_left > 0 &&
                sum + states.back()[static_cast<std::size_t>(expected_left - 1)]
                    <= limit
            ){
                sum += states.back()[static_cast<std::size_t>(--expected_left)];
            }
            assert(seg.min_left(
                r,
                [](SumLen x, std::int64_t bound){
                    return x.sum <= bound;
                },
                limit
            ) == expected_left);
        }

        assert(seg.snapshot() == static_cast<int>(states.size()) - 1);
        assert(seg.history_size() == seg.snapshot());
        assert(seg.can_undo() == (states.size() > 1));
    }
}
