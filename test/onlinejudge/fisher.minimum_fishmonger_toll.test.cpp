// competitive-verifier: PROBLEM https://www.spoj.com/problems/FISHER/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/minimum_fishmonger_toll.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n,time; while(std::cin>>n>>time && (n||time)){ std::vector<std::vector<int>> travel(n,std::vector<int>(n));
        std::vector<std::vector<int>> toll(n,std::vector<int>(n));
        for(auto& row:travel) for(int& x:row) std::cin>>x;
        for(auto& row:toll) for(int& x:row) std::cin>>x;
        auto answer=minimum_fishmonger_toll(travel,toll,time);
        std::cout<<answer.toll<<' '<<answer.travel_time<<'\n'; }
}
