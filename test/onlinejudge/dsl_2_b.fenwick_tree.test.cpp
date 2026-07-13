#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/DSL_2_B"

#include <iostream>

#include "../../src/structure/other/fenwick_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int size, query_count;
    std::cin >> size >> query_count;
    static FenwickTree<long long, 1000000> fenwick(size);
    while(query_count--){
        int type, x;
        long long y;
        std::cin >> type >> x >> y;
        if(type == 0) fenwick.add(x - 1, y);
        else std::cout << fenwick.sum(x - 1, static_cast<int>(y)) << '\n';
    }
}
