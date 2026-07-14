// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/persistent_unionfind

#include <iostream>

#include "../../src/structure/dsu/offline_persistent_unionfind_queries.hpp"

constexpr int MAX_N = 200000;
constexpr int MAX_Q = 200000;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size, operation_count;
    std::cin >> size >> operation_count;

    OfflinePersistentUnionFindQueries<MAX_N, MAX_Q, MAX_Q> queries(size);
    while(operation_count-- != 0){
        int type, base_operation, left, right;
        std::cin >> type >> base_operation >> left >> right;
        if(type == 0){
            queries.merge(base_operation, left, right);
        }else{
            queries.same(base_operation, left, right);
        }
    }
    for(const int answer: queries.solve()){
        std::cout << answer << '\n';
    }
}
