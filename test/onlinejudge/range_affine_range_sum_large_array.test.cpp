// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_affine_range_sum_large_array


#include <iostream>
#include <utility>

#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/segtree/dynamic_lazysegtree.hpp"


using namespace std;
using mint=Modint998244353;


using S = mint;
using T = pair<mint,mint>;
using ll = long long;

S op(S a, ll a_len, S b, ll b_len){
    return a+b;
}

S e(){
    return 0;
}

S mapping(T f, S x, ll len){
    return f.first*x+f.second*len;
}

T composition(T f, T g){
    return T{f.first*g.first, f.first*g.second+f.second};
}

T id(){
    return T{1,0};
}




constexpr Monoid_Act_Len<op,e,mapping,composition,id> monoid;

int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N,Q;
    cin>>N>>Q;
    DynamicLazySegtree<monoid,1'000'000'000> seg(N);
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
            cout<<seg.prod(l,r).val()<<"\n";
        }
    }
}
