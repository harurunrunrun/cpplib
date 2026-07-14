// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_affine_point_get


#include <iostream>
#include <vector>



#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/segtree/range_affine_point_get.hpp"


using namespace std;
using mint=Modint998244353;
int main(){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N,Q;
    cin>>N>>Q;
    vector<mint> A(N);
    for(int i=0;i<N;i++){
        int a;
        cin>>a;
        A[i]=a;
    }

    RangeAffinePointGet<mint,500000> queries(A);
    for(int i=0;i<Q;i++){
        int t;
        cin>>t;
        if(t==0){
            int l,r,b,c;
            cin>>l>>r>>b>>c;
            queries.apply(l,r,b,c);
        }else{
            int idx;
            cin>>idx;
            cout<<queries.get(idx).val()<<"\n";
        }
    }
}
