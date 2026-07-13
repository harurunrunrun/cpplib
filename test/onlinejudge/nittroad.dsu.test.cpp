#define PROBLEM "https://www.spoj.com/problems/NITTROAD/"

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/structure/dsu/dsu.hpp"

namespace{

struct RoadQuery{
    char type;
    int road;
};

}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int vertex_count;
        std::cin >> vertex_count;

        std::vector<std::pair<int, int>> roads;
        roads.reserve(static_cast<std::size_t>(std::max(0, vertex_count - 1)));
        for(int edge = 1; edge < vertex_count; ++edge){
            int left, right;
            std::cin >> left >> right;
            roads.emplace_back(left - 1, right - 1);
        }

        int query_count;
        std::cin >> query_count;
        std::vector<RoadQuery> queries;
        queries.reserve(static_cast<std::size_t>(query_count));
        std::vector<bool> removed(static_cast<std::size_t>(vertex_count - 1), false);
        for(int query = 0; query < query_count; ++query){
            char type;
            std::cin >> type;
            int road = -1;
            if(type == 'R'){
                std::cin >> road;
                --road;
                removed[static_cast<std::size_t>(road)] = true;
            }
            queries.push_back({type, road});
        }

        DSU<20000> dsu(vertex_count);
        long long disconnected =
            static_cast<long long>(vertex_count) * (vertex_count - 1LL) / 2LL;
        const auto restore_road = [&](int road){
            const auto [left, right] = roads[static_cast<std::size_t>(road)];
            if(dsu.same(left, right)){
                return;
            }
            disconnected -= static_cast<long long>(dsu.component_size(left))
                * dsu.component_size(right);
            dsu.merge(left, right);
        };
        for(int road = 0; road + 1 < vertex_count; ++road){
            if(!removed[static_cast<std::size_t>(road)]){
                restore_road(road);
            }
        }

        std::vector<long long> answers;
        for(auto iterator = queries.rbegin(); iterator != queries.rend(); ++iterator){
            if(iterator->type == 'Q'){
                answers.push_back(disconnected);
            }else{
                restore_road(iterator->road);
            }
        }
        std::reverse(answers.begin(), answers.end());
        for(long long answer: answers){
            std::cout << answer << '\n';
        }
        std::cout << '\n';
    }
}
