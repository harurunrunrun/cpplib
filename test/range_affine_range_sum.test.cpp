// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_affine_range_sum


#include <iostream>
#include <utility>

#include "../src/structure/modint/modint.hpp"
#include "../src/structure/segtree/lazysegtree.hpp"


using namespace std;
using mint=Modint998244353;


using S = pair<mint,mint>;
using T = pair<mint,mint>;

S op(S a, S b){
    return {a.first+b.first, a.second+b.second};
}

S e(){
    return S{0,0};
}

S mapping(T f, S x){
    return S{f.first*x.first+f.second*x.second, x.second};
}

T composition(T f, T g){
    return T{f.first*g.first, f.first*g.second+f.second};
}

T id(){
    return T{1,0};
}




constexpr Monoid_Act<op,e,mapping,composition,id> monoid;

int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N,Q;
    cin>>N>>Q;
    vector<S> init(N);
    for(int i=0;i<N;i++){
        cin>>init[i].first;
        init[i].second=1;
    }

    LazySegtree<monoid,500000> seg(init);
    for(int i=0;i<Q;i++){
        int t;
        cin>>t;
        if(t==0){
            int l,r,b,c;
            cin>>l>>r>>b>>c;
            seg.apply(l,r,T{b,c});
        }else{
            int l,r;
            cin>>l>>r;
            cout<<seg.prod(l,r).first<<"\n";
        }
    }
}