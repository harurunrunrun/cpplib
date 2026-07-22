// competitive-verifier: PROBLEM https://www.spoj.com/problems/HISTOGRA/

#include <iostream>
#include <vector>

#include "../../src/algorithm/other/sequence/largest_rectangle_histogram.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    while(std::cin >> size && size != 0){
        std::vector<long long> heights(static_cast<std::size_t>(size));
        for(long long& height: heights) std::cin >> height;
        std::cout << largest_rectangle_histogram(heights).area << '\n';
    }
}
