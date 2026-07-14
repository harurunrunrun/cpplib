// competitive-verifier: PROBLEM https://open.kattis.com/problems/cats

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/can_feed_all_cats.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        long long milk;
        int cat_count;
        std::cin >> milk >> cat_count;
        const int edge_count = cat_count * (cat_count - 1) / 2;
        std::vector<CatFeastDistance> distances;
        distances.reserve(static_cast<std::size_t>(edge_count));
        for(int index = 0; index < edge_count; index++){
            int first, second;
            long long distance;
            std::cin >> first >> second >> distance;
            distances.push_back({first, second, distance});
        }
        std::cout << (can_feed_all_cats(milk, cat_count, distances)
            ? "yes\n" : "no\n");
    }
}
