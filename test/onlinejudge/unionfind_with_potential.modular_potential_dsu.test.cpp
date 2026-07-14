// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/unionfind_with_potential

#include "../../src/structure/dsu/modular_potential_dsu.hpp"
#include "../../src/structure/other/fastio.hpp"

int main(){
    fastio io;
    int n, q;
    io.readint(n);
    io.readint(q);
    ModularPotentialDSU<998244353, 200000> dsu(n);
    while(q--){
        int t, u, v;
        io.readint(t);
        io.readint(u);
        io.readint(v);
        if(t == 0){
            int x;
            io.readint(x);
            io.write(dsu.merge(u, v, x) ? '1' : '0');
            io.write('\n');
        }else{
            const auto difference = dsu.difference(u, v);
            io.writeint(difference ? *difference : -1);
            io.write('\n');
        }
    }
}
