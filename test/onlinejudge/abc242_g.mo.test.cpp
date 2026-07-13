// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc242/tasks/abc242_g

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/mo.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<int> values(static_cast<std::size_t>(n));
    for(int& value: values) std::cin >> value;
    int q;
    std::cin >> q;
    Mo mo(n);
    for(int i = 0; i < q; ++i){
        int left, right;
        std::cin >> left >> right;
        mo.add_query(left - 1, right);
    }

    std::vector<int> frequency(static_cast<std::size_t>(n + 1), 0);
    std::vector<int> answer(static_cast<std::size_t>(q));
    int pair_count = 0;
    const auto add = [&](int index){
        int& count = frequency[static_cast<std::size_t>(values[static_cast<std::size_t>(index)])];
        if(count & 1) ++pair_count;
        ++count;
    };
    const auto erase = [&](int index){
        int& count = frequency[static_cast<std::size_t>(values[static_cast<std::size_t>(index)])];
        if((count & 1) == 0) --pair_count;
        --count;
    };
    mo.solve(add, erase, [&](int query){
        answer[static_cast<std::size_t>(query)] = pair_count;
    });
    for(int value: answer) std::cout << value << '\n';
}
