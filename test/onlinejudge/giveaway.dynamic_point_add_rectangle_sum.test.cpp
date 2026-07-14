// competitive-verifier: PROBLEM https://www.spoj.com/problems/GIVEAWAY/

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/dynamic_point_add_rectangle_sum.hpp"

namespace{

struct Query{
    int type;
    int left;
    int right;
    int value;
};

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;

    int query_count;
    std::cin >> query_count;
    std::vector<Query> queries;
    queries.reserve(static_cast<std::size_t>(query_count));

    DynamicPointAddRectangleSum<int> points;
    for(int index = 0; index < size; ++index){
        points.reserve_point(index, values[static_cast<std::size_t>(index)]);
    }
    for(int query_index = 0; query_index < query_count; ++query_index){
        int type, first, second;
        std::cin >> type >> first >> second;
        --first;
        if(type == 0){
            int threshold;
            std::cin >> threshold;
            queries.push_back({type, first, second, threshold});
        }else{
            queries.push_back({type, first, 0, second});
            points.reserve_point(first, second);
        }
    }

    points.build();
    for(int index = 0; index < size; ++index){
        points.add(index, values[static_cast<std::size_t>(index)], 1);
    }
    for(const Query& query: queries){
        if(query.type == 0){
            const int below_threshold =
                points.prefix_sum_less(query.right, query.value)
                - points.prefix_sum_less(query.left, query.value);
            std::cout << query.right - query.left - below_threshold << '\n';
        }else{
            int& current = values[static_cast<std::size_t>(query.left)];
            points.add(query.left, current, -1);
            current = query.value;
            points.add(query.left, current, 1);
        }
    }
}
