// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include "../../src/algorithm/other/mo.hpp"

std::vector<int> solve_distinct(const std::vector<int>& a, const std::vector<std::pair<int, int>>& queries){
    Mo mo(static_cast<int>(a.size()));
    for(auto [l, r]: queries) mo.add_query(l, r);
    std::vector<int> count(16, 0);
    int distinct = 0;
    std::vector<int> answer(queries.size());
    auto add = [&](int index){
        if(count[static_cast<std::size_t>(a[static_cast<std::size_t>(index)])]++ == 0) distinct++;
    };
    auto erase = [&](int index){
        if(--count[static_cast<std::size_t>(a[static_cast<std::size_t>(index)])] == 0) distinct--;
    };
    mo.solve(add, erase, [&](int index){
        answer[static_cast<std::size_t>(index)] = distinct;
    });
    return answer;
}

void self_test(){
    {
        std::vector<int> a = {1, 2, 1, 3, 2};
        std::vector<std::pair<int, int>> queries = {{0, 3}, {1, 5}, {2, 2}};
        auto answer = solve_distinct(a, queries);
        assert((answer == std::vector<int>{2, 3, 0}));
    }
    std::mt19937 rng(20260910);
    for(int n = 0; n <= 50; n++){
        std::vector<int> a(static_cast<std::size_t>(n));
        for(int& x: a) x = static_cast<int>(rng() % 16);
        std::vector<std::pair<int, int>> queries;
        for(int q = 0; q < 100; q++){
            int l = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
            int r = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
            if(l > r) std::swap(l, r);
            queries.push_back({l, r});
        }
        auto answer = solve_distinct(a, queries);
        for(int q = 0; q < static_cast<int>(queries.size()); q++){
            std::vector<char> used(16, 0);
            int expected = 0;
            for(int i = queries[static_cast<std::size_t>(q)].first; i < queries[static_cast<std::size_t>(q)].second; i++){
                int x = a[static_cast<std::size_t>(i)];
                if(!used[static_cast<std::size_t>(x)]){
                    used[static_cast<std::size_t>(x)] = 1;
                    expected++;
                }
            }
            assert(answer[static_cast<std::size_t>(q)] == expected);
        }
    }
}

int main(){
    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    std::vector<int> a(static_cast<std::size_t>(n));
    for(int& x: a) std::cin >> x;
    std::vector<std::pair<int, int>> queries(static_cast<std::size_t>(q));
    for(auto& [l, r]: queries) std::cin >> l >> r;
    auto answer = solve_distinct(a, queries);
    for(int x: answer) std::cout << x << '\n';
}
