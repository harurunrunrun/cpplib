// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/rectangle_add_point_get

#include <iostream>
#include <vector>
#include "../../src/algorithm/other/dynamic_rectangle_add_point_get.hpp"

struct RectanglePointQuery{
    int type;
    long long a, b, c, d, value;
};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    DynamicRectangleAddPointGet<long long> structure;
    std::vector<RectanglePointQuery> initial(static_cast<std::size_t>(n));
    for(auto& rectangle: initial){
        rectangle.type = 0;
        std::cin >> rectangle.a >> rectangle.b >> rectangle.c >> rectangle.d >> rectangle.value;
        structure.reserve_rectangle(rectangle.a, rectangle.b, rectangle.c, rectangle.d);
    }
    std::vector<RectanglePointQuery> queries;
    queries.reserve(static_cast<std::size_t>(q));
    while(q--){
        RectanglePointQuery query{};
        std::cin >> query.type;
        if(query.type == 0){
            std::cin >> query.a >> query.b >> query.c >> query.d >> query.value;
            structure.reserve_rectangle(query.a, query.b, query.c, query.d);
        }else{
            std::cin >> query.a >> query.b;
        }
        queries.push_back(query);
    }
    structure.build();
    for(const auto& rectangle: initial){
        structure.add(rectangle.a, rectangle.b, rectangle.c, rectangle.d, rectangle.value);
    }
    for(const auto& query: queries){
        if(query.type == 0) structure.add(query.a, query.b, query.c, query.d, query.value);
        else std::cout << structure.get(query.a, query.b) << '\n';
    }
}
