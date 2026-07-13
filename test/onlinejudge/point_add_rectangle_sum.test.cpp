// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/point_add_rectangle_sum

#include <iostream>
#include <vector>
#include "../../src/algorithm/other/dynamic_point_add_rectangle_sum.hpp"

struct PointRectangleQuery{
    int type;
    long long a, b, c, d;
};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    std::vector<PointRectangleQuery> initial(static_cast<std::size_t>(n));
    DynamicPointAddRectangleSum<long long> structure;
    for(auto& point: initial){
        std::cin >> point.a >> point.b >> point.c;
        point.type = 0;
        point.d = 0;
        structure.reserve_point(point.a, point.b);
    }
    std::vector<PointRectangleQuery> queries;
    queries.reserve(static_cast<std::size_t>(q));
    while(q--){
        PointRectangleQuery query{};
        std::cin >> query.type;
        if(query.type == 0){
            std::cin >> query.a >> query.b >> query.c;
            structure.reserve_point(query.a, query.b);
        }else{
            std::cin >> query.a >> query.b >> query.c >> query.d;
        }
        queries.push_back(query);
    }
    structure.build();
    for(const auto& point: initial) structure.add(point.a, point.b, point.c);
    for(const auto& query: queries){
        if(query.type == 0) structure.add(query.a, query.b, query.c);
        else std::cout << structure.sum(query.a, query.b, query.c, query.d) << '\n';
    }
}
