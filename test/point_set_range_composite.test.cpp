// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/point_set_range_composite


#include <iostream>
#include <utility>
#include "../src/structure/modint/modint.hpp"

#include "../src/structure/segtree/segtree.hpp"


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
    vector<S> init(N);
    for(int i=0;i<N;i++){
        int a,b;
        cin>>a>>b;
        init[i]={a,b};
    }

    Segtree<monoid,500000> seg(init);
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