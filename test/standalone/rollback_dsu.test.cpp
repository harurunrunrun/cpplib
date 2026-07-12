// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/dsu/rollback_dsu.hpp"

struct NaiveDSU{
    int groups;
    std::vector<int> component;
};

bool naive_merge(NaiveDSU& state, int u, int v){
    if(state.component[static_cast<std::size_t>(u)] == state.component[static_cast<std::size_t>(v)]){
        return false;
    }
    int old = state.component[static_cast<std::size_t>(v)];
    int next = state.component[static_cast<std::size_t>(u)];
    for(int& x: state.component) if(x == old) x = next;
    state.groups--;
    return true;
}

int naive_size(const NaiveDSU& state, int v){
    int component = state.component[static_cast<std::size_t>(v)];
    int result = 0;
    for(int x: state.component) if(x == component) result++;
    return result;
}

void check_all(const RollbackDSU<64, 512>& dsu, const NaiveDSU& naive){
    int n = static_cast<int>(naive.component.size());
    assert(dsu.size() == n);
    assert(dsu.groups() == naive.groups);
    for(int i = 0; i < n; i++){
        assert(dsu.component_size(i) == naive_size(naive, i));
        for(int j = 0; j < n; j++){
            bool same = naive.component[static_cast<std::size_t>(i)] ==
                        naive.component[static_cast<std::size_t>(j)];
            assert(dsu.same(i, j) == same);
        }
    }
}

void self_test(){
    {
        RollbackDSU<8, 32> dsu(4);
        assert(dsu.merge(0, 1));
        assert(dsu.merge(2, 3));
        int s = dsu.snapshot();
        assert(dsu.merge(1, 2));
        assert(dsu.same(0, 3));
        dsu.rollback(s);
        assert(!dsu.same(0, 3));
        dsu.undo();
        assert(!dsu.same(2, 3));
        assert(dsu.same(0, 1));
    }

    std::mt19937 rng(20261116);
    for(int n = 1; n <= 20; n++){
        for(int step = 0; step < 80; step++){
            RollbackDSU<64, 512> dsu(n);
            NaiveDSU naive;
            naive.groups = n;
            naive.component.resize(static_cast<std::size_t>(n));
            for(int i = 0; i < n; i++) naive.component[static_cast<std::size_t>(i)] = i;
            std::vector<NaiveDSU> states = {naive};
            for(int op = 0; op < 150; op++){
                int type = static_cast<int>(rng() % 5);
                if(type < 3){
                    int u = static_cast<int>(rng() % n);
                    int v = static_cast<int>(rng() % n);
                    assert(dsu.merge(u, v) == naive_merge(naive, u, v));
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
                assert(dsu.snapshot() == static_cast<int>(states.size()) - 1);
                check_all(dsu, naive);
            }
        }
    }

    bool thrown = false;
    try{
        RollbackDSU<4, 8> dsu(2);
        (void)dsu.same(0, 2);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    RollbackDSU<256, 4096> dsu(n);
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "MERGE"){
            int u, v;
            std::cin >> u >> v;
            std::cout << dsu.merge(u, v) << '\n';
        }else if(type == "SAME"){
            int u, v;
            std::cin >> u >> v;
            std::cout << dsu.same(u, v) << '\n';
        }else if(type == "SIZE"){
            int v;
            std::cin >> v;
            std::cout << dsu.component_size(v) << '\n';
        }else if(type == "GROUPS"){
            std::cout << dsu.groups() << '\n';
        }else if(type == "SNAP"){
            std::cout << dsu.snapshot() << '\n';
        }else if(type == "UNDO"){
            dsu.undo();
        }else if(type == "ROLLBACK"){
            int snap;
            std::cin >> snap;
            dsu.rollback(snap);
        }
    }
}
