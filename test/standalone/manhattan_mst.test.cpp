// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <tuple>
#include <vector>
#include "../../src/algorithm/graph/manhattan_mst.hpp"

struct Dsu{
    std::vector<int> p;
    explicit Dsu(int n): p(static_cast<std::size_t>(n)){
        std::iota(p.begin(), p.end(), 0);
    }
    int leader(int x){
        while(p[static_cast<std::size_t>(x)] != x){
            x = p[static_cast<std::size_t>(x)] = p[static_cast<std::size_t>(p[static_cast<std::size_t>(x)])];
        }
        return x;
    }
    bool merge(int a, int b){
        a = leader(a);
        b = leader(b);
        if(a == b) return false;
        p[static_cast<std::size_t>(b)] = a;
        return true;
    }
};

long long brute(const std::vector<ManhattanMstPoint<long long>>& points){
    const int n = static_cast<int>(points.size());
    std::vector<std::tuple<long long, int, int>> edges;
    for(int i = 0; i < n; i++){
        for(int j = i + 1; j < n; j++){
            long long cost = std::abs(points[static_cast<std::size_t>(i)].x - points[static_cast<std::size_t>(j)].x) +
                             std::abs(points[static_cast<std::size_t>(i)].y - points[static_cast<std::size_t>(j)].y);
            edges.push_back({cost, i, j});
        }
    }
    std::sort(edges.begin(), edges.end());
    Dsu dsu(n);
    long long result = 0;
    for(auto [cost, u, v]: edges){
        if(dsu.merge(u, v)) result += cost;
    }
    return result;
}

void self_test(){
    {
        std::vector<ManhattanMstPoint<long long>> points = {{0, 0}, {1, 2}, {3, 1}, {-1, 4}};
        auto res = manhattan_mst<long long>(points);
        assert(res.cost == brute(points));
        assert(res.edges.size() == points.size() - 1);
    }
    std::mt19937 rng(20260817);
    for(int n = 0; n <= 60; n++){
        for(int step = 0; step < 50; step++){
            std::vector<ManhattanMstPoint<long long>> points;
            for(int i = 0; i < n; i++){
                points.push_back({
                    static_cast<int>(rng() % 101) - 50,
                    static_cast<int>(rng() % 101) - 50,
                });
            }
            auto res = manhattan_mst<long long>(points);
            assert(res.cost == brute(points));
            assert(res.edges.size() == (n == 0 ? 0 : static_cast<std::size_t>(n - 1)));
        }
    }
}

int main(){
    int n;
    if(!(std::cin >> n)){
        self_test();
        return 0;
    }
    std::vector<ManhattanMstPoint<long long>> points(static_cast<std::size_t>(n));
    for(int i = 0; i < n; i++){
        std::cin >> points[static_cast<std::size_t>(i)].x >> points[static_cast<std::size_t>(i)].y;
    }
    auto res = manhattan_mst<long long>(points);
    std::cout << res.cost << '\n';
}
