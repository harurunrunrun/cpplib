// competitive-verifier: PROBLEM https://www.spoj.com/problems/PARTY/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/maximum_party_fun.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int budget,n; while(std::cin>>budget>>n && (budget||n)){ std::vector<PartyOption> parties(n);
        for(auto& p:parties) std::cin>>p.fee>>p.fun;
        auto answer=maximum_party_fun(budget,parties);
        std::cout<<answer.minimum_fee<<' '<<answer.maximum_fun<<'\n'; }
}
