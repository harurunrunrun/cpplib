// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/unionfind

#include "../../src/structure/dsu/dsu.hpp"
#include "../../src/structure/other/fastio.hpp"

int main(){
    fastio io;
    int n, q;
    io.readint(n);
    io.readint(q);
    DSU<200000> dsu(n);
    while(q--){
        int t, u, v;
        io.readint(t);
        io.readint(u);
        io.readint(v);
        if(t == 0){
            dsu.merge(u, v);
        }else{
            io.write(dsu.same(u, v) ? '1' : '0');
            io.write('\n');
        }
    }
}
