// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/point_set_range_composite_large_array


#include <iostream>
#include <utility>

#include "../src/structure/modint/modint.hpp"
#include "../src/structure/segtree/dynamic_segtree.hpp"


using namespace std;
using mint=Modint998244353;


using S = pair<mint,mint>;

S op(S a, S b){
    return {a.first*b.first, a.second*b.first+b.second};
}

S e(){
    return S{1,0};
}


constexpr Monoid<op,e> monoid{};

int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N,Q;
    cin>>N>>Q;

    DynamicSegtree<monoid,1000000000> seg;
    for(int i=0;i<Q;i++){
        int t;
        cin>>t;
        if(t==0){
            int p,c,d;
            cin>>p>>c>>d;
            seg.set(p,S{c,d});
        }else{
            int l,r,x;
            cin>>l>>r>>x;
            auto [a,b]=seg.prod(l,r);
            mint ans=a*x+b;
            cout<<ans.val()<<"\n";
        }
    }
}