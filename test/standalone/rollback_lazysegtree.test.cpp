// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../../src/structure/segtree/rollback_lazysegtree.hpp"

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
    int input_n, input_q;
    if(std::cin >> input_n >> input_q){
        std::vector<SumLen> input(static_cast<std::size_t>(input_n));
        for(auto& value: input){
            std::cin >> value.sum;
            value.len = 1;
        }
        RollbackLazySegtree<range_add_sum, 128, 700> seg(input);
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "SET"){
                int k;
                std::int64_t value;
                std::cin >> k >> value;
                seg.set(k, {value, 1});
                std::cout << seg.snapshot() << '\n';
            }else if(type == "ADD1"){
                int k;
                std::int64_t value;
                std::cin >> k >> value;
                seg.apply(k, value);
                std::cout << seg.snapshot() << '\n';
            }else if(type == "ADD"){
                int l, r;
                std::int64_t value;
                std::cin >> l >> r >> value;
                seg.apply(l, r, value);
                std::cout << seg.snapshot() << '\n';
            }else if(type == "UNDO"){
                seg.undo();
                std::cout << seg.snapshot() << '\n';
            }else if(type == "ROLLBACK"){
                int snapshot;
                std::cin >> snapshot;
                seg.rollback(snapshot);
                std::cout << seg.snapshot() << '\n';
            }else if(type == "GET"){
                int k;
                std::cin >> k;
                std::cout << seg.get(k).sum << '\n';
            }else if(type == "SUM"){
                int l, r;
                std::cin >> l >> r;
                std::cout << seg.prod(l, r).sum << '\n';
            }else if(type == "ALL"){
                std::cout << seg.all_prod().sum << '\n';
            }else if(type == "MR"){
                int l;
                std::int64_t limit;
                std::cin >> l >> limit;
                std::cout << seg.max_right(
                    l,
                    [](SumLen value, std::int64_t bound){
                        return value.sum <= bound;
                    },
                    limit
                ) << '\n';
            }else if(type == "ML"){
                int r;
                std::int64_t limit;
                std::cin >> r >> limit;
                std::cout << seg.min_left(
                    r,
                    [](SumLen value, std::int64_t bound){
                        return value.sum <= bound;
                    },
                    limit
                ) << '\n';
            }else if(type == "SNAP"){
                std::cout << seg.snapshot() << '\n';
            }
        }
        return 0;
    }
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
