// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_affine_point_get


#include <iostream>
#include <utility>



#include "../src/structure/modint/modint.hpp"
#include "../src/structure/segtree/dualsegtree.hpp"


using namespace std;
using mint=Modint998244353;


using S = mint;
using T = pair<mint,mint>;

S op(S a, S b){
    return 0;
}

S e(){
    return 0;
}

S mapping(T f, S x){
    return f.first*x+f.second;
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
    vector<S> A(N);
    for(int i=0;i<N;i++){
        int a;
        cin>>a;
        A[i]=a;
    }

    DualSegtree<monoid,500000> seg(A);
    for(int i=0;i<Q;i++){
        int t;
        cin>>t;
        if(t==0){
            int l,r,b,c;
            cin>>l>>r>>b>>c;
            seg.apply(l,r,T{b,c});
        }else{
            int idx;
            cin>>idx;
            cout<<seg.get(idx).val()<<"\n";
        }
    }
}