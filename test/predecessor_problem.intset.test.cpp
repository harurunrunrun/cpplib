// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/predecessor_problem

#include <algorithm>
#include <vector>
#include <utility>
#include <string>

#include "../src/structure/other/fastio.hpp"
#include "../src/structure/tree/integer_set.hpp"

using namespace std;

int main(){
    fastio io;
    int N, Q;
    io.readint(N);
    io.readint(Q);
    char T[10000010];
    io.readstr(T);
    Int_Set<unsigned int, 10000000> IS;
    for(int i=0;i<N;i++){
        if(T[i]=='1'){
            IS.insert(i);
        }
    }
    for(int i=0;i<Q;i++){
        int c,k;
        io.readint(c);
        io.readint(k);
        if(c==0){
            IS.insert(k);
        }else if(c==1){
            IS.erase(k);
        }else if(c==2){
            if(IS.contain(k)){
                io.write('1');
            }else{
                io.write('0');
            }
            io.write('\n');
        }else if(c==3){
            auto val=IS.least(k);
            if(val.has_value()){
                io.writeint(*val);
            }else{
                io.writeint(-1);
            }
            io.write('\n');
        }else{
            auto val=IS.most(k);
            if(val.has_value()){
                io.writeint(*val);
            }else{
                io.writeint(-1);
            }
            io.write('\n');
        }
    }
}
