// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/ordered_set

#include <vector>
#include <utility>

#include "../../src/structure/other/fastio.hpp"
#include "../../src/structure/tree/compressed_ordered_set.hpp"

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
    CompressedOrderedSet<int, 1000000> set(C, A);
    for(int i=0;i<Q;i++){
        if(B[i].first==0){
            set.insert(B[i].second);
        }else if(B[i].first==1){
            set.erase(B[i].second);
        }else if(B[i].first==2){
            auto val=set.kth_smallest_one_based(B[i].second);
            if(val.has_value()){
                io.writeint(*val);
            }else{
                io.writeint(-1);
            }
            io.write('\n');
        }else if(B[i].first==3){
            io.writeint(set.count_less_equal(B[i].second));
            io.write('\n');
        }else if(B[i].first==4){
            auto val=set.predecessor_or_equal(B[i].second);
            if(val.has_value()){
                io.writeint(*val);
            }else{
                io.writeint(-1);
            }
            io.write('\n');
        }else{
            auto val=set.successor_or_equal(B[i].second);
            if(val.has_value()){
                io.writeint(*val);
            }else{
                io.writeint(-1);
            }
            io.write('\n');
        }
    }
}
