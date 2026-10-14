// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/point_set_range_frequency

#include <iostream>
#include <utility>
#include <vector>
#include "../../src/structure/tree/treap/treap.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<int> values(n);
    TreapSet<std::pair<int, int>, 500'000> index;
    for(int i = 0; i < n; ++i){ std::cin >> values[i]; index.insert({values[i], i}); }
    while(q--){
        int type, l, r; std::cin >> type >> l >> r;
        if(type == 0){ index.erase({values[l], l}); values[l] = r; index.insert({values[l], l}); }
        else{ int x; std::cin >> x; std::cout << index.order_of_key({x, r}) - index.order_of_key({x, l}) << '\n'; }
    }
}
