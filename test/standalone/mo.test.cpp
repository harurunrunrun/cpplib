// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>
#include "../../src/algorithm/other/offline_query/mo.hpp"

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
        Mo mo(5);
        assert(mo.size() == 0);
        assert(mo.add_query(1, 1) == 0);
        assert(mo.add_query(0, 4) == 1);
        assert(mo.add_query(2, 5) == 2);
        assert(mo.add_query(3, 3) == 3);
        assert(mo.size() == 4);

        const std::vector<int> values = {2, 3, 5, 7, 11};
        const std::vector<int> expected = {0, 17, 23, 0};
        std::vector<int> answer(4, -1);
        int current = 0;
        int add_left_calls = 0;
        int add_right_calls = 0;
        int erase_left_calls = 0;
        int erase_right_calls = 0;
        mo.solve(
            [&](int index){ current += values[static_cast<std::size_t>(index)]; ++add_left_calls; },
            [&](int index){ current += values[static_cast<std::size_t>(index)]; ++add_right_calls; },
            [&](int index){ current -= values[static_cast<std::size_t>(index)]; ++erase_left_calls; },
            [&](int index){ current -= values[static_cast<std::size_t>(index)]; ++erase_right_calls; },
            [&](int index){ answer[static_cast<std::size_t>(index)] = current; },
            2
        );
        assert(answer == expected);
        assert(add_left_calls > 0);
        assert(add_right_calls > 0);
        assert(erase_left_calls > 0);
        assert(erase_right_calls > 0);
    }
    {
        bool thrown = false;
        try{
            [[maybe_unused]] Mo invalid(-1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        Mo mo(3);
        thrown = false;
        try{
            (void)mo.add_query(-1, 2);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        thrown = false;
        try{
            (void)mo.add_query(1, 4);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
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
