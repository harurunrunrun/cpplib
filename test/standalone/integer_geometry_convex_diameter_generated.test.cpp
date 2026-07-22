// competitive-verifier: STANDALONE

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/integer_geometry/distance.hpp"

namespace{

std::string to_decimal(integer_geometry::UnsignedWide value){
    if(value == 0) return "0";
    std::string result;
    while(value != 0){
        result.push_back(static_cast<char>('0' + value % 10));
        value /= 10;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count = 0;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::size_t size = 0;
        std::cin >> size;
        std::vector<integer_geometry::Point> polygon(size);
        for(auto& point: polygon){
            std::cin >> point.x >> point.y;
        }

        try{
            const std::optional<integer_geometry::UnsignedWide> answer =
                integer_geometry::convex_diameter_squared(polygon);
            std::cout << (answer ? to_decimal(*answer) : "none") << '\n';
        }catch(const std::overflow_error&){
            std::cout << "overflow\n";
        }
    }
}
