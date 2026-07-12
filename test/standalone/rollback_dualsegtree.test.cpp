// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../../src/structure/segtree/rollback_dualsegtree.hpp"

struct Affine{
    std::int64_t a;
    std::int64_t b;
};

std::int64_t op(std::int64_t, std::int64_t){
    return 0;
}

std::int64_t e(){
    return 0;
}

std::int64_t mapping(Affine f, std::int64_t x){
    return f.a * x + f.b;
}

Affine composition(Affine f, Affine g){
    return {f.a * g.a, f.a * g.b + f.b};
}

Affine id(){
    return {1, 0};
}

constexpr Monoid_Act<op, e, mapping, composition, id> affine{};

int main(){
    int input_n, input_q;
    if(std::cin >> input_n >> input_q){
        std::vector<std::int64_t> input(static_cast<std::size_t>(input_n));
        for(auto& value: input) std::cin >> value;
        RollbackDualSegtree<affine, 128, 700> seg(input);
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "SET"){
                int k;
                std::int64_t value;
                std::cin >> k >> value;
                seg.set(k, value);
                std::cout << seg.snapshot() << '\n';
            }else if(type == "APPLY"){
                int l, r;
                Affine f;
                std::cin >> l >> r >> f.a >> f.b;
                seg.apply(l, r, f);
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
                std::cout << seg.get(k) << '\n';
            }else if(type == "SNAP"){
                std::cout << seg.snapshot() << '\n';
            }
        }
        return 0;
    }
    constexpr int n = 33;
    std::mt19937 rng(424242);
    std::vector<std::int64_t> initial(n);
    for(auto& x: initial){
        x = rng() % 20;
    }

    RollbackDualSegtree<affine, 64, 1500> seg(initial);
    std::vector<std::vector<std::int64_t>> states = {initial};

    for(int step = 0; step < 10000; step++){
        int type = static_cast<int>(rng() % 6);
        if(type <= 2 && states.size() < 1400){
            auto next = states.back();
            if(type == 0){
                int k = static_cast<int>(rng() % n);
                std::int64_t x = rng() % 100;
                next[static_cast<std::size_t>(k)] = x;
                seg.set(k, x);
            }else{
                int l = static_cast<int>(rng() % (n + 1));
                int r = static_cast<int>(rng() % (n + 1));
                if(l > r){
                    std::swap(l, r);
                }
                Affine f{
                    static_cast<std::int64_t>(rng() % 2),
                    static_cast<std::int64_t>(rng() % 10)
                };
                for(int k = l; k < r; k++){
                    auto& x = next[static_cast<std::size_t>(k)];
                    x = mapping(f, x);
                }
                seg.apply(l, r, f);
            }
            states.push_back(next);
        }else if(type == 3 && states.size() > 1){
            seg.undo();
            states.pop_back();
        }else if(type == 4){
            int snapshot = static_cast<int>(rng() % states.size());
            seg.rollback(snapshot);
            states.resize(static_cast<std::size_t>(snapshot + 1));
        }else{
            for(int query = 0; query < 20; query++){
                int k = static_cast<int>(rng() % n);
                assert(
                    seg.get(k) ==
                    states.back()[static_cast<std::size_t>(k)]
                );
            }
        }
        assert(seg.snapshot() == static_cast<int>(states.size()) - 1);
        assert(seg.history_size() == seg.snapshot());
        assert(seg.can_undo() == (states.size() > 1));
    }
}
