// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/majority_voting

#include <iostream>
#include <vector>

#include "../../src/algorithm/range/range_majority_query.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size, query_count;
    std::cin >> size >> query_count;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;
    RangeMajorityQuery<int> query(std::move(values));

    while(query_count--){
        int type, first, second;
        std::cin >> type >> first >> second;
        if(type == 0){
            query.set(first, second);
        }else{
            const auto result = query.majority(first, second);
            std::cout << (result ? *result : -1) << '\n';
        }
    }
}
