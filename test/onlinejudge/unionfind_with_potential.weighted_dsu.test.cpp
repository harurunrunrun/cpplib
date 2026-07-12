// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/unionfind_with_potential

#include "../../src/structure/dsu/weighted_dsu.hpp"
#include "../../src/structure/other/fastio.hpp"

struct Mint{
    static constexpr int mod = 998244353;
    int v;

    Mint(long long x = 0): v(static_cast<int>(x % mod)){
        if(v < 0) v += mod;
    }
    Mint& operator+=(const Mint& rhs){
        v += rhs.v;
        if(v >= mod) v -= mod;
        return *this;
    }
    Mint& operator-=(const Mint& rhs){
        v -= rhs.v;
        if(v < 0) v += mod;
        return *this;
    }
    friend Mint operator+(Mint lhs, const Mint& rhs){ return lhs += rhs; }
    friend Mint operator-(Mint lhs, const Mint& rhs){ return lhs -= rhs; }
    friend bool operator==(const Mint& lhs, const Mint& rhs){ return lhs.v == rhs.v; }
    friend bool operator!=(const Mint& lhs, const Mint& rhs){ return !(lhs == rhs); }
};

int main(){
    fastio io;
    int n, q;
    io.readint(n);
    io.readint(q);
    WeightedDSU<Mint, 200000> dsu(n);
    while(q--){
        int t, u, v;
        io.readint(t);
        io.readint(u);
        io.readint(v);
        if(t == 0){
            int x;
            io.readint(x);
            io.write(dsu.merge(v, u, Mint(x)) ? '1' : '0');
            io.write('\n');
        }else{
            if(!dsu.same(u, v)){
                io.writeint(-1);
            }else{
                io.writeint(dsu.diff(v, u).v);
            }
            io.write('\n');
        }
    }
}
