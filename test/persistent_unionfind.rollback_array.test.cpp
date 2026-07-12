// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/persistent_unionfind

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>
#include "../src/structure/array/rollback_array.hpp"

constexpr int MAX_N = 200000;

RollbackArray<int, MAX_N, 2 * MAX_N> dsu(MAX_N, -1);

int root(int x){
    while(dsu.get(x) >= 0){
        x = dsu.get(x);
    }
    return x;
}

void unite(int x, int y){
    x = root(x);
    y = root(y);
    if(x == y){
        return;
    }
    if(dsu.get(x) > dsu.get(y)){
        std::swap(x, y);
    }
    dsu.set(x, dsu.get(x) + dsu.get(y));
    dsu.set(y, x);
}

struct VersionEdge{
    int child;
    int u;
    int v;
};

struct ConnectivityQuery{
    int index;
    int u;
    int v;
};

struct DfsFrame{
    int version;
    std::size_t next_child;
    RollbackArray<int, MAX_N, 2 * MAX_N>::Snapshot snapshot;
    bool entered;
};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N, Q;
    std::cin >> N >> Q;

    std::vector<std::vector<VersionEdge>> children(Q + 1);
    std::vector<std::vector<ConnectivityQuery>> queries(Q + 1);
    std::vector<int> type(Q);
    std::vector<int> answer(Q, -1);

    for(int q = 0; q < Q; q++){
        int k, u, v;
        std::cin >> type[q] >> k >> u >> v;
        int base = k + 1;
        if(type[q] == 0){
            children[base].push_back({q + 1, u, v});
        }else{
            queries[base].push_back({q, u, v});
        }
    }

    std::vector<DfsFrame> stack;
    stack.push_back({0, 0, dsu.snapshot(), false});

    while(!stack.empty()){
        DfsFrame& frame = stack.back();

        if(!frame.entered){
            for(const auto& query: queries[frame.version]){
                answer[query.index] = (root(query.u) == root(query.v));
            }
            frame.entered = true;
        }

        if(frame.next_child < children[frame.version].size()){
            VersionEdge edge = children[frame.version][frame.next_child++];
            auto snapshot = dsu.snapshot();
            unite(edge.u, edge.v);
            stack.push_back({edge.child, 0, snapshot, false});
        }else{
            auto snapshot = frame.snapshot;
            stack.pop_back();
            dsu.rollback(snapshot);
        }
    }

    for(int q = 0; q < Q; q++){
        if(type[q] == 1){
            std::cout << answer[q] << '\n';
        }
    }
}
