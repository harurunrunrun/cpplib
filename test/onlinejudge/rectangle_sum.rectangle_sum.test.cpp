// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/rectangle_sum

#include <iostream>
#include <tuple>
#include <vector>
#include "../../src/structure/wavelet_matrix/rectangle_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    std::cin >> n >> q;
    std::vector<std::tuple<int, int, long long>> points;
    points.reserve(n);
    for(int i = 0; i < n; i++){
        int x, y;
        long long w;
        std::cin >> x >> y >> w;
        points.emplace_back(x, y, w);
    }
    RectangleSum<int, int, long long, 200000> rectangle_sum(points);
    while(q--){
        int l, d, r, u;
        std::cin >> l >> d >> r >> u;
        std::cout << rectangle_sum.rectangle_sum(l, r, d, u) << '\n';
    }
}
