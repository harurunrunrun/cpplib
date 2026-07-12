// competitive-verifier: STANDALONE

#define PROBLEM "https://example.com/standalone/persistent_dsu"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include "../../src/structure/dsu/persistent_dsu.hpp"

struct NaiveDSU{
    std::vector<int> parent_or_size;

    explicit NaiveDSU(int n = 0): parent_or_size(n, -1){}

    int leader(int v) const{
        while(parent_or_size[v] >= 0) v = parent_or_size[v];
        return v;
    }

    bool same(int u, int v) const{
        return leader(u) == leader(v);
    }

    int component_size(int v) const{
        return -parent_or_size[leader(v)];
    }

    int groups() const{
        int res = 0;
        for(int x: parent_or_size) res += x < 0;
        return res;
    }

    void merge(int u, int v){
        u = leader(u);
        v = leader(v);
        if(u == v) return;
        if(-parent_or_size[u] < -parent_or_size[v]) std::swap(u, v);
        parent_or_size[u] += parent_or_size[v];
        parent_or_size[v] = u;
    }
};

int main(){
    int input_n, input_q;
    if(std::cin >> input_n >> input_q){
        PersistentDSU<128, 2048> dsu(input_n);
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "MERGE"){
                int version, u, v;
                std::cin >> version >> u >> v;
                std::cout << dsu.merge(version, u, v) << '\n';
            }else if(type == "FORK"){
                int version;
                std::cin >> version;
                std::cout << dsu.fork(version) << '\n';
            }else if(type == "SAME"){
                int version, u, v;
                std::cin >> version >> u >> v;
                std::cout << dsu.same(version, u, v) << '\n';
            }else if(type == "SIZE"){
                int version, v;
                std::cin >> version >> v;
                std::cout << dsu.component_size(version, v) << '\n';
            }else if(type == "GROUPS"){
                int version;
                std::cin >> version;
                std::cout << dsu.groups(version) << '\n';
            }else if(type == "LEADER"){
                int version, v;
                std::cin >> version >> v;
                std::cout << dsu.leader(version, v) << '\n';
            }
        }
        return 0;
    }

    constexpr int N = 9;
    PersistentDSU<N, 300> dsu(N);
    std::vector<NaiveDSU> naive(1, NaiveDSU(N));

    assert(dsu.size() == N);
    assert(dsu.versions() == 1);
    assert(dsu.latest_version() == 0);
    assert(dsu.groups(0) == N);

    std::mt19937 rng(123456789);

    for(int step = 0; step < 220; step++){
        int base = static_cast<int>(rng() % naive.size());
        int u = static_cast<int>(rng() % N);
        int v = static_cast<int>(rng() % N);
        int type = static_cast<int>(rng() % 5);

        if(type == 0){
            int created = dsu.fork(base);
            naive.push_back(naive[base]);
            assert(created == static_cast<int>(naive.size()) - 1);
        }else if(type <= 2){
            int created = dsu.merge(base, u, v);
            NaiveDSU expected = naive[base];
            expected.merge(u, v);
            naive.push_back(expected);
            assert(created == static_cast<int>(naive.size()) - 1);
        }else{
            assert(dsu.same(base, u, v) == naive[base].same(u, v));
            assert(dsu.component_size(base, u) == naive[base].component_size(u));
            assert(dsu.groups(base) == naive[base].groups());
        }

        for(int version = 0; version < static_cast<int>(naive.size()); version++){
            assert(dsu.groups(version) == naive[version].groups());
            for(int a = 0; a < N; a++){
                assert(dsu.component_size(version, a) == naive[version].component_size(a));
                for(int b = 0; b < N; b++){
                    assert(dsu.same(version, a, b) == naive[version].same(a, b));
                }
            }
        }
    }

    assert(dsu.versions() == static_cast<int>(naive.size()));
    assert(dsu.latest_version() == dsu.versions() - 1);
}
