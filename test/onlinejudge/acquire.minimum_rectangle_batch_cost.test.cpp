// competitive-verifier: PROBLEM https://www.spoj.com/problems/ACQUIRE/
#include <iostream>
#include <vector>
#include <utility>

#include "../../src/algorithm/other/minimum_rectangle_batch_cost.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int rectangle_count;
    if(!(std::cin >> rectangle_count)) return 0;
    std::vector<RectangleBatchDimensions> rectangles(
        static_cast<std::size_t>(rectangle_count)
    );
    for(auto& rectangle: rectangles){
        std::cin >> rectangle.width >> rectangle.height;
    }
    std::cout << minimum_rectangle_batch_cost(std::move(rectangles)) << '\n';
    return 0;
}
