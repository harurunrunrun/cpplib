// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/dsu/partially_persistent_dsu.hpp"

struct NaiveDSU{
    std::array<int, 30> parent{};
    int group_count = 30;

    NaiveDSU(){
        parent.fill(-1);
    }
    int leader(int v) const{
        while(parent[v] >= 0) v = parent[v];
        return v;
    }
    bool same(int u, int v) const{
        return leader(u) == leader(v);
    }
    int component_size(int v) const{
        return -parent[leader(v)];
    }
    bool merge(int u, int v){
        u = leader(u);
        v = leader(v);
        if(u == v) return false;
        if(-parent[u] < -parent[v]) std::swap(u, v);
        parent[u] += parent[v];
        parent[v] = u;
        group_count--;
        return true;
    }
};

void compare_all(const PartiallyPersistentDSU<30, 300>& dsu, int version, const NaiveDSU& naive){
    assert(dsu.groups(version) == naive.group_count);
    for(int i = 0; i < 30; i++){
        assert(dsu.component_size(version, i) == naive.component_size(i));
        for(int j = 0; j < 30; j++){
            assert(dsu.same(version, i, j) == naive.same(i, j));
        }
    }
}

int main(){
    int input_n, input_q;
    if(std::cin >> input_n >> input_q){
        PartiallyPersistentDSU<128, 2048> dsu(input_n);
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "MERGE"){
                int u, v;
                std::cin >> u >> v;
                std::cout << dsu.merge(u, v) << '\n';
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
            }else if(type == "LATEST"){
                std::cout << dsu.latest_version() << '\n';
            }
        }
        return 0;
    }

    {
        PartiallyPersistentDSU<8, 4> dsu(5);
        assert(dsu.latest_version() == 0);
        int v1 = dsu.merge(0, 1);
        int v2 = dsu.merge(1, 2);
        assert(v1 == 1);
        assert(v2 == 2);
        assert(!dsu.same(0, 0, 1));
        assert(dsu.same(1, 0, 1));
        assert(!dsu.same(1, 0, 2));
        assert(dsu.same(2, 0, 2));
        assert(dsu.component_size(0, 0) == 1);
        assert(dsu.component_size(1, 0) == 2);
        assert(dsu.component_size(2, 0) == 3);
    }

    PartiallyPersistentDSU<30, 300> dsu(30);
    std::vector<NaiveDSU> naive(1);
    std::mt19937 rng(20260714);

    for(int step = 0; step < 250; step++){
        int u = static_cast<int>(rng() % 30);
        int v = static_cast<int>(rng() % 30);
        NaiveDSU next = naive.back();
        next.merge(u, v);
        int version = dsu.merge(u, v);
        assert(version == static_cast<int>(naive.size()));
        naive.push_back(next);

        for(int t = 0; t < 20; t++){
            int qv = static_cast<int>(rng() % naive.size());
            int a = static_cast<int>(rng() % 30);
            int b = static_cast<int>(rng() % 30);
            assert(dsu.same(qv, a, b) == naive[static_cast<std::size_t>(qv)].same(a, b));
            assert(dsu.component_size(qv, a) == naive[static_cast<std::size_t>(qv)].component_size(a));
            assert(dsu.groups(qv) == naive[static_cast<std::size_t>(qv)].group_count);
        }
    }

    for(int version = 0; version < static_cast<int>(naive.size()); version++){
        compare_all(dsu, version, naive[static_cast<std::size_t>(version)]);
    }
}
