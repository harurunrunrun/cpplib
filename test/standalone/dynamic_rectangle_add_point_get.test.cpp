// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/algorithm/other/offline_query/dynamic_rectangle_add_point_get.hpp"

struct Query{
    std::string type;
    int x1;
    int y1;
    int x2;
    int y2;
    long long value;
};

std::vector<long long> solve(const std::vector<Query>& queries){
    DynamicRectangleAddPointGet<long long, int> seg;
    for(const auto& query: queries){
        if(query.type == "ADD") seg.reserve_rectangle(query.x1, query.y1, query.x2, query.y2);
    }
    seg.build();
    std::vector<long long> answer;
    for(const auto& query: queries){
        if(query.type == "ADD"){
            seg.add(query.x1, query.y1, query.x2, query.y2, query.value);
        }else{
            answer.push_back(seg.get(query.x1, query.y1));
        }
    }
    return answer;
}

void self_test(){
    {
        DynamicRectangleAddPointGet<long long, int> structure;
        bool thrown = false;
        try{
            (void)structure.get(0, 0);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        thrown = false;
        try{
            structure.add(0, 0, 1, 1, 3);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);

        structure.reserve_rectangle(4, 5, 1, 2);
        structure.build();
        structure.add(4, 5, 1, 2, 9);
        assert(structure.get(1, 2) == 9);
        assert(structure.get(3, 4) == 9);
        assert(structure.get(4, 4) == 0);
        assert(structure.get(3, 5) == 0);

        thrown = false;
        try{
            structure.reserve_rectangle(0, 0, 2, 2);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        thrown = false;
        try{
            structure.build();
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    {
        std::vector<Query> queries = {
            {"ADD", 0, 0, 3, 3, 5},
            {"GET", 1, 1, 0, 0, 0},
            {"ADD", 1, 1, 4, 4, -2},
            {"GET", 2, 2, 0, 0, 0},
            {"GET", 3, 3, 0, 0, 0},
        };
        auto answer = solve(queries);
        assert((answer == std::vector<long long>{5, 3, -2}));
    }
    std::mt19937 rng(20260914);
    for(int step = 0; step < 300; step++){
        std::vector<Query> queries;
        long long naive[20][20] = {};
        std::vector<long long> expected;
        for(int q = 0; q < 80; q++){
            if(rng() % 2 == 0){
                int x1 = static_cast<int>(rng() % 20);
                int x2 = static_cast<int>(rng() % 20);
                int y1 = static_cast<int>(rng() % 20);
                int y2 = static_cast<int>(rng() % 20);
                if(x1 > x2) std::swap(x1, x2);
                if(y1 > y2) std::swap(y1, y2);
                long long value = static_cast<int>(rng() % 21) - 10;
                queries.push_back({"ADD", x1, y1, x2, y2, value});
                for(int x = x1; x < x2; x++){
                    for(int y = y1; y < y2; y++){
                        naive[x][y] += value;
                    }
                }
            }else{
                int x = static_cast<int>(rng() % 20);
                int y = static_cast<int>(rng() % 20);
                queries.push_back({"GET", x, y, 0, 0, 0});
                expected.push_back(naive[x][y]);
            }
        }
        assert(solve(queries) == expected);
    }
}

int main(){
    int q;
    if(!(std::cin >> q)){
        self_test();
        return 0;
    }
    std::vector<Query> queries(static_cast<std::size_t>(q));
    for(auto& query: queries){
        std::cin >> query.type;
        if(query.type == "ADD"){
            std::cin >> query.x1 >> query.y1 >> query.x2 >> query.y2 >> query.value;
        }else{
            std::cin >> query.x1 >> query.y1;
        }
    }
    auto answer = solve(queries);
    for(long long value: answer) std::cout << value << '\n';
}
