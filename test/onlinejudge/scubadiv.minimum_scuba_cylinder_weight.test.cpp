// competitive-verifier: PROBLEM https://www.spoj.com/problems/SCUBADIV/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/minimum_cost_two_resource_cover.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){
        int required_first,required_second,n;
        std::cin>>required_first>>required_second>>n;
        std::vector<TwoResourceItem> items(n);
        for(auto& item:items){
            std::cin>>item.first_resource>>item.second_resource>>item.cost;
        }
        std::cout<<minimum_cost_two_resource_cover(
            required_first,required_second,items
        )<<'\n';
    }
}
