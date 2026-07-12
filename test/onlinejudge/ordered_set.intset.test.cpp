// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/ordered_set

#include <algorithm>
#include <vector>
#include <utility>

#include "../../src/structure/other/fastio.hpp"
#include "../../src/structure/tree/integer_set.hpp"

using namespace std;

int main(){
    fastio io;
    int N, Q;
    io.readint(N);
    io.readint(Q);
    vector<int> A(N), C(N+Q);
    vector<pair<int,int>> B(Q);
    for(int i=0;i<N;i++){
        io.readint(A[i]);
        C[i]=A[i];
    }
    for(int i=0;i<Q;i++){
        io.readint(B[i].first);
        io.readint(B[i].second);
        C[i+N]=B[i].second;
    }
    sort(C.begin(),C.end());
    C.erase(unique(C.begin(),C.end()),C.end());
    Int_Set<unsigned int, 1000000> IS;
    for(int i=0;i<N;i++){
        unsigned int x=lower_bound(C.begin(),C.end(),A[i])-C.begin();
        IS.insert(x);
    }
    for(int i=0;i<Q;i++){
        unsigned int x=lower_bound(C.begin(),C.end(),B[i].second)-C.begin();
        if(B[i].first==0){
            IS.insert(x);
        }else if(B[i].first==1){
            IS.erase(x);
        }else if(B[i].first==2){
            auto val=IS.kth_ge(0u, (unsigned)B[i].second-1);
            if(val.has_value()){
                io.writeint(C[*val]);
            }else{
                io.writeint(-1);
            }
            io.write('\n');
        }else if(B[i].first==3){
            auto ans=IS.range_sum(0u,x+1);
            io.writeint((int)ans);
            io.write('\n');
        }else if(B[i].first==4){
            auto val=IS.most(x);
            if(val.has_value()){
                io.writeint(C[*val]);
            }else{
                io.writeint(-1);
            }
            io.write('\n');
        }else{
            auto val=IS.least(x);
            if(val.has_value()){
                io.writeint(C[*val]);
            }else{
                io.writeint(-1);
            }
            io.write('\n');
        }
    }
}
