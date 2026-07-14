// competitive-verifier: PROBLEM https://www.spoj.com/problems/HELPR2D2/

#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "../../src/approximate/packing/bin_packing.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count--){
        int capacity, item_count;
        std::cin >> capacity >> item_count;

        std::vector<int> item_size;
        item_size.reserve(static_cast<std::size_t>(item_count));
        while(static_cast<int>(item_size.size()) < item_count){
            std::string token;
            std::cin >> token;
            if(token == "b"){
                int repeat, size;
                std::cin >> repeat >> size;
                item_size.insert(
                    item_size.end(),
                    static_cast<std::size_t>(repeat),
                    size
                );
            }else{
                item_size.push_back(std::stoi(token));
            }
        }

        const auto result = approximate::packing::first_fit_bin_packing(
            item_size,
            capacity
        );
        const long long waste = std::accumulate(
            result.remaining_capacity.begin(),
            result.remaining_capacity.end(),
            0LL
        );
        std::cout << result.bin_count() << ' ' << waste << '\n';
    }
}
