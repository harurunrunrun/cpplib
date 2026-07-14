// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/dsu/weighted_dsu.hpp"

struct NaiveWeighted{
    int groups;
    std::vector<int> component;
    std::vector<long long> potential;
};

bool naive_merge(NaiveWeighted& state, int u, int v, long long w){
    if(state.component[static_cast<std::size_t>(u)] == state.component[static_cast<std::size_t>(v)]){
        return state.potential[static_cast<std::size_t>(v)] -
               state.potential[static_cast<std::size_t>(u)] == w;
    }
    int from = state.component[static_cast<std::size_t>(v)];
    int to = state.component[static_cast<std::size_t>(u)];
    long long add = state.potential[static_cast<std::size_t>(u)] + w -
                    state.potential[static_cast<std::size_t>(v)];
    for(int i = 0; i < static_cast<int>(state.component.size()); i++){
        if(state.component[static_cast<std::size_t>(i)] == from){
            state.component[static_cast<std::size_t>(i)] = to;
            state.potential[static_cast<std::size_t>(i)] += add;
        }
    }
    state.groups--;
    return true;
}

int naive_size(const NaiveWeighted& state, int v){
    int component = state.component[static_cast<std::size_t>(v)];
    int result = 0;
    for(int x: state.component) if(x == component) result++;
    return result;
}

void check_all(WeightedDSU<long long, 64>& dsu, const NaiveWeighted& naive){
    int n = static_cast<int>(naive.component.size());
    assert(dsu.size() == n);
    assert(dsu.groups() == naive.groups);
    for(int i = 0; i < n; i++){
        assert(dsu.component_size(i) == naive_size(naive, i));
        for(int j = 0; j < n; j++){
            bool same = naive.component[static_cast<std::size_t>(i)] ==
                        naive.component[static_cast<std::size_t>(j)];
            assert(dsu.same(i, j) == same);
            const auto difference = dsu.diff_if_connected(i, j);
            assert(difference.has_value() == same);
            if(same){
                const long long expected = naive.potential[static_cast<std::size_t>(j)] -
                                           naive.potential[static_cast<std::size_t>(i)];
                assert(*difference == expected);
                assert(dsu.diff(i, j) == expected);
            }
        }
    }
}

void self_test(){
    {
        WeightedDSU<long long, 8> dsu(4);
        assert(dsu.merge(0, 1, 3));
        assert(dsu.merge(1, 2, 4));
        assert(dsu.same(0, 2));
        assert(dsu.diff(0, 2) == 7);
        const auto difference = dsu.diff_if_connected(0, 2);
        assert(difference && *difference == 7);
        assert(!dsu.diff_if_connected(0, 3));
        assert(dsu.merge(0, 2, 7));
        assert(!dsu.merge(0, 2, 8));
        assert(dsu.groups() == 2);
    }

    std::mt19937 rng(20261116);
    for(int n = 1; n <= 20; n++){
        for(int step = 0; step < 80; step++){
            WeightedDSU<long long, 64> dsu(n);
            NaiveWeighted naive;
            naive.groups = n;
            naive.component.resize(static_cast<std::size_t>(n));
            naive.potential.assign(static_cast<std::size_t>(n), 0);
            for(int i = 0; i < n; i++) naive.component[static_cast<std::size_t>(i)] = i;
            for(int op = 0; op < 150; op++){
                int type = static_cast<int>(rng() % 5);
                if(type < 3){
                    int u = static_cast<int>(rng() % n);
                    int v = static_cast<int>(rng() % n);
                    long long w = static_cast<int>(rng() % 81) - 40;
                    assert(dsu.merge(u, v, w) == naive_merge(naive, u, v, w));
                }else{
                    check_all(dsu, naive);
                }
            }
            check_all(dsu, naive);
        }
    }

    bool thrown = false;
    try{
        WeightedDSU<long long, 4> dsu(2);
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
    WeightedDSU<long long, 256> dsu(n);
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
            const auto difference = dsu.diff_if_connected(u, v);
            if(difference) std::cout << *difference << '\n';
            else std::cout << "NA\n";
        }else if(type == "GROUPS"){
            std::cout << dsu.groups() << '\n';
        }else if(type == "SIZE"){
            int v;
            std::cin >> v;
            std::cout << dsu.component_size(v) << '\n';
        }
    }
}
