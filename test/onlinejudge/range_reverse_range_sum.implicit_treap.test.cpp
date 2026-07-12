// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_reverse_range_sum

#include <vector>
#include "../../src/structure/other/fastio.hpp"
#include "../../src/structure/tree/implicit_treap.hpp"
#include "../../src/structure/types/monoid.hpp"

long long op(long long a, long long b){ return a + b; }
long long e(){ return 0; }
constexpr Monoid<op, e> monoid;

int main(){
    fastio io;
    int n, q;
    io.readint(n);
    io.readint(q);
    std::vector<long long> a(n);
    for(auto& x: a) io.readll(x);
    ImplicitTreap<monoid, 400000> tree(a);
    while(q--){
        int t, l, r;
        io.readint(t);
        io.readint(l);
        io.readint(r);
        if(t == 0){
            tree.reverse(l, r);
        }else{
            io.writell(tree.prod(l, r));
            io.write('\n');
        }
    }
}
