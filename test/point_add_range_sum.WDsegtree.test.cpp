// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/point_add_range_sum



#include "../src/structure/other/fastio.hpp"
#include "../src/structure/segtree/widesegtree.hpp"

using ll=long long;
using namespace std;

int main(){
    fastio io;
    int N,Q;
    io.readint(N);
    io.readint(Q);
    Wide_Segtree<500000> seg;
    for(int i=0;i<N;i++){
        int a;
        io.readint(a);
        seg.set(i,a);
    }
    for(int i=0;i<Q;i++){
        int t;
        io.readint(t);
        if(t==0){
            int p,x;
            io.readint(p);
            io.readint(x);
            seg.add(p, x);
        }else{
            int l,r;
            io.readint(l);
            io.readint(r);
            long long ans=seg.prod(l,r);
            io.writell(ans);
            io.write('\n');
        }
    }
}