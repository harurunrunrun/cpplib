// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>

#include "../../src/structure/dsu/modular_potential_dsu.hpp"

void self_test(){
    constexpr int large_mod = 2147483647;
    ModularPotentialDSU<large_mod, 16> dsu(4);
    assert(dsu.merge(0, 1, large_mod - 1));
    assert(dsu.merge(1, 2, large_mod - 1));
    assert(dsu.difference(0, 2) == large_mod - 2);
    assert(dsu.difference(2, 0) == 2);
    assert(!dsu.difference(0, 3));
    assert(dsu.merge(0, 2, -2));
    assert(!dsu.merge(0, 2, -1));
}

template<int MOD>
void run_queries(int n, int query_count){
    ModularPotentialDSU<MOD, 256> dsu(n);
    while(query_count--){
        std::string type;
        int u, v;
        std::cin >> type >> u >> v;
        if(type == "M"){
            long long difference;
            std::cin >> difference;
            std::cout << dsu.merge(u, v, difference) << '\n';
        }else{
            const auto answer = dsu.difference(u, v);
            if(answer) std::cout << *answer << '\n';
            else std::cout << "NA\n";
        }
    }
}

int main(){
    int mode, n, query_count;
    if(!(std::cin >> mode >> n >> query_count)){
        self_test();
        return 0;
    }
    if(mode == 0){
        run_queries<998244353>(n, query_count);
    }else{
        run_queries<2147483647>(n, query_count);
    }
}
