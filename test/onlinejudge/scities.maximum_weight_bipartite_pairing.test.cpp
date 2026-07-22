// competitive-verifier: PROBLEM https://www.spoj.com/problems/SCITIES/

#include <iostream>
#include <vector>

#include "../../src/algorithm/matching/bipartite/maximum_weight_bipartite_pairing.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int seller_count, buyer_count;
        std::cin >> seller_count >> buyer_count;
        std::vector<MaximumWeightBipartitePairingEdge> offers;
        while(true){
            int seller, buyer;
            long long goods;
            std::cin >> seller >> buyer >> goods;
            if(seller == 0 && buyer == 0 && goods == 0) break;
            offers.push_back({seller - 1, buyer - 1, goods});
        }
        std::cout << maximum_weight_bipartite_pairing(
            seller_count, buyer_count, offers
        ) << '\n';
    }
}
