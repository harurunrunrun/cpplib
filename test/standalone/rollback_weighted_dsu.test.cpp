// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "../../src/structure/dsu/rollback_weighted_dsu.hpp"

struct Naive{
    int groups;
    std::vector<int> component;
    std::vector<long long> potential;
};

bool naive_merge(Naive& state, int u, int v, long long w){
    if(state.component[u] == state.component[v]){
        return state.potential[v] - state.potential[u] == w;
    }
    int from = state.component[v];
    int to = state.component[u];
    long long add = state.potential[u] + w - state.potential[v];
    for(int i = 0; i < static_cast<int>(state.component.size()); i++){
        if(state.component[i] == from){
            state.component[i] = to;
            state.potential[i] += add;
        }
    }
    state.groups--;
    return true;
}

void check_all(const RollbackWeightedDSU<long long, 32, 2000>& dsu, const Naive& naive){
    int n = static_cast<int>(naive.component.size());
    assert(dsu.groups() == naive.groups);
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            assert(dsu.same(i, j) == (naive.component[i] == naive.component[j]));
            if(dsu.same(i, j)){
                assert(dsu.diff(i, j) == naive.potential[j] - naive.potential[i]);
            }
        }
    }
}

void self_test(){
    {
        RollbackWeightedDSU<long long, 8, 32> dsu(4);
        assert(dsu.merge(0, 1, 3));
        assert(dsu.merge(1, 2, 4));
        int s = dsu.snapshot();
        assert(dsu.same(0, 2));
        assert(dsu.diff(0, 2) == 7);
        assert(!dsu.merge(0, 2, 8));
        dsu.undo();
        assert(dsu.diff(0, 2) == 7);
        assert(dsu.merge(2, 3, -5));
        assert(dsu.diff(0, 3) == 2);
        dsu.rollback(s);
        assert(!dsu.same(0, 3));
        assert(dsu.diff(0, 2) == 7);
    }

    std::mt19937 rng(20261115);
    for(int n = 1; n <= 12; n++){
        for(int step = 0; step < 80; step++){
            RollbackWeightedDSU<long long, 32, 2000> dsu(n);
            Naive initial;
            initial.groups = n;
            initial.component.resize(static_cast<std::size_t>(n));
            initial.potential.assign(static_cast<std::size_t>(n), 0);
            for(int i = 0; i < n; i++) initial.component[static_cast<std::size_t>(i)] = i;
            Naive naive = initial;
            std::vector<Naive> states = {naive};

            for(int op = 0; op < 200; op++){
                int type = static_cast<int>(rng() % 5);
                if(type < 3){
                    int u = static_cast<int>(rng() % n);
                    int v = static_cast<int>(rng() % n);
                    long long w = static_cast<int>(rng() % 41) - 20;
                    bool expected = naive_merge(naive, u, v, w);
                    assert(dsu.merge(u, v, w) == expected);
                    states.push_back(naive);
                }else if(type == 3 && states.size() > 1){
                    dsu.undo();
                    states.pop_back();
                    naive = states.back();
                }else{
                    int snap = static_cast<int>(rng() % states.size());
                    dsu.rollback(snap);
                    states.resize(static_cast<std::size_t>(snap + 1));
                    naive = states.back();
                }
                check_all(dsu, naive);
            }
        }
    }
}

int main(){
    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    RollbackWeightedDSU<long long, 256, 4096> dsu(n);
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "MERGE"){
            int u, v;
            long long w;
            std::cin >> u >> v >> w;
            std::cout << dsu.merge(u, v, w) << '\n';
        }else if(type == "SAME"){
            int u, v;
            std::cin >> u >> v;
            std::cout << dsu.same(u, v) << '\n';
        }else if(type == "DIFF"){
            int u, v;
            std::cin >> u >> v;
            if(dsu.same(u, v)) std::cout << dsu.diff(u, v) << '\n';
            else std::cout << "NA\n";
        }else if(type == "SNAP"){
            std::cout << dsu.snapshot() << '\n';
        }else if(type == "UNDO"){
            dsu.undo();
        }else if(type == "ROLLBACK"){
            int snap;
            std::cin >> snap;
            dsu.rollback(snap);
        }else if(type == "GROUPS"){
            std::cout << dsu.groups() << '\n';
        }
    }
}
