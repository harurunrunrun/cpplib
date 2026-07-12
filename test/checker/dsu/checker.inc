#include <array>
#include <cassert>
#include <random>
#include <vector>
#include "../../../src/structure/dsu/dsu.hpp"
#include "../../../src/structure/dsu/rollback_dsu.hpp"
#include "../../../src/structure/dsu/weighted_dsu.hpp"

struct NaiveDSU{
    std::array<int, 20> parent{};
    int groups = 20;

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
        groups--;
        return true;
    }
};

struct Snapshot{
    int id;
    NaiveDSU naive;
};

void compare_all(const RollbackDSU<20, 2000>& dsu, const NaiveDSU& naive){
    assert(dsu.groups() == naive.groups);
    for(int i = 0; i < 20; i++){
        assert(dsu.component_size(i) == naive.component_size(i));
        for(int j = 0; j < 20; j++){
            assert(dsu.same(i, j) == naive.same(i, j));
        }
    }
}

inline void run_dsu_standalone_check(){
    {
        DSU<10> dsu(6);
        assert(dsu.groups() == 6);
        assert(dsu.merge(0, 1));
        assert(dsu.merge(2, 3));
        assert(!dsu.merge(0, 1));
        assert(dsu.same(0, 1));
        assert(!dsu.same(0, 2));
        assert(dsu.component_size(0) == 2);
        assert(dsu.groups() == 4);
    }

    {
        WeightedDSU<long long, 10> dsu(5);
        assert(dsu.merge(0, 1, 3));
        assert(dsu.merge(1, 2, 4));
        assert(dsu.same(0, 2));
        assert(dsu.diff(0, 2) == 7);
        assert(dsu.diff(2, 0) == -7);
        assert(dsu.merge(0, 2, 7));
        assert(!dsu.merge(0, 2, 8));
    }

    RollbackDSU<20, 2000> dsu(20);
    NaiveDSU naive;
    std::vector<Snapshot> snapshots = {{dsu.snapshot(), naive}};
    std::mt19937 rng(991);
    for(int step = 0; step < 1000; step++){
        int type = static_cast<int>(rng() % 10);
        if(type < 6){
            int u = static_cast<int>(rng() % 20);
            int v = static_cast<int>(rng() % 20);
            bool expected = naive.merge(u, v);
            assert(dsu.merge(u, v) == expected);
        }else if(type < 8){
            snapshots.push_back({dsu.snapshot(), naive});
        }else{
            int id = static_cast<int>(rng() % snapshots.size());
            int snapshot_id = snapshots[static_cast<std::size_t>(id)].id;
            naive = snapshots[static_cast<std::size_t>(id)].naive;
            dsu.rollback(snapshot_id);
            std::vector<Snapshot> next;
            for(const auto& snapshot: snapshots){
                if(snapshot.id <= snapshot_id) next.push_back(snapshot);
            }
            snapshots.swap(next);
        }
        compare_all(dsu, naive);
    }
}
