// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/persistent_queue

#include <array>
#include <iostream>
#include "../../src/structure/array/persistent_array.hpp"

constexpr int MAX_Q = 500000;

PersistentArray<int, MAX_Q, MAX_Q> values(MAX_Q, 0);
std::array<int, MAX_Q + 1> roots{};
std::array<int, MAX_Q + 1> left_index{};
std::array<int, MAX_Q + 1> right_index{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int Q;
    std::cin >> Q;

    for(int q = 0; q < Q; q++){
        int type, t;
        std::cin >> type >> t;
        int base = t + 1;

        left_index[q + 1] = left_index[base];
        right_index[q + 1] = right_index[base];

        if(type == 0){
            int x;
            std::cin >> x;
            roots[q + 1] = values.set(roots[base], right_index[base], x);
            right_index[q + 1]++;
        }else{
            std::cout << values.get(roots[base], left_index[base]) << '\n';
            roots[q + 1] = values.fork(roots[base]);
            left_index[q + 1]++;
        }
    }
}
