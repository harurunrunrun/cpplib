// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/all_furthest_neighbors
#include <iostream>
#include <utility>
#include <vector>
#include "../../src/algorithm/geometry/2d/index_set/point_set/integer_convex_farthest_neighbors.hpp"
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int t;std::cin>>t;
    while(t--){
        int n;std::cin>>n;
        std::vector<std::pair<long long,long long>>points(n);
        for(auto& [x,y]:points)std::cin>>x>>y;
        const auto answer=integer_convex_farthest_neighbors(points);
        for(int i=0;i<n;++i)std::cout<<answer[i]<<(i+1==n?'\n':' ');
    }
}
