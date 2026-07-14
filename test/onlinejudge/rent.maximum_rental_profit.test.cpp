// competitive-verifier: PROBLEM https://www.spoj.com/problems/RENT/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/maximum_rental_profit.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){ int n; std::cin>>n; std::vector<RentalOffer> offers(n);
        for(auto& offer:offers) std::cin>>offer.start>>offer.duration>>offer.profit;
        std::cout<<maximum_rental_profit(std::move(offers))<<'\n'; }
}
