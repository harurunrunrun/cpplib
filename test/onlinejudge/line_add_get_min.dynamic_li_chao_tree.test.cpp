// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/line_add_get_min

#include <iostream>
#include "../../src/structure/convex_hull_trick/dynamic_li_chao_tree.hpp"

constexpr long long X_MIN = -1000000000LL;
constexpr long long X_MAX = 1000000000LL;
constexpr int MAX_LINE = 400000;

DynamicLiChaoTree<X_MIN, X_MAX, MAX_LINE> cht;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N, Q;
    std::cin >> N >> Q;

    for(int i = 0; i < N; i++){
        long long a, b;
        std::cin >> a >> b;
        cht.add_line(a, b);
    }

    for(int q = 0; q < Q; q++){
        int type;
        std::cin >> type;
        if(type == 0){
            long long a, b;
            std::cin >> a >> b;
            cht.add_line(a, b);
        }else{
            long long p;
            std::cin >> p;
            std::cout << cht.query(p) << '\n';
        }
    }
}
