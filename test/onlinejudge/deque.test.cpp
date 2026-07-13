// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/deque

#include <iostream>
#include <memory>

#include "../../src/structure/tree/implicit_treap.hpp"
#include "../../src/structure/types/common_monoids.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    std::cin >> query_count;
    auto sequence = std::make_unique<
        ImplicitTreap<AddMonoid<long long>{}, 500000>
    >();
    while(query_count--){
        int type;
        std::cin >> type;
        if(type == 0){
            long long value;
            std::cin >> value;
            sequence->push_front(value);
        }else if(type == 1){
            long long value;
            std::cin >> value;
            sequence->push_back(value);
        }else if(type == 2){
            sequence->erase(0);
        }else if(type == 3){
            sequence->erase(sequence->size() - 1);
        }else{
            int index;
            std::cin >> index;
            std::cout << sequence->get(index) << '\n';
        }
    }
}
