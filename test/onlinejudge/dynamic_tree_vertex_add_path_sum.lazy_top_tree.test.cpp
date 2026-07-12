// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/dynamic_tree_vertex_add_path_sum

#include <iostream>
#include <vector>
#include "../../src/structure/tree/lazy_top_tree.hpp"
#include "../../src/structure/types/monoid_act_len.hpp"

long long op(long long a, long long, long long b, long long){ return a + b; }
long long e(){ return 0; }
long long mapping(long long f, long long x, long long len){ return x + f * len; }
long long composition(long long f, long long g){ return f + g; }
long long id(){ return 0; }
constexpr Monoid_Act_Len<op, e, mapping, composition, id> act;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for(auto& x: a) std::cin >> x;
    LazyTopTree<act, 200000> tree(a);
    for(int i = 0; i < n - 1; i++){
        int u, v;
        std::cin >> u >> v;
        tree.link(u, v);
    }
    while(q--){
        int type;
        std::cin >> type;
        if(type == 0){
            int u, v, w, x;
            std::cin >> u >> v >> w >> x;
            tree.cut(u, v);
            tree.link(w, x);
        }else if(type == 1){
            int p;
            long long x;
            std::cin >> p >> x;
            tree.path_apply(p, p, x);
        }else{
            int u, v;
            std::cin >> u >> v;
            std::cout << tree.path_prod(u, v) << '\n';
        }
    }
}
