// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <tuple>
#include <vector>
#include "../../src/algorithm/other/offline_query/dynamic_point_add_rectangle_sum.hpp"

struct Query{
    std::string type;
    int x1;
    int y1;
    int x2;
    int y2;
    long long value;
};

std::vector<long long> solve(int n, const std::vector<std::tuple<int, int, long long>>& initial, const std::vector<Query>& queries){
    (void)n;
    DynamicPointAddRectangleSum<long long, int> seg;
    for(auto [x, y, value]: initial){
        (void)value;
        seg.reserve_point(x, y);
    }
    for(const auto& query: queries){
        if(query.type == "ADD") seg.reserve_point(query.x1, query.y1);
    }
    seg.build();
    for(auto [x, y, value]: initial){
        seg.add(x, y, value);
    }
    std::vector<long long> answer;
    for(const auto& query: queries){
        if(query.type == "ADD"){
            seg.add(query.x1, query.y1, query.value);
        }else{
            answer.push_back(seg.sum(query.x1, query.y1, query.x2, query.y2));
        }
    }
    return answer;
}

void self_test(){
    {
        DynamicPointAddRectangleSum<long long, int> seg;
        bool thrown = false;
        try{
            (void)seg.prefix_sum_less(0, 0);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);

        seg.reserve_point(-2, 4);
        seg.reserve_point(1, -3);
        seg.reserve_point(1, 5);
        seg.build();
        seg.add(-2, 4, 7);
        seg.add(1, -3, 11);
        seg.add(1, 5, 13);
        assert(seg.prefix_sum_less(1, 5) == 7);
        assert(seg.prefix_sum_less(2, 6) == 31);
        assert(seg.prefix_sum_leq(1, 4) == 18);
        assert(seg.prefix_sum_leq(-2, 3) == 0);
        assert(seg.sum(-2, -3, 2, 5) == 18);

        thrown = false;
        try{
            seg.add(1, 4, 1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        thrown = false;
        try{
            seg.reserve_point(0, 0);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        thrown = false;
        try{
            seg.build();
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    {
        std::vector<std::tuple<int, int, long long>> initial = {
            {1, 2, 5},
            {3, 4, 7},
        };
        std::vector<Query> queries = {
            {"SUM", 0, 0, 5, 5, 0},
            {"ADD", 1, 2, 0, 0, -2},
            {"SUM", 0, 0, 2, 3, 0},
        };
        auto answer = solve(5, initial, queries);
        assert((answer == std::vector<long long>{12, 3}));
    }
    std::mt19937 rng(20260913);
    for(int step = 0; step < 300; step++){
        std::vector<std::tuple<int, int, long long>> initial;
        std::vector<Query> queries;
        std::map<std::pair<int, int>, long long> naive;
        int initial_count = static_cast<int>(rng() % 8);
        for(int i = 0; i < initial_count; i++){
            int x = static_cast<int>(rng() % 20) - 10;
            int y = static_cast<int>(rng() % 20) - 10;
            long long value = static_cast<int>(rng() % 21) - 10;
            initial.push_back({x, y, value});
            naive[{x, y}] += value;
        }
        std::vector<long long> expected;
        for(int q = 0; q < 80; q++){
            if(rng() % 2 == 0){
                int x = static_cast<int>(rng() % 20) - 10;
                int y = static_cast<int>(rng() % 20) - 10;
                long long value = static_cast<int>(rng() % 21) - 10;
                queries.push_back({"ADD", x, y, 0, 0, value});
                naive[{x, y}] += value;
            }else{
                int x1 = static_cast<int>(rng() % 20) - 10;
                int x2 = static_cast<int>(rng() % 20) - 10;
                int y1 = static_cast<int>(rng() % 20) - 10;
                int y2 = static_cast<int>(rng() % 20) - 10;
                if(x1 > x2) std::swap(x1, x2);
                if(y1 > y2) std::swap(y1, y2);
                long long sum = 0;
                for(auto [point, value]: naive){
                    auto [x, y] = point;
                    if(x1 <= x && x < x2 && y1 <= y && y < y2) sum += value;
                }
                queries.push_back({"SUM", x1, y1, x2, y2, 0});
                expected.push_back(sum);
            }
        }
        assert(solve(20, initial, queries) == expected);
    }
}

int main(){
    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    std::vector<std::tuple<int, int, long long>> initial(static_cast<std::size_t>(n));
    for(auto& [x, y, value]: initial) std::cin >> x >> y >> value;
    std::vector<Query> queries(static_cast<std::size_t>(q));
    for(auto& query: queries){
        std::cin >> query.type;
        if(query.type == "ADD"){
            std::cin >> query.x1 >> query.y1 >> query.value;
        }else{
            std::cin >> query.x1 >> query.y1 >> query.x2 >> query.y2;
        }
    }
    auto answer = solve(n, initial, queries);
    for(long long value: answer) std::cout << value << '\n';
}
